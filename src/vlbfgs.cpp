#include <cmath>
#include <algorithm>
#include <numeric>
#include <cassert> 
#include <functional>
#include "vlbfgs.h"
#if 1
#include <iostream>
#endif


VLBFGS::VLBFGS(FDF *func, size_t maxIter, const std::vector<double> &x0,
               size_t m, double eps, double xtol)
  : MultiMin{func, maxIter, x0}, m_{m}, eps_{eps}
{
  // Create line search object.
  mt_ = std::make_unique<MoreThuente>(func, xtol);
  
  // Allocate storage for VLBFGS
  size_t m2 = m_ * 2 + 1;
  alpha_.resize(m_);
  b_.resize(n_ * m2);
  B_.resize(m2 * m2);
  delta_.resize(m2); 

  // Initialize
  std::fill(alpha_.begin(), alpha_.end(), 0.0);
  std::fill(b_.begin(), b_.end(), 0.0);
  std::fill(B_.begin(), B_.end(), 0.0);
  std::fill(delta_.begin(), delta_.end(), 0.0); 
   
  // Set the initial search direction to the negative gradient direction. In
  // each iteration, the search direction is first saved in the storage slot for
  // variable s. It will be scaled when a step size is returned from the line
  // search module.
  func_->eval(x_, f_, g_); 

  // Save the gradient also in the slot for vector b_2m
  std::transform(&g_[0], &g_[n_], &b_[0], [](double x) { return -x; });
  std::copy(&g_[0], &g_[n_], &b_[2 * m_ * n_]); 

  // Initialize threads
  nthreads_ = std::thread::hardware_concurrency();
  threads_.resize(nthreads_); 
}

bool VLBFGS::iterate() {
  std::cout << x_[0] << "\n"; 

  
  // Variable iter_ is the subscript of the correction term to be generated in
  // the current iteration. The search direction term will be properly scaled
  // and the new gradient difference will be computed. 
  size_t m2 = m_ * 2; 

  // Get addresses for the latest s and y variables and the last b vector
  double *s = &b_[(iter_ % m_) * n_];
  double *y = s + m_ * n_;
  double *last_b = &b_[m2 * n_]; 
  
  // Set the initial guess of the step size for the line search. For the first
  // iteration, the initial guess of the step size for the line search is the
  // reciprocal of the initial gradient.
  double step_size = (iter_ ? 1.0 :
		      1.0 / sqrt(std::inner_product(s, s + n_, s, 0.0))); 

  // Line search. Variables x_, f_, and g_ will be updated to the new location
  assert(mt_->search(x_, f_, g_, s, step_size) == 0); 

  // Save the latest correction term. Update s variable by multiplying the
  // search direction with the step size returned from the line search. Compute
  // the gradient difference.
  std::transform(s, s + n_, s, [step_size](double x) { return x * step_size; });
  std::transform(&g_[0], &g_[n_], last_b, y, std::minus<double>()); 

  // Check for termination
  double gnorm = std::inner_product(&g_[0], &g_[n_], &g_[0], 0.0);
  double xnorm = std::inner_product(&x_[0], &x_[n_], &x_[0], 0.0); 
  gnorm = sqrt(gnorm);
  xnorm = std::max(1.0, sqrt(xnorm));
  if (gnorm / xnorm <= eps_)
    return false;

  // More iteration is needed. Compute the new search direction
  
  // Update last_b with the latest gradient direction
  std::copy(&g_[0], &g_[n_], last_b); 

  // Update the dot product matrix B in parallel
  for (size_t i = 0; i < nthreads_; ++i)
    threads_[i] = std::thread(&VLBFGS::updateMatrixB, this, i);

  // The inner product of the last b vector with itself has already been
  // computed during the termination detection part. Here, we simply square
  // gnorm.
  B_[4 * m_ * (m_ + 1)] = gnorm * gnorm; 
    
  for (auto &thread: threads_)
    thread.join();

  // Compute the linear combination coefficients
  computeDelta(); 

  // Assemble the new search direction in parallel
  for (size_t i = 0; i < nthreads_; ++i)
    threads_[i] = std::thread(&VLBFGS::assembleSearchDirection, this, i);

  for (auto &thread: threads_)
    thread.join(); 

  return true; 
}

void VLBFGS::updateMatrixB(size_t tid) {
  // Indices for the rows to be updated.
  size_t r1 = iter_ % m_;
  size_t r2 = r1 + m_;
  size_t r3 = m_ * 2;
  size_t rows[3] = {r1, r2, r3}; 

  // Handles of the vectors that are just updated.
  double *lhs[3] = {&b_[r1 * n_], &b_[r2 * n_], &b_[r3 * n_]}; 

  // Compute the inner products to be computed by the current thread.
  size_t bound = std::min(m_, iter_ + 1);
  size_t bound2 = bound * 2; 
  size_t total = bound * 6;
  size_t per_worker = total / nthreads_;
  size_t remainder = total % nthreads_;
  size_t first = 0, last = 0;

  if (tid < remainder) {
    first = (per_worker + 1) * tid;
    last = first + per_worker + 1;
  } else {
    first = per_worker * tid + remainder;
    last = first + per_worker;
  }

  size_t row_size = 2 * m_ + 1; 
  for (size_t i = first; i < last; ++i) {
    size_t j = i / bound2;
    size_t k = i % bound2;

    size_t row = rows[j];
    size_t col = (k < bound ? k : k + m_ - bound);

    double *v1 = lhs[j];
    double *v2 = &b_[col * n_];

    double t = std::inner_product(v1, v1 + n_, v2, 0.0); 

    // Update B(row, col)
    B_[row * row_size + col] = t;

    // Update B(col, row) when there is no write conflict
    if (row != m_ * 2 && (row == col + m_ || col == row + m_))
      continue; 

    B_[col * row_size + row] = t;
  }
}
				 
