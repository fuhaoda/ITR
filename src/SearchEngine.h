#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include "Data.h" 

namespace ITR {

class SearchEngine {
public:
  // Constructor
  SearchEngine(unsigned depth) : depth_{depth} { }
  
  // Run the comprehensive search
  void run(Data *data); 
  
private:
  unsigned depth_;
  void depthOneSearch(Data *data); 
  void depthTwoSearch(Data *data);
  void depthThreeSearch(Data *data); 
}; 

} // namespace ITR



#endif 
