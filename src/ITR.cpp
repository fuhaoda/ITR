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

#ifdef USE_RCPP

RCPP_MODULE(ITR) {
  using namespace Rcpp;

  function("register_data", &register_data); 

  class_<CompSearch>("CompSearch")
    .constructor<unsigned, unsigned>()
    .method("preprocess", &CompSearch::preprocess)
    .method("run", &CompSearch::run)
    .method("score", &CompSearch::score)
    .method("var", &CompSearch::var)
    .method("cut", &CompSearch::cut)
    .method("dir", &CompSearch::dir)
    ;
  
}

#endif 
