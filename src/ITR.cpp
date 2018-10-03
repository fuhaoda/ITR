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

NumericMatrix ITR::topDir() const {
  return engine_->topDir(nTop_); 
}

void ITR::report(size_t nTop) const {
  engine_->report(nTop); 
}

RCPP_MODULE(ITR) {
  using namespace Rcpp;

  class_<ITR>("ITR")
    .constructor<std::string, unsigned, unsigned>()
    .method("run", &ITR::run)
    .method("sort", &ITR::sort)
    .method("topScore", &ITR::topScore)
    .method("topVar", &ITR::topVar)
    .method("topDir", &ITR::topDir)
    .method("report", &ITR::report)
  ;
}

