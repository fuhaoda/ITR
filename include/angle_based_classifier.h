#ifndef __ANGLE_BASED_CLASSIFIER_H__
#define __ANGLE_BASED_CLASSIFIER_H__

#include <string>
#include <set> 
#include "fdf.h"
#include "data.h"
#include "types.h"

class AngleBasedClassifier final : public FDF {
public:
  // Constructor
  AngleBasedClassifier(double c, double lambda,
                       std::string kernel, unsigned nthreads);
  
  // Preprocess raw data set i.
  void preprocess(size_t i);

  // Solve the nonlinear optimization problem.
  void run(); 

  // Return the solution to the nonlinear optimization problem
  rVector beta() const; 
  
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
  double c_;          // Loss function parameter. 
  double thres_;      // Loss function transition point. 
  double lambda_;     // Weight of the penalty term.
  unsigned nthreads_; // Number of threads to use.
  double sigma_;      // Parameter for the RBF kernel.
  double shift_;      // Parameter for the polynomial (linear) kernel.
  double deg_;        // Parameter for the polynomial (linear) kernel. 
  size_t k_;          // Number of categories.
  size_t nsample_;    // Number of samples.
  size_t ncont_;      // Number of continuous variables.
  size_t nord_;       // Number of ordinal variables.
  size_t nnom_;       // Number of nominal variables.
  size_t nvar_;       // Total number of variables
  size_t ncomp_;      // Number of variables that are comparable. 
  
  // RBF kernel function 
  double rbf(const double *d, size_t i, size_t j) const; 

  // Polynomial kernel function 
  double poly(const double *d, size_t i, size_t j) const ; 

  // Pointer to the kernel function being used.
  double (AngleBasedClassifier::*func_)(const double *, size_t, size_t) const; 

  // Parse raw action values.
  void parse_actions(const std::vector<int> &act);

  // Compute the scaled responses.
  void scale_response(const std::vector<double> &resp,
                      const std::vector<double> &prob); 
  
  // Unique values of the raw actions
  std::set<int> uniq_act_; 

  // Mapped action values. The raw values are replaced by their ranks in the
  // ordered set `uniq_act_` that collects the unique values. 
  std::vector<size_t> act_; 
  
  // Scaled response values.
  std::vector<double> resp_;

  // Indices of the samples whose responses are positive. 
  std::vector<size_t> pidx_; 

  // Indices of the samples whose responses are negative.
  std::vector<size_t> nidx_; 

  // Set the vertices of the simplex
  void set_simplex_vertices(); 

  // Vertices of the simplex. When interpreted as a 2D matrix with a row-major
  // layout in memory, w(i, j) stores component j of vertex i of the
  // simplex. This memory layout is used when evaluting the function value. 
  std::vector<double> w_;

  // Transpose of w. This memory layout is used when evaluating the gradient.
  std::vector<double> wt_;

  // Compute kernel function. 
  void compute_kernel_matrix(const Data *data);

  // Worker function to compute the entries of the kernel matrix
  void kernel_worker(const double *d, size_t tid); 
  
  // Full kernel matrix, stored in row major.
  std::vector<double> kmat_; 

  // Solution to the nonlinear optimization problem
  std::vector<double> beta_;
  
  // Loss function when the reponse is positive. 
  double loss_p(double x) const;

  // Loss function when the resposne is negative.
  double loss_m(double x) const; 
  
  // Derivative of the loss function when the response is positive. 
  double dloss_p(double x) const;

  // Derivative of the loss function when the response is negative.
  double dloss_m(double x) const; 
}; 


#endif 
