#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include <vector>
#include <cstring>
#include "Data.h" 

namespace ITR {

class SearchEngine {
public:
  // Constructor
  SearchEngine(int depth, const Data *data); 
  
  // Run the comprehensive search
  void run(); 
  
private:
  int depth_;  // Depth of the search
  const Data *data_; // Data associated with the search
  std::vector<int> choices_; // Choices examined in the search
  std::vector<double> result_; // Results associated with each choice

  // This function discovers all the choices for depth one search
  void setDepthOneChoices();

  // This function discovers all the choices for depth two search
  void setDepthTwoChoices();

  // This function discovers all the choices for depth three search
  void setDepthThreeChoices();
  

  
  // // This function runs depth one search
  // void depthOneSearch(const Data *data);

  // // This function runs depth two search
  // void depthTwoSearch(const Data *data);

  // // This function runs depth three search 
  // void depthThreeSearch(const Data *data);

  // // Struct to store the cut information
  // template<int depth> struct CutInfo {
  //   CutInfo(const int *src) {
  //     memcpy(info, src, sizeof(int) * depth * 2);
  //   }
  //   int info[depth * 2]; // (index, cut) 
  // };     
}; 

} // namespace ITR



#endif 
