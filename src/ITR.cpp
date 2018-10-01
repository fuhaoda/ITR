#include <iostream>
#include "ITR.h"

namespace ITR {

ITR::ITR(std::string input, unsigned depth, unsigned nThreads) {
  // Load the input file
  std::cout << "Loading input data ...\n";
  data_ = std::make_unique<Data>(input);
  
  // Construct the search engine
  std::cout << "Creating search engine with depth " << depth << "\n";
  engine_ = std::make_unique<SearchEngine>(data_.get(), depth, nThreads);  
}

void ITR::run() const {
  std::cout << "Searching " << engine_->nChoices() << "  choices ...\n";
  engine_->run(); 
}

void ITR::report(size_t nTop) const {
  engine_->report(nTop); 
}

} // namespace ITR
