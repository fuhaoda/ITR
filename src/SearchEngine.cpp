#include "SearchEngine.h"

#include <iostream> 

namespace ITR {

SearchEngine::SearchEngine(unsigned search_depth) {
  depth_ = search_depth; 
}; 
  
void SearchEngine::run(Data *data) {
  switch (depth_) {
  case 1:
    DepthOneSearch(data);
    break;
  case 2:
    DepthTwoSearch(data);
    break;
  case 3:
    DepthThreeSearch(data);
    break;
  default:
    break;
  }
}

void SearchEngine::DepthOneSearch(Data *data) {
  std::cout << "Depth one search ...\n"; 
}

void SearchEngine::DepthTwoSearch(Data *data) {
  std::cout << "Depth two search ...\n";
}

void SearchEngine::DepthThreeSearch(Data *data) {
  std::cout << "Depth three search ...\n"; 
}




} // namespace ITR
