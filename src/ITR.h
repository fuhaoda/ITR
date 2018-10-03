#ifndef __ITR_H__
#define __ITR_H__

#include <Rcpp.h>
#include <string>
#include <vector>
#include <memory>
#include "Data.h"
#include "SearchEngine.h"

using namespace Rcpp; 

class ITR {
public:
  // Constructor
  ITR(std::string input, unsigned depth, unsigned nThreads);
  
  // Run the comprehensive search
  void run() const;
  
  // Sort the search results in descending orders of the scores
  void sort(size_t nTop); 
  
  // Return the top scores 
  NumericVector topScore() const; 
  
  // Return the index of the variables associated with the top scores 
  NumericMatrix topVar() const; 
  
  // Return the directions of the cuts associated with the top scores
  // For a continuous or ordinal variable, direction 1 means < while direction
  // 0 means >= 
  // For a nominal variable, direction 1 means included while direction 0 means
  // excluded
  NumericMatrix topDir() const;
  
  // Report the top n search results
  void report(size_t nTop) const;
  
private:
  std::unique_ptr<Data> data_;
  std::unique_ptr<SearchEngine> engine_;
  size_t nTop_; 
};

#endif 


