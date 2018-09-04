#ifndef __ITR_H__
#define __ITR_H__

#include <string>

namespace ITR {

class ITR { 
public:
  // Constructor
  ITR(std::string input, unsigned depth); 
  
  // Run the comprehensive search 
  void run() const;

  // Display the top n search results
  void report(int nTop = 1) const; 
};

} // namespace ITR
  

#endif 


