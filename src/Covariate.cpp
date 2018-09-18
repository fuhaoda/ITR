#include <numeric>
#include <map>
#include <algorithm>
#include <utility>
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

void convertContToDeciles(std::vector<double> &cont,
                          std::vector<double> &decile) {
  decile.resize(10); 
  
  auto nSample = cont.size();
  const double scaling_factor = 10.0 / nSample;

  // Sort the continuous variable and return the indices
  auto sorted = sort_indices(cont);

  for (size_t i = 0; i < 9; i++) {
    double val = (i + 1) * nSample / 10.0 + 0.5;

    // Get the integral and fractional parts
    size_t k = static_cast<size_t>(val);
    double f = val - k;

    // Gee the values
    double xk = cont[sorted[k]];
    double xk1 = cont[sorted[k + 1]];

    decile[i] = (1 - f) * xk + f * xk1;
  }

  // Store the maximum value 
  decile[9] = cont[sorted[nSample - 1]];     
  
  // Replace the value of the continuous variable with the decile
  for (size_t i = 0; i < nSample; ++i) {
    // The jth component of the variable is in the ith sorted position
    auto j = sorted[i];

    // The percentile of the jth component is 100 * (i + 1 - 0.5) / nSample.
    cont[j] = (i + 0.5) * scaling_factor;
  }
}

void convertOrdToRanks(std::vector<int> &ord, const std::set<int> &uniq) {
  // Create a reverse map
  std::map<int, int> reverse_map;

  int rank = 0; 
  for (const auto &v : uniq)
    reverse_map.insert(std::make_pair(v, rank++));

  // Replace the ordinal values with the corresponding ranks in the unique set
  for (auto &v : ord)
    v = reverse_map[v]; 
}

void convertNomToBitMasks(std::vector<int> &ord, const std::set<int> &uniq) {
  // Create a reverse map
  std::map<int, int> reverse_map;

  int rank = 0; 
  for (const auto &v : uniq)
    reverse_map.insert(std::make_pair(v, rank++)); 

  // Replace the nominal values with the corresponding ranks in the unique set
  for (auto &v : ord) 
    v = (1 << reverse_map[v]);    
}


} // namespace ITR
