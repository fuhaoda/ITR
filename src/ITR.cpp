#include <iostream>
#include "ITR.h"

ITR::ITR(std::string input, unsigned depth, unsigned nThreads) {
  // Load the input file
  Rprintf("Loading input data ...\n");
  data_ = std::make_unique<Data>(input);

  // Construct the search engine
  Rprintf("Creating search engine with depth %d\n", depth); 
  engine_ = std::make_unique<SearchEngine>(data_.get(), depth, nThreads);  
}

void ITR::run() const {
  Rprintf("Searching %d choices ...\n", engine_->nChoices()); 
  engine_->run(); 
}

void ITR::report(size_t nTop) const {
  engine_->report(nTop); 
}

RCPP_MODULE(ITR) {
  using namespace Rcpp;

  class_<ITR>("ITR")
    .constructor<std::string, unsigned, unsigned>("Constructor")
    .method("report", &ITR::report)
    .method("run", &ITR::run)
  ;
}

