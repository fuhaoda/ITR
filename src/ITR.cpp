#include <iostream>
#include "ITR.h"

ITR::ITR(std::string input, unsigned depth, unsigned nThreads) {
  // Load the input file
#ifdef USE_RCPP  
  Rprintf("Loading input data ...\n");
#else
  std::cout << "Loading input data ...\n";
#endif  
  data_ = std::make_unique<Data>(input);

  // Construct the search engine
#ifdef USE_RCPP  
  Rprintf("Creating search engine with depth %d\n", depth);
#else
  std::cout << "Creating search engine with depth " << depth << "\n";
#endif  
  engine_ = std::make_unique<SearchEngine>(data_.get(), depth, nThreads);  
}

void ITR::run() const {
#if USE_RCPP  
  Rprintf("Searching %d choices ...\n", engine_->nChoices());
#else
  std::cout << "Searching " << engine_->nChoices() << " choices ...\n";
#endif 
  engine_->run(); 
}

rVector ITR::score(size_t nTop) const {
  return engine_->score(nTop); 
}

uMatrix ITR::var(size_t nTop) const {
  return engine_->var(nTop);
}

sVector ITR::cut(size_t i) const {
  return engine_->cut(i); 
}

sVector ITR::dir(size_t i) const {
  return engine_->dir(i);
}

#ifdef USE_RCPP

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

#endif 
