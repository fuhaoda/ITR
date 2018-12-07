#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef USE_RCPP
#include <Rcpp.h>

// Vector of real numbers 
using rVector = Rcpp::NumericVector; 

// Vector of strings
using sVector = Rcpp::List; 

// Matrix of unsigned integers
using uMatrix = Rcpp::NumericMatrix;

#else

#include <vector>
#include <string>
#include <cstring> 

using rVector = std::vector<double>;
using sVector = std::vector<std::string>;

class uMatrix {
public:
  uMatrix(size_t row, size_t col) : col_{col} {
    val_.resize(row * col);
  }

  size_t operator()(size_t i, size_t j) const {
    return val_[i * col_ + j];
  }

  size_t &operator()(size_t i, size_t j) {
    return val_[i * col_ + j];
  }

private:
  size_t col_; 
  std::vector<size_t> val_;
};

#endif 


#endif
