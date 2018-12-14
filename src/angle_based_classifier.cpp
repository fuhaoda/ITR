#include <thread>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <cmath>
#include "angle_based_classifier.h"

AngleBasedClassifier::AngleBasedClassifier(double c, double lambda, 
                                           std::string kernel,
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
    func_ = &AngleBasedClassifier::rbf;
    // Cast the sigma value
    sigma_ =  std::stod(params[1]);
    sigma_ = -1.0 / 2.0 / sigma_ / sigma_; 
  } else if (params[0] == "POLY") {
    func_ = &AngleBasedClassifier::poly; 
    shift_ = std::stod(params[1]);
    deg_ = std::stod(params[2]);    
  } else {
    throw "Unsupported kernel type!";
  }      
}

void AngleBasedClassifier::preprocess(size_t i) {
  Data *data = rdata[i].get();

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

  // Resize beta_ and initialize it with 0. 
  beta_.resize((1 + nsample_) * (k_ - 1));
  std::fill(beta_.begin(), beta_.end(), 0.0); 
}

void AngleBasedClassifier::parse_actions(const std::vector<int> &act) {
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

void
AngleBasedClassifier::scale_response(const std::vector<double> &resp,
                                     const std::vector<double> &prob) {
  resp_.resize(nsample_);
  for (size_t i = 0; i < nsample_; ++i) {
    resp_[i] = fabs(resp[i]) / prob[i];
    if (resp[i] >= 0) {
      pidx_.push_back(i);
    } else {
      nidx_.push_back(i);
    }
  }
}

void AngleBasedClassifier::set_simplex_vertices() {
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

void AngleBasedClassifier::compute_kernel_matrix(const Data *data) {
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
  
  for (size_t i = 0; i < nsample_; ++i) {
    for (size_t j = 0; j < nsample_; ++j) {
      kmat_[i * nsample_ + j] = (this->*func_)(d, i, j);
    }
  }
}

double AngleBasedClassifier::rbf(const double *d, size_t i, size_t j) const {
  const double *di = d + i * nvar_;
  const double *dj = d + j * nvar_; 
  double r = 0.0;
  
  for (size_t k = 0; k < ncomp_; ++k)
    r += pow(di[k] - dj[k], 2);
  
  for (size_t k = ncomp_; k < nvar_; ++k)
    r += (di[k] == dj[k]);
  
  return exp(r * sigma_); 
} 

double AngleBasedClassifier::poly(const double *d, size_t i, size_t j) const {
  const double *di = d + i * nvar_;
  const double *dj = d + j * nvar_;
  double r = std::inner_product(di, di + ncomp_, dj, 0.0) +
    std::inner_product(di + ncomp_, di + nvar_, dj + ncomp_, 0.0, 
                       std::plus<>(), std::equal_to<>());
  return pow(r + shift_, deg_); 
}

void AngleBasedClassifier::run() {

}

double AngleBasedClassifier::loss_p(double x) const {
  static double c1 = (1 + c_) * (1 + c_);
  static double c2 = 1 - c_ * c_;
  double retval = 1 - x;
  if (x >= thres_) {
    retval = 1.0 / (c1 * x + c2);
  }
  return retval; 
}

double AngleBasedClassifier::loss_m(double x) const {
  static double c1 = (1 + c_) * (1 + c_);
  static double c2 = 1 - c_ * c_;
  double retval = 1 + x;
  if (-x >= thres_) {
    retval = 1.0 / (c2 - c1 * x);
  }
  return retval;
}

double AngleBasedClassifier::dloss_p(double x) const {
  static double c1 = 1 + c_;
  static double c2 = 1 - c_;
  double retval = -1;
  if (x >= thres_) {
    retval = pow(c1 * x + c2, -2);
  }
  return retval;
}

double AngleBasedClassifier::dloss_m(double x) const {
  static double c1 = 1 + c_;
  static double c2 = 1 - c_; 
  double retval = 1;
  if (-x >= thres_) {
    retval = pow(c2 - c1 * x, -2);
  }
  return retval; 
}

void AngleBasedClassifier::eval(const std::vector<double> &x,
                                double &f) const {

}

void AngleBasedClassifier::eval(const std::vector<double> &x,
                                std::vector<double> &g) const {

}


void AngleBasedClassifier::eval(const std::vector<double> &x, double &f,
                                std::vector<double> &g) const {
}

rVector AngleBasedClassifier::beta() const {
  rVector retval(beta_.begin(), beta_.end());
  return retval; 
}
                                
