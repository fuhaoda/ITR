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

NumericVector ITR::score(size_t nTop) const {
  return engine_->score(nTop); 
}

NumericMatrix ITR::var(size_t nTop) const {
  return engine_->var(nTop);
}

List ITR::cut(size_t i) const {
  return engine_->cut(i); 
}

NumericMatrix ITR::dir(size_t nTop) const {
  return engine_->dir(nTop); 
}

RCPP_MODULE(ITR) {
  using namespace Rcpp;

  class_<ITR>("ITR")
    .constructor<std::string, unsigned, unsigned>()
    .const_method("run", &ITR::run)
    .const_method("score", &ITR::score)
    .const_method("var", &ITR::var)
    .const_method("cut", &ITR::cut)
    .const_method("dir", &ITR::dir)
  ;
}

