#include <thread>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <cmath>
#include <future>
#include <functional>
#include "angle_based_classifier.h"
#include "vlbfgs.h"

AngleBasedClassifier::AngleBasedClassifier(double c, double lambda,
					   const std::string &kernel,
					   unsigned nthreads) {
  // Create the objective function 
  func_ = std::make_unique<ABCFunc>(c, lambda, kernel, nthreads); 
}

void AngleBasedClassifier::preprocess(size_t i) {
  // Bind the data to the objective function.
  func_->bind(rdata[i].get());

  // Resize beta and initialize it to zero.
  beta_.resize(func_->dim());
  std::fill(beta_.begin(), beta_.end(), 0.0);

#if 1
  beta_[0] = 1;
  beta_[1] = 2;
  beta_[2] = 3;
  beta_[3] = 4;
  beta_[4] = 5;
  beta_[5] = 6;
  beta_[6] = 7;
  beta_[7] = 8;
  beta_[8] = 9;
  beta_[9] = 10;
#endif
}

void AngleBasedClassifier::run(size_t maxIter, size_t m, double eps) {
  if (vlbfgs_ == nullptr) {
    // Create the solver object if it does not exist. 
    vlbfgs_ = std::make_unique<VLBFGS>(func_.get(), maxIter, beta_, m,
				       eps, 1e-16);
  } else {
    // Update the solver object.
    vlbfgs_.reset(new VLBFGS{func_.get(), maxIter, beta_, m, eps, 1e-16});
  }

  // Solve the nonlinear optimization problem.
  vlbfgs_->solve();

  // Copy the results.
  vlbfgs_->x(beta_); 
}

rVector AngleBasedClassifier::beta() const {
  rVector retval(beta_.begin(), beta_.end());
  return retval;
}

ABCFunc::ABCFunc(double c, double lambda, std::string kernel,
                 unsigned nthreads) {
  c_ = c;
  thres_ = c / (1 + c);
  lambda_ = lambda; 
  nthreads_ = std::min(nthreads, std::thread::hardware_concurrency());

  // Transform input kernel string to all CAPs.
  std::transform(kernel.begin(), kernel.end(), kernel.begin(), ::toupper);
  
  std::istringstream ss(kernel);
  std::vector<std::string> params{std::istream_iterator<std::string>(ss), {}};
  if (params[0] == "RBF") {
    func_ = &ABCFunc::rbf;
    // Cast the sigma value
    sigma_ =  std::stod(params[1]);
    sigma_ = -1.0 / 2.0 / sigma_ / sigma_; 
  } else if (params[0] == "POLY") {
    func_ = &ABCFunc::poly; 
    shift_ = std::stod(params[1]);
    deg_ = std::stod(params[2]);    
  } else {
    throw "Unsupported kernel type!";
  }      
}

void ABCFunc::bind(const Data *data) {
  nsample_ = data->nsample();
  ncont_ = data->ncont();
  nord_ = data->nord();
  nnom_ = data->nnom();
  nvar_ = ncont_ + nord_ + nnom_;
  ncomp_ = ncont_ + nord_; 

  // Count number of categories and map raw values into their ranks.
  parse_actions(data->act());

  // Scale responses.
  scale_response(data->resp(), data->prob());

  // Set up the vertices of the simplex
  set_simplex_vertices(); 

  // Compute kernel matrix
  compute_kernel_matrix(data);

  // Set problem dimension
  dim_ = (1 + nsample_) * (k_ - 1); 
}

void ABCFunc::eval(const std::vector<double> &x, double &f) const {
  std::vector<double> loss(nsample_);
  std::vector<double> J(nthreads_);

  compute(x, loss.data(), J.data(), nullptr);

  f = 0.0;
  // Accumulate the loss terms.
  for (size_t i = 0; i < nsample_; ++i)
    f += fabs(resp_[i]) * loss[i];

  f /= nsample_;

  // Add the penalty term.
  for (size_t i = 0; i < nthreads_; ++i)
    f += J[i]; 
}

void ABCFunc::eval(const std::vector<double> &x, std::vector<double> &g) const {
  std::vector<double> dJ(dim_ * nthreads_);

  compute(x, nullptr, nullptr, dJ.data());

  // Copy the results from thread 0
  std::copy(dJ.begin(), dJ.begin() + dim_, g.begin());

  // Accumulate the results from the remaining threads.
  for (size_t j = 1; j < nthreads_; ++j) {
    size_t j1 = j * dim_;
    size_t j2 = j1 + dim_;
    std::transform(&dJ[j1], &dJ[j2], &g[0], &g[0], std::plus<double>());
  }
}

