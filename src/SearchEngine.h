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
  int depth_;  // Depth of the search
  const Data *data_; // Data associated with the search

  struct MetaData {
    std::vector<size_t> vIdx; // Choices of variables used in the search
    std::vector<size_t> cIdx; // Choices of cut levels used in the search

    // There are 2^(depth + 1) searches formed by the above variable and cut
    // level combinations.
    double result;            // Best values among the 2^(depth + 1) searches
    size_t rank;              // Rank of the best value within the 2^(depth + 1)
                              // searches
  };
  
  std::vector<MetaData> log_; // Search history 

  // This function discovers all the choices for depth one search
  void setDepthOneChoices();

  // This function discovers all the choices for depth two search
  void setDepthTwoChoices();

  // This function discovers all the choices for depth three search
  void setDepthThreeChoices();
  
  // This funciton is the worker function for depth one search
  void runDepthOneSearch(size_t tid, unsigned nThreads);

  // This function is the worker function for depth two search
  void runDepthTwoSearch(size_t tid, unsigned nThreads);

  // This function is the worker function for depth three search
  void runDepthThreeSearch(size_t i, unsigned nThreads);

  // This is a helper function that determines the range of searches performed
  // by each worker.
  void setSearchRange(size_t tid, unsigned nThreads,
                      size_t &firstSearchID, size_t &lastSearchID);   
}; 

} // namespace ITR



#endif 
