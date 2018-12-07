#include <algorithm>
#include <thread> 
#include <numeric>
#include <cstdio>
#include "comp_search.h"

CompSearch::CompSearch(const Data *data, unsigned depth, unsigned nthreads)
  : data_{data}, depth_{depth}
{
  if (depth != 1 && depth != 2 && depth != 3)
    throw "Invalid search depth!";

  nthreads_ = std::min(nthreads, std::thread::hardware_concurrency());
  
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
