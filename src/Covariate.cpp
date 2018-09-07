#include <numeric>
#include <map>
#include <algorithm>
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

void convertContToDeciles(std::vector<double> &cont) {
  auto nSample = cont.size();
  const double scaling_factor = 10.0 / nSample;

  // Sort the continuous variable and return the indices
  auto sorted = sort_indices(cont); 
  
  // Replace the value of the continuous variable with the decile
  for (auto i = 0; i < nSample; ++i) {
    // The jth component of the variable is in the ith sorted position
    auto j = sorted[i];

    // The percentile of the jth component is 100 * (i + 1 - 0.5) / nSample.
    cont[j] = (i + 0.5) * scaling_factor;
  }
}

void convertOrdToRanks(std::vector<int> &ord, const std::set<int> &uniq) {
  auto largest = *uniq.rbegin();

  // Create a reverse table
  // reverse_table[i] = j means value i is rank j, where the rank starts from 0. 
  std::vector<int> reverse_table(largest + 1);
  
  int rank = 0;
  for (auto it = uniq.begin(); it != uniq.end(); ++it) 
    reverse_table[*it] = rank++;

  // Replace the ordinal values with the corresponding ranks in the unique set
  for (auto &v : ord)
    v = reverse_table[v]; 
}

void convertNomToBitMasks(std::vector<int> &ord, const std::set<int> &uniq) {
  // Create a reverse map
  std::map<int, int> reverse_map;

  int rank = 0; 
  for (auto &v : uniq)
    reverse_map[v] = rank++;

  // Replace the nominal values with the corresponding ranks in the unique set
  for (auto &v : ord) 
    v = (1 << reverse_map[v]);    
}


} // namespace ITR
