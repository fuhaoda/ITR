
// This file should contain only Rcpp exports 

// #include <iostream>
// #include "ITR.h"

// ITR::ITR(std::string input, unsigned depth, unsigned nThreads) {
//   // Load the input file
// #ifdef USE_RCPP  
//   Rprintf("Loading input data ...\n");
// #else
//   std::cout << "Loading input data ...\n";
// #endif  
//   data_ = std::make_unique<Data>(input);

//   // Construct the search engine
// #ifdef USE_RCPP  
//   Rprintf("Creating search engine with depth %d\n", depth);
// #else
//   std::cout << "Creating search engine with depth " << depth << "\n";
// #endif  
//   engine_ = std::make_unique<SearchEngine>(data_.get(), depth, nThreads);  
// }

// void ITR::run() const {
// #if USE_RCPP  
//   Rprintf("Searching %d choices ...\n", engine_->nChoices());
// #else
//   std::cout << "Searching " << engine_->nChoices() << " choices ...\n";
// #endif 
//   engine_->run(); 
// }

// rVector ITR::score(size_t nTop) const {
//   return engine_->score(nTop); 
// }

// uMatrix ITR::var(size_t nTop) const {
//   return engine_->var(nTop);
// }

// sVector ITR::cut(size_t i) const {
//   return engine_->cut(i); 
// }

// sVector ITR::dir(size_t i) const {
//   return engine_->dir(i);
// }

// #ifdef USE_RCPP

// RCPP_MODULE(ITR) {
//   using namespace Rcpp;

//   class_<ITR>("ITR")
//     .constructor<std::string, unsigned, unsigned>()
//     .const_method("run", &ITR::run)
//     .const_method("score", &ITR::score)
//     .const_method("var", &ITR::var)
//     .const_method("cut", &ITR::cut)
//     .const_method("dir", &ITR::dir)
//   ;
// }

// #endif 

#include <unordered_map> 
#include "ITR.h"

size_t register_data(const std::string &input) {
  static std::unordered_map<std::string, size_t> data_log;
  size_t retval; 

  auto it = data_log.find(input);
  if (it == data_log.end()) {
    // This data is registered for the first time

    // Load the data
    rdata.push_back(std::make_unique<Data>(input));

    // Update the data log
    retval = rdata.size() - 1;

    data_log.insert(std::make_pair(input, retval));
  } else {
    retval = it->second;
  }

  return retval;
}

/*
void load_data(const std::string &input) {

  if (data == nullptr) {
    data = std::make_unique<Data>(input);
  } else {
    data = std::unique_ptr<Data>(new Data{input});
  }
}

*/
#ifdef USE_RCPP

RCPP_MODULE(ITR) {
  using namespace Rcpp;

  class_<CompSearch>("CompSearch")
    .constructor<unsigned, unsigned, unsigned>()
    .method("run", &CompSearch::run)
    .method("score", &CompSearch::score)
    .method("var", &CompSearch::var)
    .method("cut", &CompSearch::cut)
    .method("dir", &CompSearch::dir)
    ;

  function("register_data", &register_data); 
  //function("load_data", &load_data); 
  
}

#endif 
