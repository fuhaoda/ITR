#include <algorithm>
#include <thread> 
#include <numeric>
#include <cstdio>
#include "comp_search.h"

CompSearch::CompSearch(const Data *data, unsigned depth,
                       unsigned nthreads) : depth_{depth}
{
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
