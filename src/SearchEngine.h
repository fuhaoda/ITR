#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include <vector>
#include <cstring>
#include "Data.h" 

namespace ITR {

class SearchEngine {
public:
  // Constructor
  SearchEngine(unsigned depth, const Data *data); 

  // Return the number of searches to examine
  size_t nChoices() const { return log_.size(); }

  // Run the comprehensive search
  void run(unsigned nThreads); 

  // Report the best results
  void report(size_t nTop);
  
private:
  // Depth of the search  
  int depth_;

  // Data associated with the search
  const Data *data_; 

  struct Meta {
    // Chices of variables used in the search    
    std::vector<size_t> vIdx;

    // Choices of cuts used in the search
    std::vector<size_t> cIdx;

    // Best result among the 2^(depth + 1) searches formed by the above variable
    // and cut combinations
    double result;

    // Rank of the best value with the 2^(depth + 1) searches
    size_t rank;
  };

  // Search history
  std::vector<Meta> log_;

  // This function sets all the search choices of the given depth
  void setSearchChoices(); 
    
  // This function is the worker function for the search
  void worker(size_t tid, unsigned nThreads);

  // This function is a helper function that determines the range of searches
  // worker tid needs to perfrom
  void setRange(size_t tid, unsigned nThreads, size_t &first, size_t &last);
}; 

} // namespace ITR



#endif 
