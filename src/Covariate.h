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
