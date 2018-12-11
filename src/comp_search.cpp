#include <algorithm>
#include <thread> 
#include <numeric>
#include <bitset>
#include <sstream>
#include <map>
#include "comp_search.h"

CompSearch::CompSearch(unsigned input, unsigned depth,
                       unsigned nthreads) : depth_{depth} {
  if (depth != 1 && depth != 2 && depth != 3)
    throw "Invalid search depth!";

  nthreads_ = std::min(nthreads, std::thread::hardware_concurrency());

  Data *data = rdata[input].get(); 
  
  nsample_ = data->nsample();
  ncont_ = data->ncont();
  nord_ = data->nord();
  nnom_ = data->nnom(); 

  decile_.resize(ncont_);
  uniq_ord_.resize(nord_);
  uniq_nom_.resize(nnom_);
  cvar_.resize(ncont_ + nord_ + nnom_); 
  
  // Pack raw action values.
  pack_actions(data->act());

  // Scale responses.
  scale_response(data->resp(), data->prob());

  // Convert variables into cut masks.
  set_cont_masks(data);
  set_ord_masks(data);
  set_nom_masks(data); 
    
  // Compute the total number of searches to examine
  std::vector<size_t> vidx(data->nvar()); 
  std::iota(vidx.begin(), vidx.end(), 0);
  combination(std::vector<size_t>{}, vidx, 'c');

  // Resize the search buffer
  choices_.resize(total_choices_);
  scores_.resize(total_choices_ << depth_);

  // Set all the search choices
  iter_ = 0;
  combination(std::vector<size_t>{}, vidx, 's'); 

  // None of the search scores have been sorted
  ntop_ = 0;
  index_.resize(scores_.size()); 
}

void CompSearch::pack_actions(const std::vector<int> &act) {
  size_t q = nsample_ >> 3;
  size_t r = nsample_ % 8;
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
    q >>= 3;
    act_[q] = val;
  }
}

