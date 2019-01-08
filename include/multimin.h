#ifndef __MULTIMIN_H__
#define __MULTIMIN_H__

#include <memory>
#include <algorithm>
#include "fdf.h"

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

  // Routine to move the minimizer to a better location. It returns 0 if the
  // iteration can be terminated. It returns 1 if more iterations are needed. It
  // returns -1 if error happens when trying to move to the new location.
  virtual int iterate() = 0; 

  // Solve the minimization problem. It returns 0 if the solution is found
  // within the allowed iterations. It returns 1 if no solution is found after
  // the maximum allowed iterations. It returns -1 if error happens during any
  // iteration. 
  int solve() {
    while (iter_ < maxIter_) {
      auto status = iterate();

      if (status == 0) {
        // A solution has been found. Terminate.
        return 0; 
      } else if (status == -1) {
        // A problem has occured during the iteration. Terminate.
        return -1; 
      }

      iter_++; 
    }

    return 1; 
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



