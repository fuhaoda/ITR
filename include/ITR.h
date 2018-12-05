#ifndef __ITR_H__
#define __ITR_H__

#include <string>
#include <vector>
#include <memory>
#include "Data.h"
#include "SearchEngine.h"
#include "Types.h"

class ITR {
public:
  // Constructor
  ITR(std::string input, unsigned depth, unsigned nThreads);
  
  // Run the comprehensive search
  void run() const;

  // Get the scores of the top n searches
  rVector score(size_t nTop) const; 

  // Get the index of the variables associated with the top scores
  uMatrix var(size_t nTop) const; 

  // Return the cut information of top score i
  sVector cut(size_t i) const; 

  // Return the direction of the cuts associated with top score i
  sVector dir(size_t i) const; 
  
private:
  std::unique_ptr<Data> data_;
  std::unique_ptr<SearchEngine> engine_;
};

#endif 