void VLBFGS::computeDelta() {
  // Variable iter_ is the subscript of the correction term just computed in the
  // current iteration. Including iter_, the subscripts of the latest m
  // correction terms are in the interval [first, iter_], where first is  
  size_t first = (iter_ >= m_ ? iter_ - m_ + 1: 0);

  // Variable bound tracks the boundary of the storage for the b vectors that
  // are currently occupied.
  size_t bound = std::min(m_ - 1, iter_);
  
  size_t m2 = m_ * 2;
  size_t row_size = m2 + 1; 
  
  // Reset expansion coefficients of the search direction for the next iteration.
  std::fill(&delta_[0], &delta_[m2], 0.0); 
  delta_[m2] = -1.0; 

  // The first for-loop recursion in algorithm 3.
  for (size_t i = iter_; i >= first; ) {
    // Compute which row of the B matrix is accessed in this iteration
    size_t rid = i % m_;

    // Get handle on the content of the above row
    double *row = &B_[rid * row_size];

    // Compute alpha
    double alpha = delta_[m2] * row[m2];
    for (size_t j = 0; j <= bound; ++j) 
      alpha += delta_[j] * row[j] + delta_[j + m_] * row[j + m_]; 
    alpha /= row[m_ + rid];

    // Save the result for the second for-loop
    alpha_[rid] = alpha;

    // Update delta
    delta_[rid + m_] -= alpha; 

    // Decrement i safely to avoid wrapping behavior of the unsigned values
    if (i == 0)
      break;
    i--;
  }

  // Scale delta as applying the initial diagnoal matrix
  {
    size_t rid = iter_ % m_ + m_;
    double *row = &B_[rid * row_size];
    double diag = row[rid - m_] / row[rid];
    std::transform(&delta_[0], &delta_[row_size], &delta_[0],
                   [&diag](double x) { return x * diag; }); 
  }

  // The second for-loop recursion in algorithm 3.
  for (size_t i = first; i <= iter_; ++i) {
    // Compute which row of the B matrix is accessed in this iteration. 
    size_t rid = (i % m_) + m_;

    // Get handle on the content of the above row
    double *row = &B_[rid * row_size];

    // Compute beta
    double beta = delta_[m2] * row[m2];
    for (size_t j = 0; j <= bound; j++) 
      beta += delta_[j] * row[j] + delta_[j + m_] * row[j + m_]; 
    beta /= row[rid - m_]; 
   
    // Update delta
    delta_[rid - m_] += (alpha_[rid - m_] - beta); 
  }
}

void VLBFGS::assembleSearchDirection(size_t tid) {
  // Compute the range to be assembled by this thread.
  size_t per_worker = n_ / nthreads_;
  size_t remainder = n_ % nthreads_;
  size_t first = 0, last = 0;

  if (tid < remainder) {
    first = (per_worker + 1) * tid;
    last = first + per_worker + 1;
  } else {
    first = per_worker * tid + remainder;
    last = first + per_worker;
  }

  // Get the address to write the search direction
  size_t sid = (iter_ + 1) % m_;
  double *next_s = &b_[sid * n_];

  // Handles on the s and y variable being processed
  size_t mn = m_ * n_; 
  double *s = next_s;
  double *y = s + mn; 
  double delta_s = delta_[sid];
  double delta_y = delta_[sid + m_]; 
  
  // Process the contribution from the s and y variables where the storage for
  // the s variable is being overwritten.
  std::transform(s + first, s + last, y + first, next_s + first,
		 [&](double x, double y) { return x * delta_s + y * delta_y; });

  // Process the remaining s and y variables
  size_t bound = std::min(m_ - 1, iter_); 
  for (size_t i = 0; i <= bound; ++i) {
    if (i == sid)
      continue;
    s = &b_[i * n_];
    delta_s = delta_[i]; 
    std::transform(s + first, s + last, next_s + first, next_s + first,
		   [&](double x, double p) { return delta_s * x + p; });

    y = s + mn;
    delta_y = delta_[i + m_]; 
    std::transform(y + first, y + last, next_s + first, next_s + first,
		   [&](double x, double p) { return delta_y * x + p; }); 
  }

  // Process the last b vector
  double *last_b = &b_[2 * mn];
  double delta_lb = delta_[2 * m_];
  std::transform(last_b + first, last_b + last, next_s + first, next_s + first,
		 [&](double g, double p) { return delta_lb * g + p; }); 
}

