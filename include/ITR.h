#ifndef __ITR_H__
#define __ITR_H__

#include <string>

namespace ITR {

class ITR { 
public:
  // Constructor
  ITR(std::string input, unsigned depth); 
  
  // Run the comprehensive search using nThreads 
  void run(unsigned nThreads = 1) const;

  // Display the top n search results
  void report(size_t nTop = 1) const; 
};

} // namespace ITR
  

#endif 