void ABCFunc::eval(const std::vector<double> &x, double &f,
                   std::vector<double> &g) const {
  std::vector<double> loss(nsample_);
  std::vector<double> J(nthreads_);
  std::vector<double> dJ(dim_ * nthreads_);

  compute(x, loss.data(), J.data(), dJ.data());

  f = 0.0; 
  // Accumulate the loss terms.
  for (size_t i = 0; i < nsample_; ++i)
    f += fabs(resp_[i]) * loss[i];

  f /= nsample_;

  // Add the penalty term.
  for (size_t i = 0; i < nthreads_; ++i)
    f += J[i]; 

  // Copy the results from thread 0
  std::copy(dJ.begin(), dJ.begin() + dim_, g.begin());

  // Accumulate the results from the remaining threads.
  for (size_t j = 1; j < nthreads_; ++j) {
    size_t j1 = j * dim_;
    size_t j2 = j1 + dim_;
    std::transform(&dJ[j1], &dJ[j2], &g[0], &g[0], std::plus<double>());
  }
}

double ABCFunc::rbf(const double *d, size_t i, size_t j) const {
  const double *di = d + i * nvar_;
  const double *dj = d + j * nvar_; 
  double r = 0.0;
  
  for (size_t k = 0; k < ncomp_; ++k)
    r += pow(di[k] - dj[k], 2);
  
  for (size_t k = ncomp_; k < nvar_; ++k)
    r += (di[k] == dj[k]);
  
  return exp(r * sigma_); 
} 

double ABCFunc::poly(const double *d, size_t i, size_t j) const {
  const double *di = d + i * nvar_;
  const double *dj = d + j * nvar_;
  double r = std::inner_product(di, di + ncomp_, dj, 0.0) +
    std::inner_product(di + ncomp_, di + nvar_, dj + ncomp_, 0.0, 
                       std::plus<>(), std::equal_to<>());
  return pow(r + shift_, deg_); 
}

void ABCFunc::parse_actions(const std::vector<int> &act) {
  // Collect the unique values of the raw values.
  for (auto v : act)
    uniq_act_.insert(v);

  // Set the number of categories
  k_ = uniq_act_.size(); 

  // Create a map from the value to its rank within the unique set.
  std::map<int, int> reverse_map;
  int rank = 0;
  for (const auto &v: uniq_act_)
    reverse_map.insert(std::make_pair(v, rank++));

  // Map the raw values into their ranks within the unique set.
  act_.resize(nsample_); 
  for (size_t i = 0; i < nsample_; ++i)
    act_[i] = reverse_map[act[i]]; 
}

void ABCFunc::scale_response(const std::vector<double> &resp,
                             const std::vector<double> &prob) {
  resp_.resize(nsample_);
  for (size_t i = 0; i < nsample_; ++i)
    resp_[i] = resp[i] / prob[i]; 
}

void ABCFunc::set_simplex_vertices() {
  size_t sz = k_ * (k_ - 1);
  w_.resize(sz);
  wt_.resize(sz); 
  
  double c1 = 1.0 / sqrt(k_ - 1);
  double c2 = - (1 + sqrt(k_)) * pow(c1, 3);
  double c3 = sqrt(k_) * c1;

  std::fill(w_.begin(), w_.begin() + k_ - 1, c1);
  std::fill(w_.begin() + k_ - 1, w_.end(), c2);
  for (size_t j = 1; j < k_; ++j) {
    // Update jth row, (j-1)th column
    w_[j * k_ - 1] += c3;
  }

  std::fill(wt_.begin(), wt_.end(), c2);
  for (size_t j = 0; j < k_ - 1; ++j) {
    wt_[j * k_] = c1;
    wt_[j * k_ + j + 1] += c3;
  }
}

