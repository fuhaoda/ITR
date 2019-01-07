#ifndef __MULTIMIN_H__
#define __MULTIMIN_H__

#include <memory>
#include <algorithm>
#include "fdf.h"

#if 1
#include <iostream>
#endif 

// An abstract base class for minimization of a multi-dimensional function 
class MultiMin {
public:
  // Constructor
  //
  // func: Function to be minimized
  // iter: Max number of iterations allowed
  // x: starting location
  MultiMin(FDF *func, size_t iter, const std::vector<double> &x)
    : func_{func}, n_{func->dim()}, maxIter_{iter}, iter_{0}, f_{0}
  {
    x_.assign(x.begin(), x.end());
    g_.resize(n_);
    std::fill(g_.begin(), g_.end(), 0.0); 
  }

  // Virtual destructor 
  virtual ~MultiMin() { } 
  
  // Routine to move the minimizer to a better location. It returns true if more
  // iteration is needed. 
  virtual bool iterate() = 0; 

  // Solve the minimization problem. 
  void solve() {
    // Terminates either when a minimizer has been found or when the number of
    // iteration exceeds the maximum allowed limit.
    while (iter_ < maxIter_) {
      if (!iterate())
        break; 
      iter_++;
    }
  }
  
  // Copy the location of the minimizer into the provided buffer
  void x(std::vector<double> &out) const {
    out.assign(x_.begin(), x_.end()); 
  }
  
  // Return the value of the function at the minimizer
  double f() const { return f_; }
  
protected:  
  // Function being minimized
  FDF *func_;

  // Dimension of the function
  size_t n_; 
  
  // Max number of iterations allowed 
  size_t maxIter_;

  // Number of iterations taken so far
  size_t iter_; 
  
  // Location of the best minimizer found so far
  std::vector<double> x_; 

  // Value of the function evaluated at x_
  double f_;

  // Value of the gradient of the function evaluated at x_
  std::vector<double> g_; 
}; 

#endif 



