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
  
  // Run the comprehensive search
  void run(); 

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
}; 

} // namespace ITR



#endif 
