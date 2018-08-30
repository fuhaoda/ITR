#ifndef __COVARIATE_H__
#define __COVARIATE_H__

#include <vector>
#include <set> 

namespace ITR {

// This function converts the value of continuous variables into deciles
void convertToDeciles(std::vector<double> &cont);


// This function converts the value of ordinal or nominal variables into ranks
// within the unique set, where the rank starts from 0
void convertToRanks(std::vector<int> &arr, const std::set<int> &uniq); 

} // namespace ITR

#endif


// void NomCovariate::clean(int idx,
//                          const std::vector<std::vector<double>> &temp_cont,
//                          const std::vector<std::set<int>> &temp_ord,
//                          const std::vector<std::set<int>> &temp_nom) {
//   // task performed in the original code
//   // given a set of p unique values
//   // generate all the subsets.

//   // the subsets are saved in a vector
  
//   // each nominate variable is represented by 1 bit. 
//   // for each integer i from 0 to 2^p,
//   // create a set that contains the nominal values of the 1 bits.
//   // if this subset contains more than p / 2 values, skip to the next one.

//   // the number of cuts equals to the number of the vector

//   // if a nominal variable is seleted in the search, it will go over all the
//   // cuts. each cut corresponds to a subset.
  
//   // we go over the component of the covariate variable, if the value of
//   // component k is inside this subset, component k contributes to the
//   // computation. 

//   // if we are only operating 64 bits integers, this implementation will only
//   // deal with 64 nomial variables.

//   // TODO
//   // use bitset::count() to count the number of 1 bits for portability
//   // if the integer counts less than p/2 1s, save this integer, instead of the
//   // whole set
//   // save the ordered set in the range vector

//   // create a reverse mapping, for each value of the nominal variable, save its
//   // idx in the range. 
  
//   // when we go over the cuts, the cut is represented by an integer, for
//   // component k, we read its value and then get its index. We bit AND with the
//   // integer representing the cut, if the result is 1, the component contribute,
//   // otherwise, it does nothing. 
  
// }
