#ifndef __ITR_H__
#define __ITR_H__

#include <Rcpp.h>
#include <string>
#include <vector>
#include <memory>
#include "Data.h"
#include "SearchEngine.h"

using namespace Rcpp; 

namespace ITR {

class ITR {
public:
  // Constructor
  ITR(std::string input, unsigned depth, unsigned nThreads);
  
  // Run the comprehensive search
  void run() const;

  // Get the scores of the top n searches
  NumericVector score(size_t nTop) const; 

  // Get the index of the variables associated with the top scores
  NumericMatrix var(size_t nTop) const; 

  // Return the cut information of top score i
  List cut(size_t i) const; 

  // Return the directions of the cuts associated with the top scores. 
  // For a continuous or ordinal variable, direction 1 means < while direction
  // 0 means >=.
  // For a nominal variable, direction 1 means included while direction 0 means
  // excluded.
  NumericMatrix dir(size_t nTop) const;

private:
  std::unique_ptr<Data> data_;
  std::unique_ptr<SearchEngine> engine_;
};

} // namespace ITR

#endif 


