#include <algorithm>
#include <thread> 
#include <numeric>
#include <bitset> 
#include "comp_search.h"

CompSearch::CompSearch(const Data *data, unsigned depth,
                       unsigned nthreads) : depth_{depth} {
  if (depth != 1 && depth != 2 && depth != 3)
    throw "Invalid search depth!";

  nthreads_ = std::min(nthreads, std::thread::hardware_concurrency());

  // Pack raw action values.
  pack_actions(data->act());

  // Scale responses.
  scale_response(data->resp());

  // Convert variables into cut masks.
  set_cont_masks(data);
  set_ord_masks(data);
  set_nom_masks(data); 
    
  // Compute the total number of searches to examine
  std::vector<size_t> vidx(data_->nvar()); // TODO: change nVar name
  std::iota(vidx.begin(), vidx.end(), 0);
  combination(std::vector<size_t>{}, vidx, 'c');

  // Resize the search buffer
  choices_.resize(total_choices_);
  scores_.resize(total_choices_ << depth_);

  // Set all the search choices
  iter_ = 0;
  combination(std::vector<size_t>{}, vidx, s'); 

  // None of the search scores have been sorted
  ntop_ = 0;
  index_.resize(scores_.size()); 
}

void CompSearch::pack_actions(const std::vector<int> &act) {
  size_t q = act.size() >> 3;
  size_t r = act.size() % 8;
  act_.resize((r > 0 ? q + 1: q)); 

  // Process the first q batches of actions
  for (size_t i = 0; i < q; ++i) {
    size_t i8 = i << 3;
    act_[i] = (act[i8] << 28) | (act[i8 + 1] << 24) |
      (act[i8 + 2] << 20) | (act[i8 + 3] << 16) |
      (act[i8 + 4] << 12) | (act[i8 + 5] << 8) |
      (act[i8 + 6] << 4)  | act[i8 + 7];
  }

  // Process the remaining bits if r is not 0.
  if (r) {
    std::uint32_t val = 0;
    q <<= 3; 
    for (size_t i = 0; i < r; ++i)
      val |= act[q + i] << (28 - 4 * i);
    q >> = 3;
    act_[q] = val;
  }
}

void CompSearch::scale_response(const std::vector<double> &resp,
                                const std::vector<double> &prob) {
  size_t q = resp.size() >> 3;
  int r = static_cast<int>(resp.size() % 8);

  T0_ = 0.0;
  // Process the first q batches
  for (size_t i = 0; i < q; ++i) {
    size_t i8 = i << 3;
    auto mask = act_[i];
    for (int k = 7; k >= 0; --k) {
      resp_[i8 + k] = resp[i8 + k] / prob[i8 + k];
      T0_ += resp_[i8 + k] * (1 - (mask & 0xF));
      mask >> = 4;
    }
  }

  if (r) {
    size_t i8 = q << 3;
    auto mask = act_[q];

    // Drop the bottom 32 - 4r bits that are all zero
    mask >>= (32 - 4 * r);

    for (int k = r - 1; k >= 0; --k) {
      resp_[i8 + k] = resp[i8 + k] / prob[i8 + k];
      T0_ += resp_[i8 + k] * (1 - (mask & 0xF));
      mask >>= 4;
    }
  }
}

void CompSearch::set_cont_masks(const Data *data) {
  size_t nsample = data->nsample();
  std::vector<double> mapped(nsample);
  std::vector<size_t> sorted(nsample);
  double scaling_factor = 10.0 / nsample;
  size_t q = nsample >> 3;
  size_t r = nsample % 8;
  size_t len = q + (r > 0); 

  for (size_t i = 0; i < data->ncont(); ++i) {
    // Get raw values
    const std::vector<double> &raw = data->cont(i);

    // Sort the raw values in ascending order
    std::iota(sorted.begin(), sorted.end(), 0);
    std::sort(sorted.begin(), sorted.end(),
              [&raw](size_t i1, size_t i2) { return raw[i1] < raw[i2]; });

    // Compute the deciles of the raw values
    decile_[i].resize(10);
    for (size_t j = 0; j < 9; ++j) {
      double val = (j + 1) * nsample / 10.0 + 0.5;

      // Get the integral and fractional parts
      auto k = static_cast<size_t>(val);
      double f = val - k;

      // Get the values
      double xk = raw[sorted[k - 1]];
      double xk1 = raw[sorted[k]];

      decile_[i][j] = (1 - f) * xk + f * xk1;
    }

    decile_[i][9] = raw[sorted[nsample - 1]];

    // Convert the raw values into deciles
    for (size_t j = 0; j < nsample; ++j) {
      // The kth component of the variable is in the jth sorted position
      auto k = sorted[j];

      // The percentile of the kth component is 100 * (j + 1 - 0.5) / nsample.
      mapped[k] = (j + 0.5) * scaling_factor;
    }

    // Compute the overall variable index
    size_t vidx = i; 

    // Each continuous variable has 10 cuts, one for each decile. 
    for (int value = 1; value <= 10; ++value) {
      std::vector<std::uint32_t> mask(len); 

      for (size_t j = 0; j < q; ++j) {
        size_t j8 = j << 3;
        std::uint32_t val{0};
        // Mask for sample j8     is stored in bits 28-31
        // Mask for sample j8 + 1 is stored in bits 24-27
        // Mask for sample j8 + 2 is stored in bits 20-23
        // Mask for sample j8 + 3 is stored in bits 16-19
        // Mask for sample j8 + 4 is stored in bits 12-15
        // Mask for sample j8 + 5 is stored in bits  8-11
        // Mask for sample j8 + 6 is stored in bits  4-7
        // Mask for sample j8 + 7 is stored in bits  0-3
        for (size_t k = 0; k < 8; ++k)
          val |= (mapped[j8 + k] < value) << (28 - 4 * k);
        mask[j] = val;
      }

      if (r) {
        size_t j8 = q << 3;
        std::uint32_t val{0};
        for (size_t k = 0; k < r; ++k)
          val |= (mapped[j8 + k] < value) << (28 - 4 * k);
        mask[q] = val;
      }

      cvar_[vidx].mask.push_back(mask);
      cvar_[vidx].value.push_back(value - 1);
    }
  }
}
  
void CompSearch::set_ord_masks(const Data *data) {
  size_t nsample = data->nsample();
  std::vector<int> mapped(nsample);
  size_t q = nsample >> 3;
  size_t r = nsample % 8;
  size_t len = q + (r > 0); 
  
  for (size_t i = 0; i < data->nord(); ++i) {
    // Get raw values
    const std::vector<int> &raw = data->ord(i); 

    // Collect the unique values of the raw values.
    for (auto v : raw)
      uniq_ord_[i].insert(v);

    // Create a map from the value to its rank within the unique set.
    std::map<int, int> reverse_map; 
    int rank = 0;
    for (const auto &v : uniq_ord_[i])
      reverse_map.insert(std::make_pair(v, rank++));

    // Map the raw values into their ranks within the unique set.
    for (size_t j = 0; j < nsample; ++j)
      mapped[j] = reverse_map[raw[j]];

    // Compute the overall variable index
    size_t vidx = data->ncont() + i;

    // The number of cuts for each ordinal variable is the cardinality of the
    // unique value set.
    for (const auto &value : uniq_ord_[i]) {
      std::vector<std::uint32_t> mask(len);

      for (size_t j = 0; j < q; ++j) {
        size_t j8 = j << 3;
        std::uint32_t val{0};
        for (size_t k = 0; k < 8; ++k)
          val |= (mapped[j8 + k] <= value) << (28 - 4 * k);
        mask[j] = val;
      }

      if (r) {
        size_t j8 = q << 3;
        std::uint32_t val{0};
        for (size_t k = 0; k < r; ++k)
          val |= (mapped[j8 + k] <= value) << (28 - 4 * k);
        mask[q] = val;
      }

      cvar_[vidx].mask.push_back(mask); 
      cvar_[vidx].value.push_back(value); 
    }    
  }
}

void CompSearch::set_nom_masks(const Data *data) {
  size_t nsample = data->nsample();
  std::vector<int> mapped(nsample);
  size_t q = nsample >> 3;
  size_t r = nsample % 8;
  size_t len = q + (r > 0);

  for (size_t i = 0; i < data->nnom(); ++i) {
    // Get raw values
    const std::vector<int> &raw = data->nom(i);

    // Collect the unique values of the raw values.
    for (auto v : raw)
      uniq_nom_[i].insert(v);

    // Create a map from the value to its rank within the unique set.
    std::map<int, int> reverse_map;
    int rank = 0;
    for (const auto &v : uniq_nom_[i])
      reverse_map.insert(std::make_pair(v, rank++)); 

    // Map the raw values into bitmasks. If the order of value v in the unique
    // set is r, then the bitmask is 1 << r. Here, we assume that there are at
    // most 31 different unique values. 
    for (size_t j = 0; j < nsample; ++j)
      mapped[j] = (1 << reverse_map[raw[j]]);

    // Compute the overall variable index
    size_t vidx = data->ncont() + data->nord() + i;

    // The number of cuts for each nominal variable is the number of subsets
    // that contain no more than half of the unique values.

    // Denote the number of unique values by p. Here, we loop through 0 to
    // 2^p. Each iterator is interpreted as a bitmask, where bit j means the jth
    // element in the unique set. For each integer, if the number of 1 bits is
    // no more than half of p, it then represents a valid cut, and the subset
    // consists of the elements corresponding to the 1 bits in the integer.
    size_t p = uniq_nom_[i].size();
    size_t max = 1u << p;
    size_t half = p / 2;
    for (size_t value = 0; value < max; ++value) {
      std::bitset<32> subset(value);
      if (subset.count() <= half) {
        std::vector<std::uint32_t> mask(len);

        for (size_t j = 0; j < q; ++j) {
          size_t j8 = j << 3;
          std::uint32_t val{0};
          for (size_t k = 0; k < 8; ++k)
            val |= ((mapped[j8 + k] & value) > 0) << (28 - 4 * k);

          mask[j] = val;
        }

        if (r) {
          size_t j8 = q << 3;
          std::uint32_t val{0};
          for (size_t k = 0; k < r; ++k)
            val |= ((mapped[j8 + k] & value) > 0) << (28 - 4 * k);
          mask[q] = val;
        }

        cvar_[vidx].mask.push_back(mask);
        cvar_[vidx].value.push_back(static_cast<int>(value));
      }
    }
  }
}



std::set<int> Data::uniq_nom(size_t i) {
  assert(i < nnom_); 

  std::set<int> uniq;
  for (auto v : nom_[i])
    uniq.insert(v);
  return uniq; 
}
