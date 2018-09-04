#ifndef __COVARIATE_H__
#define __COVARIATE_H__

#include <vector>
#include <set> 

namespace ITR {

// This function converts the value of continuous variables into deciles
void convertContToDeciles(std::vector<double> &cont);

// This function converts the value of ordinal variables into ranks within the
// unique set, where the rank starts from 0  
void convertOrdToRanks(std::vector<int> &arr, const std::set<int> &uniq); 

// This function converts the value of nominal variables into ranks within the
// unique set, where the rank starts from 0
void convertNomToRanks(std::vector<int> &arr, const std::set<int> &uniq); 


} // namespace ITR

#endif
