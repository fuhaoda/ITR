#ifndef __MULTIMIN_H__
#define __MULTIMIN_H__

#include <memory>
#include <cstring> 
#include "fdf.h"

// An abstract base class for minimization of a multi-dimensional function 
class MultiMin {
public:
  // Constructor
  //
  // func: Function to be minimized
  // iter: Max number of iterations allowed
  // x: starting location 
  MultiMin(FDF &func, size_t iter, const double *x)
    : func_{func}, n_{func.dim()}, maxIter_{iter}, iter_{0}, f_{0}
  {
    x_ = std::unique_ptr<double []>{new double[n_]};
    g_ = std::unique_ptr<double []>{new double[n_]{0.0}};

    memcpy(x_.get(), x, sizeof(double) * n_); 
  }

  
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
  void x(double *out) const {
    memcpy(out, x_.get(), sizeof(double) * n_);
  }
  
  // Return the value of the function at the minimizer
  double f() const { return f_; }
  
protected:  
  // Function being minimized
  FDF &func_;

  // Dimension of the function
  size_t n_; 
  
  // Max number of iterations allowed 
  size_t maxIter_;

  // Number of iterations taken so far
  size_t iter_; 
  
  // Location of the best minimizer found so far
  std::unique_ptr<double []> x_; 

  // Value of the function evaluated at x_
  double f_;

  // Value of the gradient of the function evaluated at x_
  std::unique_ptr<double []> g_; 
}; 





#endif 