void CompSearch::scale_response(const std::vector<double> &resp,
                                const std::vector<double> &prob) {  
  size_t q = nsample_ >> 3;
  int r = static_cast<int>(nsample_ % 8);
  resp_.resize(nsample_);

  T0_ = 0.0;
  // Process the first q batches
  for (size_t i = 0; i < q; ++i) {
    size_t i8 = i << 3;
    auto mask = act_[i];
    for (int k = 7; k >= 0; --k) {
      resp_[i8 + k] = resp[i8 + k] / prob[i8 + k];
      T0_ += resp_[i8 + k] * (1 - (mask & 0xF));
      mask >>= 4;
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
  std::vector<double> mapped(nsample_);
  std::vector<size_t> sorted(nsample_);
  double scaling_factor = 10.0 / nsample_;
  size_t q = nsample_ >> 3;
  size_t r = nsample_ % 8;
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
      double val = (j + 1) * nsample_ / 10.0 + 0.5;

      // Get the integral and fractional parts
      auto k = static_cast<size_t>(val);
      double f = val - k;

      // Get the values
      double xk = raw[sorted[k - 1]];
      double xk1 = raw[sorted[k]];

      decile_[i][j] = (1 - f) * xk + f * xk1;
    }

    decile_[i][9] = raw[sorted[nsample_ - 1]];

    // Convert the raw values into deciles
    for (size_t j = 0; j < nsample_; ++j) {
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
  std::vector<int> mapped(nsample_);
  size_t q = nsample_ >> 3;
  size_t r = nsample_ % 8;
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
    for (size_t j = 0; j < nsample_; ++j)
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
  std::vector<int> mapped(nsample_);
  size_t q = nsample_ >> 3;
  size_t r = nsample_ % 8;
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
    for (size_t j = 0; j < nsample_; ++j)
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

void CompSearch::combination(std::vector<size_t> curr,
                             std::vector<size_t> option, char mode) {
  // Get the number of variables to choose
  size_t nchoices = depth_ - curr.size();
  if (nchoices == 0) {
    // Need to choose no more
    if (mode == 'c') {
      count_choices(curr);
    } else {
      set_choices(curr);
    }
  } else if (option.size() < nchoices) {
    // Does not have enough to choose from
    return;
  } else {
    auto curr1 = curr;
    curr1.insert(curr1.end(), option.begin(), option.begin() + 1);
    option.erase(option.begin());

    // Choose `nchoices` from options
    combination(curr, option, mode);
    // Choose `nchoices - 1` from options
    combination(curr1, option, mode);
  }
}

void CompSearch::count_choices(const std::vector<size_t> &choices) {
  size_t nchoices = 1;
  for (const auto &vidx : choices)
    nchoices *= cvar_[vidx].value.size(); 
  total_choices_ += nchoices;
}

void CompSearch::set_choices(const std::vector<size_t> &choices) {
  std::vector<size_t> cidx;
  std::vector<size_t> cbound;
  for (const auto &vidx : choices) {
    cidx.push_back(0);
    cbound.push_back(cvar_[vidx].value.size());
  }

  set_choices_helper(choices, cidx, cbound, 0);
}

void CompSearch::set_choices_helper(const std::vector<size_t> &vidx,
                                    std::vector<size_t> cidx,
                                    std::vector<size_t> &cbound,
                                    size_t curr) {
  if (cidx[curr] == cbound[curr]) {
    return;
  } else {
    if (curr < depth_ - 1) {
      set_choices_helper(vidx, cidx, cbound, curr + 1);
    } else {
      for (size_t d = 0; d < depth_; ++d) {
        choices_[iter_].vidx[d] = vidx[d];
        choices_[iter_].cidx[d] = cidx[d];
      }
      iter_++;
    }

    cidx[curr]++;
    set_choices_helper(vidx, cidx, cbound, curr);
  }
}

void CompSearch::run() {
  std::vector<std::thread> threads(nthreads_);

  for (size_t i = 0; i < nthreads_; ++i)
    threads[i] = std::thread(&CompSearch::worker, this, i);

  for (auto &th : threads)
    th.join(); 
}

void CompSearch::worker(size_t tid) {
  // Compute the range of searches the worker needs to perform.
  size_t first = 0, last = 0;
  auto nchoice = choices_.size();
  auto per_worker = nchoice / nthreads_;
  auto remainder = nchoice % nthreads_;

  if (tid < remainder) {
    first = (per_worker + 1) * tid;
    last = first + per_worker + 1;
  } else {
    first = per_worker * tid + remainder;
    last = first + per_worker;
  }

  size_t stride = 1u << depth_;

  for (size_t i = first; i < last; ++i) {
    std::vector<double> v(1u << (depth_ + 1), 0.0);
    double *ans = scores_.data() + i * stride;

    std::vector<const std::uint32_t *> m(depth_);
    for (size_t d = 0; d < depth_; ++d) {
      auto vidx = choices_[i].vidx[d];
      auto cidx = choices_[i].cidx[d];
      m[d] = cvar_[vidx].mask[cidx].data();
    }

    size_t q = nsample_ >> 3;
    size_t r = nsample_ % 8;

    // We sort data into different buckets based on the values of response and
    // cut masks. For example, there are 16 buckets if depth is 3:
    // (0000)_2 (0001)_2 (0010)_2 (0011)_2
    // (0100)_2 (0101)_2 (0110)_2 (0111)_2
    // (1000)_2 (1001)_2 (1010)_2 (1011)_2
    // (1100)_2 (1101)_2 (1110)_2 (1111)_2 
    for (size_t j = 0; j < q; ++j) {
      size_t idx = act_[j];
      for (size_t d = 0; d < depth_; ++d)
        idx += m[d][j] << (depth_ - d);

      size_t j8 = j << 3;
      for (int k = 7; k >= 0; --k) {
        // Read the bottom 4 bits
        v[idx & 0xF] += resp_[j8 + k];
        // Drop the bottom 4 bits
        idx >>= 4;
      }
    }

    if (r) {
      size_t idx = act_[q];
      for (size_t d = 0; d < depth_; ++d)
        idx += m[d][q] << (depth_ - d);

      // Drop the bottom bits that are all zero.
      idx >>= (32 - 4 * r);

      size_t j8 = q << 3;
      for (int k = static_cast<int>(r) - 1; k >= 0; --k) {
        v[idx & 0xF] += resp_[j8 + k];
        idx >>= 4;
      }
    }

    for (size_t j = 0; j < stride; ++j)
      ans[j] = v[2 * j + 1] - v[2 * j]; 
  }
}

void CompSearch::report_helper(size_t &ntop) {
  // Cap ntop
  ntop = std::min(ntop, scores_.size() - 1);

  if (ntop > ntop_) {
    // We need to sort again
    ntop_ = ntop;
    std::iota(index_.begin(), index_.end(), 0);
    std::partial_sort(index_.begin(), index_.begin() + ntop_, index_.end(),
                      [&](size_t i1, size_t i2) {
                        return scores_[i1] > scores_[i2];
                      });
  }
}

rVector CompSearch::score(size_t ntop) {
  report_helper(ntop); 

  rVector retval(ntop);
  double scale = 1.0 / nsample_;
  for (size_t i = 0; i < ntop; ++i) {
    size_t sid = index_[i]; // search id
    retval[i] = (T0_ + scores_[sid]) * scale;
  }

  return retval; 
}

uMatrix CompSearch::var(size_t ntop) {
  report_helper(ntop);

  uMatrix retval(ntop, depth_);
  for (size_t i = 0; i < ntop; ++i) {
    size_t sid = index_[i];     // search id
    size_t cid = sid >> depth_; // choice id 
    for (size_t d = 0; d < depth_; ++d)
      retval(i, d) = choices_[cid].vidx[d];
  }

  return retval; 
}

sVector CompSearch::cut(size_t i) {
  report_helper(i);
  sVector out(depth_); 

#ifdef USE_RCPP
  // i is passed through R where array index starts from 1. Need to decrement
  // before accessing the value.
  i--;
#endif

  size_t sid = index_[i];     // search id
  size_t cid = sid >> depth_; // choice id
  for (size_t d = 0; d < depth_; ++d)
    out[d] = cut_val(choices_[cid].vidx[d], choices_[cid].cidx[d]);
  
  return out;       
}

sVector CompSearch::dir(size_t i) {
  report_helper(i);
  sVector out(depth_);

#ifdef USE_RCPP
  i--;
#endif

  size_t sid = index_[i];     // search id
  size_t cid = sid >> depth_; // choice id
  size_t mask = sid % (1 << depth_);

  for (size_t d = 0; d < depth_; ++d)
    out[d] = cut_dir(choices_[cid].vidx[d],
                     mask & (1u << (depth_ - 1u - d)));
  return out; 
}

std::string CompSearch::cut_val(size_t vidx, size_t cidx) const {
  std::stringstream info; 
  if (vidx < ncont_) {
    info << decile_[vidx][cidx] << " (percentile " << (cidx + 1) * 10 << ")";
  } else if (vidx < ncont_ + nord_) {
    info << cvar_[vidx].value[cidx] << " (" << cidx + 1 << " out of "
         << uniq_nom_[vidx - ncont_].size() << ")";
  } else {
    auto subset = cvar_[vidx].value[cidx];
    if (subset == 0) {
      info << "None";
    } else {
      unsigned iter = 0;
      for (auto const &v : uniq_ord_[vidx - ncont_ - nord_]) {
        if (subset & (1 << (iter++)))
          info << v << " ";
      }
    }
  }

  return info.str(); 
}


std::string CompSearch::cut_dir(size_t vidx, size_t m) const {
  std::string str;
  if (vidx < ncont_ + nord_) {
    str = (m ? " < " : " >= ");
  } else {
    str = (m ? " in " : " not in ");
  }

  return str; 
}
