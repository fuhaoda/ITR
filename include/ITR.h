#ifndef __ITR_H__
#define __ITR_H__

#include <string>
#include <vector>
#include <iostream>
#include <memory> 
#include "Data.h"
#include "SearchEngine.h" 

namespace ITR {

template <unsigned depth>
class ITR {
public:
  // Constructor
  ITR(std::string input, unsigned nThreads = 1) {
    // Load the input file
    std::cout << "Loading input data ...\n";
    data_ = std::make_unique<Data>(input);

    // Construct the search engine
    std::cout << "Creating search engine with depth " << depth << "\n";
    engine_ = std::make_unique<SearchEngine<depth>>(data_.get(), nThreads);       
  }


  // Run the comprehensive search
  void run() const {
    std::cout << "Searching " << engine_->nChoices() << " choices ...\n";
    engine_->run(); 
  }

  // Return the top n search results
  std::vector<Treatment> report(size_t nTop = 1) const {
    return engine_->report(nTop); 
  }

private:
  std::unique_ptr<Data> data_;
  std::unique_ptr<SearchEngine<depth>> engine_; 
}; 
  
} // namespace ITR
  

#endif 


