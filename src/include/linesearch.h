#ifndef __LINE_SEARCH_H__
#define __LINE_SEARCH_H__

#include <memory>
#include "fdf.h"

// Line search class implementing the algorithm described in the reference:
//
// Jorge J. More and David J. Thuente. Line Search Algorithm with Guaranteed
// Sufficient Decreases. ACM Transaction on Mathematical Software, 20: 206-307,
// 1994. 
class MoreThuente {
public: 
  // Constructor
  // 
  // func: Reference to a real-valued differentiable function
  // maxFuncEval: Maximum number of function evaluation allowed
  // mu: Sufficient decrease parameter
  // eta: Directional derivative parameter
  // alphaMin: Lower bound on the step size
  // alphaMax: Upper bound on the step size
  // xtol: Nonnegative value. The line search terminates if the relative width
  // of the interval of uncertainty is at most xtol
  MoreThuente(FDF &func, double xtol = 1e-16, size_t maxFuncEval = 20,
              double mu = 1e-4, double eta = 0.9,
              double alphaMin = 1e-20, double alphaMax = 1e20) : 
    func_{func}, xtol_{xtol}, n_{func.dim()}, nFuncEval_{0},
    maxFuncEval_{maxFuncEval}, mu_{mu}, eta_{eta}, alphaMin_{alphaMin},
    alphaMax_{alphaMax}, work_{std::unique_ptr<double []>{new double[n_]}} { }

  // Perform the line search along the direction x + alpha * p.
  //
  // x: It contains the base point for the line search on input, and contains
  //    x + alpha * p on output.
  // f: It contains the value of f at x on input, and contains the value of f at
  //    x + alpha * p on output.
  // g: It contains the gradient of f at x on input, and contains the gradient
  //    of f at x + alpha * p on output.
  // p: It contains the search direction.
  // alpha: It contains an initial estimate of a satisfactory step on input and
  //    contains the final estminate on the output.
  //
  // Return values:
  //  0: Normal return
  // -1: Improper input parameters
  // -2: Input p does not point to a descent direction
  // -3: Rounding errors prevent further progress. There may not be a step which
  //  satisifies the sufficient decrease and curvature conditions. Tolerances
  //  may be too small.
  // -4: The step is too large.
  // -5: The step is too small.
  // -6: Too many function evaluations
  // -7: Relative width of the interval of uncertainty is at most xtol. 
  int search(double *x, double &f, double *g, const double *p, double &alpha);
  
private:
  // A real-valued differentiable function
  FDF &func_;

  // A nonnegative input variable. The line search terminates if the relative
  // width of the interval of uncertainty is at most xtol. 
  double xtol_;

  // Dimension of the function
  size_t n_;
    
  // Number of calls to func_.eval() accunulated so far in the current search.
  size_t nFuncEval_; 
  
  // Maximum number of calls to func_.eval() before termination occurs. 
  size_t maxFuncEval_; 

  // Sufficient decrease parameter in equation (1.1) of the reference.
  double mu_;

  // Directional derviative parameter in equation (1.2) of the reference.
  double eta_;

  // Lower bound on the step size
  double alphaMin_;

  // Upper bound on the step size
  double alphaMax_;  

  // A scratch space used by the search routine
  std::unique_ptr<double []> work_; 
  
  // Compute a safeguard step for a linesearch and to update an interval of
  // uncertainty for a minimizer of the function.
  //
  // x, fx, dx: step with the least function value, function value at x, and
  //   gradient of f at x. x is referred alpha_l in the reference.
  // y, fy, dy: the other endpoint, function value at y, and gradient of f at
  //   y. y is referred alpha_u in the reference.
  // t, ft, dt: trial value, function value at t, and gradient of f at t. t is
  //   referred alpha_t in the reference.
  // tmin, tmax: lower and upper bound on the trial value.
  // bracket: boolean predicating if the trail value is bracketed.
  //
  // Return 0 for normal exit. 
  int updateTrial(double &x, double &fx, double &dx,
                  double &y, double &fy, double &dy,
                  double &t, double &ft, double &dt,
                  bool &backet, double tmin, double tmax) const; 
  

  // Computes the minimizer of the cubic that interpolates fu, du, fv, and
  // dv. The value of the minimizer is referred as alpha_c in the reference.
  double cubicMinimizer1(double u, double fu, double du,
                         double v, double fv, double dv) const;

  // Computes the minizer of the cubic that interpolates fu, du, fv, and dv.
  double cubicMinimizer2(double u, double fu, double du,
                         double v, double fv, double dv,
                         double xmin, double xmax) const;
  // Computes the minimizer of the quadratic that interpolates fu, du, and
  // fv. The value of the minimizer is referred as alpha_q in the reference.
  double quadMinimizer1(double u, double fu, double du,
                        double v, double fv) const; 

  // Computes the minimizer of the quadratic that interpolates fu, du, and
  // dv. The value of the minimizer does not depend on fu and is referred as
  // alpha_s in the reference. 
  double quadMinimizer2(double u, double du,
                        double v, double dv) const;
};   


#endif 

