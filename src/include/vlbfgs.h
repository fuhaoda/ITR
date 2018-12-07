#ifndef __VLBFGS_H__
#define __VLBFGS_H__

#include <memory>
#include "multimin.h"
#include "linesearch.h"


// Reference
// https://papers.nips.cc/paper/5333-large-scale-l-bfgs-using-mapreduce.pdf

class VLBFGS final : public MultiMin {
public:
  // Constructor
  //
  // func: Function being minimized
  // maxIter: Max number of iterations allowed
  // x: Starting location
  // m: Number of corrections used in the BFGS update
  // eps: A positive parameter controlling the accuracy of the solution
  // xtol: Estimate of the machine precision
  VLBFGS(FDF &func, size_t maxIter, const double *x, size_t m,
         double eps, double xtol);

  // One iteration of VL-BFGS
  bool iterate() override;

private:
  // Number of corrections used in the BFGS update.
  size_t m_;

  // Positive value controlling the accuracy with which the solution is to be
  // found. Optimization is terminated when
  // ||G|| < eps * max(1, ||x||)
  // where ||.|| denotes the Euclidean norm and G is the gradient of the
  // function being minimized.
  double eps_;

  // Line search object
  std::unique_ptr<MoreThuente> mt_;

  // Storage for the number alphas used in the formula that computes H * g
  std::unique_ptr<double []> alpha_;

  // Storage for the (2m + 1) b vectors. The first m are the search
  // direction vectors,  i.e., the s variables. The next m are the
  // gradient difference vectors, i.e., the y variables. The last slot saves the
  // gradient direction at the latest minimizer location, meaning the content
  // will be updated in the end of an iteration when a new gradient direction
  // becomes available.
  std::unique_ptr<double []> b_; 

  // Storage for the inner product matrix between all pairs of b vectors. As the
  // storage for vectors b are organized in circular fashion, the B matrix is
  // circular in both the row and column directions.
  std::unique_ptr<double []> B_;

  // Storage for the expansion coefficient of the new search direction over the
  // m search directions, m gradient differences, and 1 new gradient
  // direction. The content of delta is arranged in the same circular fashion as
  // that for vectors b. 
  std::unique_ptr<double []> delta_;

  // Update the dot product between b vectors
  void updateMatrixB(double gnorm); 
}; 

#endif 
