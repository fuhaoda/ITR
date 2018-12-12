#ifndef __ANGLE_BASED_CLASSIFIER_H__
#define __ANGLE_BASED_CLASSIFIER_H__

#include "fdf.h"

class AngleBasedClassifier final : public FDF {
public:
  // Constructor
  AngleBasedClassifier(); 

  // Preprocess raw data set i.
  void preprocess(unsigned i);

  // Solve the nonlinear optimization problem.
  void run(); 

  // Evaluate the function at location x.
  void eval(const std::vector<double> &x, double &f) const override;

  // Evaluate the gradient of the function at location x.
  void eval(const std::vector<double> &x,
            std::vector<double> &g) const override;

  // Evaluate the value of the function and its gradient at location x.
  void eval(const std::vector<double> &x, double &f,
            std::vector<double> &g) const override; 

  // Return the dimension of the function
  size_t dim() const override { return (1 + nsample_) * (k_ - 1); } 
  
private:
  size_t k_;         // Number of categories.
  double lambda_;    // Weight of the penalty term.
  double sigma_;     // Parameter for the RBF kernel
  unsigned nthreads_;
  size_t nsample_;
  size_t ncont_;
  size_t nord_;
  size_t nnom_;

  // Scaled response values.
  std::vector<double> resp_; 

  // Full kernel matrix, stored in row major.
  std::vector<double> kernel_; 

  // Vertices of the simplex. When interpreted as a 2D matrix with a row-major
  // layout in memory, w(i, j) stores component j of vertex i of the
  // simplex. This memory layout is used when evaluting the function value. 
  std::vector<double> w_;

  // Transpose of w. This memory layout is used when evaluating the gradient.
  std::vector<double> wt_;

  // Solution to the nonlinear optimization problem
  std::vector<double> beta_;

  // Loss function
  double loss(double x, bool sign); 

  // Derivative of the loss function
  double dloss(double x, bool sign); 
}; 


#endif 