void ABCFunc::compute_kernel_matrix(const Data *data) {
  // The raw data are read column wise. Here, we need to store it row wise to
  // perform the inner product more efficiently.
  std::vector<double> temp(nsample_ * nvar_); 

  for (size_t i = 0; i < ncont_; ++i) {
    const std::vector<double> &cont = data->cont(i);
    size_t iter = i;

    // Note: Perform normalization if needed. 

    for (size_t j = 0; j < nsample_; ++j) {
      temp[iter] = cont[j];
      iter += nvar_;
    }
  }

  for (size_t i = 0; i < nord_; ++i) {
    const std::vector<int> &ord = data->ord(i);
    size_t iter = i + ncont_;
    for (size_t j = 0; j < nsample_; ++j) {
      temp[iter] = static_cast<double>(ord[j]);
      iter += nvar_;
    }
  }

  for (size_t i = 0; i < nnom_; ++i) {
    const std::vector<int> &nom = data->nom(i);
    size_t iter = i + ncont_ + nord_;
    for (size_t j = 0; j < nsample_; ++j) {
      temp[iter] = static_cast<double>(nom[j]);
      iter += nvar_;
    }
  }
  
  kmat_.resize(nsample_ * nsample_);
  auto d = temp.data();   

  // Compute the kernel matrix in parallel. 
  std::vector<std::thread> threads(nthreads_);
  for (size_t i = 0; i < nthreads_; ++i)
    threads[i] = std::thread(&ABCFunc::kernel_worker, this, d, i);

  for (auto &th: threads)
    th.join(); 
}

void ABCFunc::kernel_worker(const double *d, size_t tid) {
  size_t first = 0, last = 0;
  size_t total = nsample_ * (nsample_ + 1) / 2;
  size_t per_worker = total / nthreads_;
  size_t remainder = total % nthreads_;

  // Compute the range of work performed by this worker [first, last]
  if (tid < remainder) {
    first = (per_worker + 1) * tid;
    last = first + per_worker;
  } else {
    first = per_worker * tid + remainder;
    last = first + per_worker - 1;
  }

  // The kernel matrix is symmetric. We only need to compute the upper
  // triangular part and variable iter is used to track the rank of these
  // entries.
  size_t iter = 0; 
  for (size_t i = 0; i < nsample_; ++i) {
    size_t bound = iter + nsample_ - i - 1;

    // The upper triangular parts of row i of the kernel matrix correspond to
    // the range [iter, bound].
    if (bound < first) {
      // [iter, bound] is to the left of [first, last]. Skip to the next row.
      iter = bound + 1;
    } else if (last >= iter) {
      // [iter, bound] intersects [first, last].

      // Don't overshoot the current row. 
      bound = std::min(last, bound);
      size_t j = iter;
      while (iter <= bound) {
        if (iter >= first) {
          size_t k = i + iter - j; 
          double v = (this->*func_)(d, i, k);
          kmat_[i * nsample_ + k] = v;
          kmat_[k * nsample_ + i] = v;
        }
        iter++;
      }
      
      // All work is done is last is in the middle of the current row
      if (iter > last) 
        break; 
    }
  }
}

void ABCFunc::compute(const std::vector<double> &x,
                      double *loss, double *J, double *dJ) const {
  std::vector<std::thread> threads(nthreads_);
  const double *px = x.data();

  for (size_t i = 0; i < nthreads_; ++i)
    threads[i] = std::thread(&ABCFunc::worker, this, i, px, loss, J, dJ);

  for (auto &th : threads)
    th.join(); 
}

