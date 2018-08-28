#include <numeric> 
#include "Covariate.h"

namespace ITR {

// Sort an input array and keep tracking of index
// https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
template <typename T>
std::vector<std::size_t> sort_indices(const std::vector<T> &v) {
  // Initialize original index locations
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // Sort indices based on comparing values in v
  std::sort(idx.begin(), idx.end(),
            [&v](std::size_t i1, std::size_t i2) {return v[i1] < v[i2];});

  return idx;
}

void ContCovariate::clean(int idx,
                          const std::vector<std::vector<double>> &temp_cont,
                          const std::vector<std::set<int>> &temp_ord,
                          const std::vector<std::set<int>> &temp_nom) {
  convertContToDeciles(temp_cont[idx]);

  // There are 10 cuts 
  nCut_ = 10;

  // The range is {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
  range_.resize(10);
  std::iota(range_.begin(), range_.end(), 1); 
}

void ContCovariate::convertContToDeciles(const std::vector<double> &cont) {
  auto nSample = cont.size(); 
  const double scaling_factor = 10.0 / nSample;

  // Sort the continuous variable and return the indices
  auto sorted = sort_indices(cont);

  // Resize the data buffer
  data_.resize(nSample);

  // Compute deciles
  for (auto i = 0; i < nSample; ++i) {
    // The jth component of the variable is in the ith sorted position
    auto j = sorted[i];

    // The percentile of component j is 100 * (i + 1 - 0.5) / nSample.
    // The decile is the floor of the above value divided by 10.
    // As only positive values are involved here, the floor is done using a cast
    // from double to int
    data_[j] = (i + 0.5) * scaling_factor;
  }
}

void OrdCovariate::clean(int idx,
                         const std::vector<std::vector<double>> &temp_cont,
                         const std::vector<std::set<int>> &temp_ord,
                         const std::vector<std::set<int>> &temp_nom) {
  
  nCut_ = temp_ord[idx].size();

  for (auto v : temp_ord[idx])
    range_.push_back(v + 1); 
}


void NomCovariate::clean(int idx,
                         const std::vector<std::vector<double>> &temp_cont,
                         const std::vector<std::set<int>> &temp_ord,
                         const std::vector<std::set<int>> &temp_nom) {
  // task performed in the original code
  // given a set of p unique values
  // generate all the subsets.

  // the subsets are saved in a vector
  
  // each nominate variable is represented by 1 bit. 
  // for each integer i from 0 to 2^p,
  // create a set that contains the nominal values of the 1 bits.
  // if this subset contains more than p / 2 values, skip to the next one.

  // the number of cuts equals to the number of the vector

  // if a nominal variable is seleted in the search, it will go over all the
  // cuts. each cut corresponds to a subset.
  
  // we go over the component of the covariate variable, if the value of
  // component k is inside this subset, component k contributes to the
  // computation. 

  // if we are only operating 64 bits integers, this implementation will only
  // deal with 64 nomial variables.

  // TODO
  // use bitset::count() to count the number of 1 bits for portability
  // if the integer counts less than p/2 1s, save this integer, instead of the
  // whole set
  // save the ordered set in the range vector

  // create a reverse mapping, for each value of the nominal variable, save its
  // idx in the range. 
  
  // when we go over the cuts, the cut is represented by an integer, for
  // component k, we read its value and then get its index. We bit AND with the
  // integer representing the cut, if the result is 1, the component contribute,
  // otherwise, it does nothing. 
  
}


} // namespace ITR
