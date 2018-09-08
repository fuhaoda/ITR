#ifndef __DATA_H__
#define __DATA_H__

#include <string>
#include <fstream>
#include <vector>
#include <set> 

namespace ITR {
  
class Data {
public:
  // Constructor
  Data(std::string input);

  // Return the number of sample size 
  size_t nSample() const { return nSample_; } 

  // Return the number of continuous covariates
  size_t nCont() const { return nCont_; }

  // Return the number of ordinal covariates
  size_t nOrd() const { return nOrd_; }

  // Return the number of nominal covariates
  size_t nNom() const { return nNom_; }

  // Return the total number of covariates
  size_t nVar() const { return nVar_; }

  // Return the number of actions
  size_t nAct() const { return nAct_; }

  // Return the number of responses
  size_t nResp() const { return nResp_; }

  // Return the sum of (Resp | Act = 0)
  double T0() const { return T0_; }
  
  // Return the ith component of the jth response vector 
  double resp(size_t i, size_t j) const { return resp_[i * nResp_ + j]; }

  // Return the ith componet of the jth action vector 
  int act(size_t i, size_t j) const { return act_[i * nAct_ + j]; } 

  // Return the ith component of the jth covariate vector
  int cvar(size_t i, size_t j) const { return cvar_[j * nSample_ + i]; }

  // Return the number of cuts for variable i
  size_t nCut(size_t i) const;

  // Return if component i of variable j belongs to cut k
  bool inCut(size_t i, size_t j, size_t k) const;

  // Print information about cut j for varible i with mask m
  // For continuous or ordinal variable, m = 0 is < and m = 1 is >=
  // For nominal variable, m = 0 is \in and m = 1 is not \in 
  void cutInfo(size_t i, size_t j, bool m) const; 
  
private:
  // This function loads the input file. It assumes that fields of the same type
  // are in consecutive columns. It also assumes that the fields are given in
  // the order of subjectID, continuous variable, ordinal variable, nominal
  // variable, action, and responses. 
  void loadCSV(std::string input); 

  // This function parses the header of the input file. It counts the number of
  // continuous, ordinal, and nominal variables, the number of differnet types
  // of actions and responses. 
  void parseCSVHeader(std::ifstream &infile); 

  // This function reads the raw data of the input file. Covariates are read
  // into temporary buffer while actions and responses are read into the
  // internal buffer directly. For ordinal and nominal variables, this function
  // also collects the unique values. 
  void loadRawData(std::ifstream &infile,
                   std::vector<std::vector<double>> &cont,
                   std::vector<std::vector<int>> &ord,
                   std::vector<std::vector<int>> &nom); 

  // This function parses the raw data of the covariates into the format needed
  // by the SearchEngine. Values of the continuous variables are converted into
  // deciles. Values of the ordinal and nominal variables are converted into the
  // order they appear in the unique sets. Variables corresponding to the same
  // sample will be stored contiguously at the end of this function.
  void parseRawData(std::vector<std::vector<double>> &cont,
                    std::vector<std::vector<int>> &ord,
                    std::vector<std::vector<int>> &nom); 

  size_t nSample_ = 0; // Sample size
  size_t nCont_ = 0;   // # of continuous variables
  size_t nOrd_ = 0;    // # of ordinal variables
  size_t nNom_ = 0;    // # of nominal variables
  size_t nVar_ = 0;    // # of variables 
  size_t nAct_ = 0;    // # of different actions
  size_t nResp_ = 0;   // # of differnet responses 
  double T0_ = 0.0; // Sum (Resp | Act = 0) 

  
  std::vector<std::set<int>> uniqOrd_; // unique values of each ordinal variable
  std::vector<std::set<int>> uniqNom_; // unique values of each nominal variable
  
  // Array of subject ID
  std::vector<int> id_; 
  
  // Response matrix Y[nSample_][nResp_]
  std::vector<double> resp_; 
  
  // Action matrix A[nSample_][nAct_];
  std::vector<int> act_; 

  // Covariate matrix X[nCont_ + nOrd_ + nNom_][nSample_]. The different storage
  // layout is to faciliate the access pattern of the comprehensive search. 
  std::vector<int> cvar_;
};


} // namespace ITR

#endif 


