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

void ITR::sort(size_t nTop) {
  Rprintf("Sorting the top %d results ...\n", nTop);
  engine_->sort(nTop);
  nTop_ = nTop; 
}

NumericVector ITR::topScore() const {
  return engine_->topScore(nTop_); 
}

NumericMatrix ITR::topVar() const {
  return engine_->topVar(nTop_); 
}

List ITR::cut(size_t i) const {
  // Cap i at nTop_
  if (i > nTop_)
    i = nTop_; 
  return engine_->cut(i);  
}

NumericMatrix ITR::topDir() const {
  return engine_->topDir(nTop_); 
}

RCPP_MODULE(ITR) {
  using namespace Rcpp;

  class_<ITR>("ITR")
    .constructor<std::string, unsigned, unsigned>()
    .method("sort", &ITR::sort)
    .const_method("run", &ITR::run)
    .const_method("topScore", &ITR::topScore)
    .const_method("topVar", &ITR::topVar)
    .const_method("cut", &ITR::cut)
    .const_method("topDir", &ITR::topDir)
  ;
}