void ABCFunc::worker(size_t tid, const double *x, double *loss, double *J,
                     double *dJ) const {
  // There are up to two rounds of work distribution in this function.
  // The first round is to distribute the computation of u_i, the argument to
  // the loss function.
  // If derivative needs to be computed, the second round distributes the
  // computation of the derivative of the penalty term. 
  size_t first = 0, last = 0;
  size_t total = nsample_;
  size_t per_worker = total / nthreads_;
  size_t remainder = total % nthreads_;

  if (tid < remainder) {
    first = (per_worker + 1) * tid;
    last = first + per_worker + 1;
  } else {
    first = per_worker * tid + remainder;
    last = first + per_worker;
  }

  // Compute the arguments to the loss function.
  std::vector<double> u(last - first, 0.0);

  // Variable to accumulate penalty contribution.
  // Note: if the function simply needs the value of the derivative, the work on
  // this variable is unnecessary. However, without doing so, one needs to
  // increase the memory footprint to store the projection of each sample. When
  // computation is cheaper than memory access, this is a tradeoff the
  // implementation makes.
  double contrib = 0.0;

  for (size_t i = first; i < last; ++i) {
    // Get the vertex of the simplex corresponding to the category of sample i.
    const double *w = &w_[act_[i] * (k_ - 1)];

    // Get row i of the kernel matrix.
    const double *row = &kmat_[i * nsample_];

    // Compute the projection of sample i.
    std::vector<double> proj(k_ - 1, 0.0);
    for (size_t j = 0; j < k_ - 1; ++j) {
      size_t j1 = j * (nsample_ + 1);
      size_t j2 = j1 + nsample_ + 1;
      proj[j] = std::inner_product(&x[j1 + 1], &x[j2], row, 0.0);

      // Accumulate the contribution from the penalty term.
      contrib += proj[j] * x[j1 + 1 + i];

      proj[j] += x[j1];
    }

    // Compute the inner product between the projection and the simplex vertex.
    u[i - first] = std::inner_product(w, w + k_ - 1, proj.begin(), 0.0);
  }

  // If loss is not nullptr, compute the function value.
  if (loss && J) {
    J[tid] = contrib;

    for (size_t i = 0; i < last; ++i) {
      double v = u[i - first]; 
      loss[i] = (resp_[i] > 0 ? loss_p(v) : loss_m(v));
    }
  }

  if (dJ == nullptr)
    return;

  // Compute the derivative of the function.

  // First compute the derivative of the loss function. The results are
  // overwritten into u.
  for (size_t i = 0; i < last - first; ++i) {
    double v = u[i];
    u[i] = (resp_[i + first] > 0 ? dloss_p(v) : dloss_m(v));
  }

  // Compute the contribution to the derivatives from the assigned samples. 
  size_t iter = tid * dim_; 
  for (size_t q = 0; q < k_ - 1; ++q) {
    const double *wt = &wt_[q * k_];

    // Partial derivative for x_{0q}
    double temp = 0.0;
    for (size_t i = first; i < last; ++i)
      temp += fabs(resp_[i]) * u[i - first] * wt[act_[i]];

    dJ[iter++] = temp / nsample_;

    // Partial derivatives for x_{pq}
    for (size_t p = 0; p < nsample_; ++p) {
      const double *row = &kmat_[p * nsample_];

      temp = 0.0;
      for (size_t i = first; i < last; ++i)
        temp += fabs(resp_[i]) * u[i - first] * wt[act_[i]] * row[i];

      dJ[iter++] = temp / nsample_;
    }
  }

  // Compute the contribution to the derivatives from the penalty term.

  // Redistribute work.
  total = nsample_ * (k_ - 1);
  per_worker = total / nthreads_;
  remainder = total % nthreads_;

  // In the first round, the extra work is distributed to threads with smaller
  // tids. To balance the work, the extra work here is distributed to threads
  // with larger tids. 
  if (tid < nthreads_ - remainder) {
    first = per_worker * tid;
    last = first + per_worker;
  } else {
    first = (per_worker + 1) * tid - remainder + 1;
    last = first + per_worker;
  }

  iter = tid * dim_; 
  for (size_t i = first; i < last; ++i) {
    // Compute which partial derivative is being computed. 
    size_t q = i / nsample_;
    size_t p = i % nsample_;

    // Get row p of the kernel matrix
    const double *row = &kmat_[p * nsample_];

    // Get x_{1q} 
    size_t j = q * (nsample_ + 1) + 1;

    double temp = std::inner_product(row, row + nsample_, &x[j], 0.0);

    // Save the result. 
    dJ[iter + j + p] += 2.0 * temp * lambda_;
  }
}

double ABCFunc::loss_p(double x) const {
  static double c1 = (1 + c_) * (1 + c_);
  static double c2 = 1 - c_ * c_;
  double retval = 1 - x;
  if (x >= thres_) {
    retval = 1.0 / (c1 * x + c2);
  }
  return retval; 
}

double ABCFunc::loss_m(double x) const {
  static double c1 = (1 + c_) * (1 + c_);
  static double c2 = 1 - c_ * c_;
  double retval = 1 + x;
  if (-x >= thres_) {
    retval = 1.0 / (c2 - c1 * x);
  }
  return retval;
}

double ABCFunc::dloss_p(double x) const {
  static double c1 = 1 + c_;
  static double c2 = 1 - c_;
  double retval = -1;
  if (x >= thres_) {
    retval = -pow(c1 * x + c2, -2);
  }
  return retval;
}

double ABCFunc::dloss_m(double x) const {
  static double c1 = 1 + c_;
  static double c2 = 1 - c_; 
  double retval = 1;
  if (-x >= thres_) {
    retval = pow(c2 - c1 * x, -2);
  }
  return retval; 
}
