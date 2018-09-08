#include <memory> 
#include <iostream> 
#include "ITR.h"
#include "Data.h"
#include "SearchEngine.h"


std::unique_ptr<ITR::Data> data;
std::unique_ptr<ITR::SearchEngine> engine; 

namespace ITR {

ITR::ITR(std::string input, unsigned depth) {
  // Load the input file
  std::cout << "Loading input data ...\n";
  data = std::make_unique<Data>(input); 

  // Construct the search engine
  std::cout << "Creating search engine with depth " << depth << "\n";
  engine = std::make_unique<SearchEngine>(depth, data.get());  
}

void ITR::run() const {
  std::cout << "Searching ...\n";
  engine->run(); 
} 


void ITR::report(size_t nTop) const {
  std::cout << "Best " << nTop << " results: ...\n";
  engine->report(nTop); 
}


} // namespace ITR


