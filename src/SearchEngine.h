#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include "Data.h" 

namespace ITR {

class SearchEngine {
public: 
  SearchEngine(unsigned search_depth);

  // Run the specified search 
  void run(Data *data); 
  
private:
  unsigned depth_;   
  void DepthOneSearch(Data *data); 
  void DepthTwoSearch(Data *data);
  void DepthThreeSearch(Data *data); 
  
}; 

} // namespace ITR



#endif 
