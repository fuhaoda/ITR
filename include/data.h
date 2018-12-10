#ifndef __DATA_H__
#define __DATA_H__

#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <cassert> 
#include <iostream>

// The code here assumes that the first column is ID, followed by continuous
// variables, ordinal categorical variables, nominal variables, actions,
// responses, and probability of treatment assignment. It further assumes
// variables start with Cont, Ord, Nom, Y for identification.

// The current implementation does not allow missing data. It assumes that the
// data has been preprocessed before calling the methods.

// The number of unique values of any nominal categorical variables should be
// less than or equal to 31. 

class Data {
public:
  // Constructor
  explicit Data(const std::string &input);

  // Return the number of samples
  size_t nsample() const { return nsample_; }

  // Return the number of continuous variables
  size_t ncont() const { return ncont_; }

  // Return the number of ordinal variables
  size_t nord() const { return nord_; }

  // Return the number of nominal variables
  size_t nnom() const { return nnom_; }

  // Return the number of variables
  size_t nvar() const { return nvar_; } 
  
  // Return the raw value of continuous variable `i`
  const std::vector<double> &cont(size_t i) const {
    assert(i < ncont_);
    return cont_[i];
  }

  // Return the raw value of ordinal variable `i`
  const std::vector<int> &ord(size_t i) const {
    assert(i < nord_);
    return ord_[i];
  }

  // Return the raw value of nominal variable `i`
  const std::vector<int> &nom(size_t i) const {
    assert(i < nnom_);
    return nom_[i];
  }

  // Return the raw value of the action
  const std::vector<int> &act() const { return act_; }

  // Return the raw value of the response
  const std::vector<double> &resp() const { return resp_; }

  // Return the raw value of the probability
  const std::vector<double> &prob() const { return prob_; } 
  
private:
  size_t nsample_ = 0; // Sample size
  size_t ncont_ = 0;   // Number of continuous variables
  size_t nord_ = 0;    // Number of ordinal variables
  size_t nnom_ = 0;    // Number of nominal variables
  size_t nvar_ = 0;    // Number of variables

  // Array of subject ID
  std::vector<int> id_;

  // Continuous variables
  std::vector<std::vector<double>> cont_;

  // Ordinal variables
  std::vector<std::vector<int>> ord_;

  // Nominal variables
  std::vector<std::vector<int>> nom_; 
  
  // Action vector
  std::vector<int> act_; 

  // Reponse vector
  std::vector<double> resp_;

  // Probability vector
  std::vector<double> prob_;

  // Load the input csv file.
  void load_csv(const std::string &input);

  // Parse the header of the input csv file. It counts the number of continuous,
  // ordinal, and nominal variables.
  void parse_csv_header(std::ifstream &infile);

  // Read the raw data of the input file.
  void load_raw_data(std::ifstream &infile);   
}; 

class Foo {
  Foo(Data *d) {
    std::cout << d->ncont() << "\n";
  }
}; 
#endif 
