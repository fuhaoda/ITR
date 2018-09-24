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
  size_t nChoices() const { return choices_.size() << depth_; }

  // Run the comprehensive search
  void run(unsigned nThreads); 

  // Report the best results
  void report(size_t nTop);
  
private:
  // Depth of the search  
  size_t depth_;
  size_t totalChoices_{0};
  
  // Data associated with the search
  const Data *data_;

  struct Meta {
    // Choices of variables used in the search
    std::vector<size_t> vIdx;

    // Choices of cuts used in the search 
    std::vector<size_t> cIdx;
  };

  // Choices of all the variables and cuts combination used in the search
  std::vector<Meta> choices_;   

  // Values of all the search results
  std::vector<double> results_; 

  // This function sets all the search choices of the given depth
  void setSearchChoices(); 

  // This function computes the number of search choices formed by the
  // combination of variables i1, ..., id, where 0 <= i1 < i2 < ... < id < max
  void countChoices(std::vector<size_t> sofar, std::vector<size_t> rest,
                    size_t restDepth);


  // This function is the worker function for the search
  void worker(size_t tid, unsigned nThreads);

  // This function is a helper function that determines the range of searches
  // worker tid needs to perfrom
  void setRange(size_t tid, unsigned nThreads, size_t &first, size_t &last);
}; 

} // namespace ITR



#endif 
