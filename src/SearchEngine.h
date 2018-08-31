#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include <vector>
#include <cstring>
#include "Data.h" 

namespace ITR {

class SearchEngine {
public:
  // Constructor
  SearchEngine(int depth) : depth_{depth} { }
  
  // Run the comprehensive search
  void run(const Data *data); 
  
private:  
  int depth_;
  
  // This function runs depth one search
  void depthOneSearch(const Data *data);

  // This function runs depth two search
  void depthTwoSearch(const Data *data);

  // This function runs depth three search 
  void depthThreeSearch(const Data *data);

  // This function updates the search result when processing row 0 of the input
  void updateResult(std::vector<double> &result, const bool *mask,
                    const Data *data) const; 

  // Struct to store the cut information
  template<int depth> struct CutInfo {
    CutInfo(const int *src) {
      memcpy(info, src, sizeof(int) * depth * 2);
    }
    int info[depth * 2]; // (index, cut) 
  };     
}; 

} // namespace ITR



#endif 
