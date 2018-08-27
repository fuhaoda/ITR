#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

/// \file
/// \brief Declaration of SearchEngine class 


#include "Data.h" 

namespace ITR {

class SearchEngine {
public:
  /// Constructor
  ///
  /// \param[in] depth Depth of the comprehensive search
  SearchEngine(unsigned depth); 
  

  /// Run the specified search
  ///
  /// \param[in] data Input data for the search   
  void run(Data *data); 
  
private:
  unsigned depth_;   
  void DepthOneSearch(Data *data); 
  void DepthTwoSearch(Data *data);
  void DepthThreeSearch(Data *data); 
}; 

} // namespace ITR



#endif 
