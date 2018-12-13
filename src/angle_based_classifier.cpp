#include <thread>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
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
    sigma_ = std::stod(params[1]);
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

  // Count number of categories and map raw values into their ranks.
  parse_actions(data->act());

  // Scale responses.
  scale_response(data->resp(), data->prob());

  // Set up the vertices of the simplex
  set_simplex_vertices(); 
  
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

}

double AngleBasedClassifier::rbf(size_t i, size_t j) const {
  return 0.0; 
} 

double AngleBasedClassifier::poly(size_t i, size_t j) const {
  return 0.0; 
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
    retval /= pow(c1 * x + c2, 2);
  }
  return retval;
}

double AngleBasedClassifier::dloss_m(double x) const {
  static double c1 = 1 + c_;
  static double c2 = 1 - c_; 
  double retval = 1;
  if (-x >= thres_) {
    retval /= pow(c2 - c1 * x, 2);
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
                                
