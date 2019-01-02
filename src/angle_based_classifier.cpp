#include <thread>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <cmath>
#include <future>
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
}

void ABCFunc::eval(const std::vector<double> &x, double &f) const {
  std::vector<double> loss(nsample_);
  compute_loss(x, loss.data(), nullptr);

  f = 0.0;
  for (size_t i = 0; i < nsample_; ++i)
    f += fabs(resp_[i]) * loss[i]; 
}

void ABCFunc::eval(const std::vector<double> &x, std::vector<double> &g) const {
  std::vector<double> dloss(nsample_);
  double *pd = dloss.data();
  double *pg = g.data(); 

  compute_loss(x, nullptr, pd);
  std::vector<std::thread> threads(nthreads_);
  for (size_t i = 0; i < nthreads_; ++i)
    threads[i] = std::thread(&ABCFunc::grad_worker, this, i, pd, pg);

  for (auto &th : threads)
    th.join(); 
}

void ABCFunc::eval(const std::vector<double> &x, double &f,
                   std::vector<double> &g) const {
  std::vector<double> loss(nsample_), dloss(nsample_);
  compute_loss(x, loss.data(), dloss.data());

  f = 0.0;
  for (size_t i = 0; i < nsample_; ++i)
    f += fabs(resp_[i]) * loss[i];

  double *pd = dloss.data();
  double *pg = g.data();
  std::vector<std::thread> threads(nthreads_);
  for (size_t i = 0; i < nthreads_; ++i)
    threads[i] = std::thread(&ABCFunc::grad_worker, this, i, pd, pg);

  for (auto &th : threads)
    th.join(); 
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

  std::fill(w_.begin(), w_.begin() + k_, c1);
  std::fill(w_.begin() + k_, w_.end(), c2);
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

void ABCFunc::compute_loss(const std::vector<double> &x,
                           double *loss, double *dloss) const {
  std::vector<std::thread> threads(nthreads_);
  const double *px = x.data(); 
  
  for (size_t i = 0; i < nthreads_; ++i)
    threads[i] = std::thread(&ABCFunc::loss_worker, this, i, px, loss, dloss); 

  for (auto &th : threads)
    th.join(); 
}

void ABCFunc::loss_worker(size_t tid, const double *x,
                          double *loss, double *dloss) const {
  size_t first = 0, last = 0;
  size_t per_worker = nsample_ / nthreads_;
  size_t remainder = nsample_ % nthreads_;

  if (tid < remainder) {
    first = (per_worker + 1) * tid;
    last = first + per_worker + 1;
  } else {
    first = per_worker * tid + remainder;
    last = first + per_worker;
  }

  std::vector<double> u(last - first, 0.0); 
  
  for (size_t i = first; i < last; ++i) {
    // Get the vertex of the simplex corresponding to the category of sample i.
    const double *w = &w_[act_[i] * (k_ - 1)];

    // Get row i of the kernel matrix.
    const double *row = &kmat_[i * nsample_];

    // Compute the projection of sample i.
    std::vector<double> proj(k_ - 1, 0.0);
    for (size_t j = 0; j < k_ - 1; ++j) {
      size_t j1 = j * (nsample_ + 1);
      size_t j2 = j1 + (nsample_ + 1);
      proj[j] = std::inner_product(&x[j1 + 1], &x[j2], row, x[j1]);
    }

    // Compute the inner product between the projection and simplex vertex.
    u[i - first] = std::inner_product(w, w + k_ - 1, proj.begin(), 0.0);
  }

  // Compute the loss function if loss is not nullptr.
  if (loss) {
    for (size_t i = first; i < last; ++i) {
      double v = u[i - first];
      loss[i] = (resp_[i] > 0 ? loss_p(v) : loss_m(v));
    }
  }
  
  // Compute the derivative of the loss function if dloss is not nullptr.
  if (dloss) {
    for (size_t i = first; i < last; ++i) {
      double v = u[i - first]; 
      dloss[i] = (resp_[i] > 0 ? dloss_p(v) : dloss_m(v));
    }
  }
}

void ABCFunc::grad_worker(size_t tid, const double *du, double *g) const {
  size_t first = 0, last = 0;
  size_t total = (nsample_ + 1) * (k_ - 1);
  size_t per_worker = total / nthreads_;
  size_t remainder = nsample_ % nthreads_;

  if (tid < remainder) {
    first = (per_worker + 1) * tid;
    last = first + per_worker + 1;
  } else {
    first = per_worker * tid + remainder;
    last = first + per_worker;
  }

  for (size_t i = first; i < last; ++i) {
    size_t q = i / (nsample_ + 1);
    size_t p = i % (nsample_ + 1);
    const double *wt = &w_[q * k_]; 
   
    if (p == 0) {
      // Partial derivative for x_{0q}
      g[i] = std::inner_product(wt, wt + k_, du, 0.0); 
    } else {
      // Partial derivative for x_{pq}
      g[i] = 0.0; 
      
      // Get row p of the kernel matrix.
      const double *row = &kmat_[p * nsample_];
      
      for (size_t j = 0; j < nsample_; ++j) 
        g[i] += du[j] * row[j] * wt[j]; 
    }
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
    retval = pow(c1 * x + c2, -2);
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





