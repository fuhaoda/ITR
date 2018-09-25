#include <memory> 
#include <iostream> 
#include "ITR.h"
#include "Data.h"
#include "SearchEngine.h"

namespace ITR {

std::unique_ptr<Data> data;
std::unique_ptr<SearchEngine> engine; 

ITR::ITR(std::string input, unsigned depth) {
  // Load the input file
  std::cout << "Loading input data ...\n";
  data = std::make_unique<Data>(input); 

  // Construct the search engine
  std::cout << "Creating search engine with depth " << depth << "\n";
  engine = std::make_unique<SearchEngine>(depth, data.get());  
}

void ITR::run(unsigned nThreads) const {
  std::cout << "Searching " << engine->nChoices() << " choices ...\n";
  engine->run(nThreads); 
} 


void ITR::report(size_t nTop) const {
  std::cout << "Best " << nTop << " results: ...\n";
  engine->report(nTop); 
}


} // namespace ITR


