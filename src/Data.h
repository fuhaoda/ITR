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

  // Return the number of responses
  size_t nResp() const { return nResp_; }

  // Return the sum of (Resp | Act = 0)
  double T0() const { return T0_; }
  
  // Return the ith component of the jth response vector 
  double resp(size_t i, size_t j) const { return resp_[i * nResp_ + j]; }

  // Return the ith componet of the jth action vector 
  int act(size_t i) const { return act_[i]; } 

  // Return the ith component of the jth covariate vector
  int cvar(size_t i, size_t j) const { return cvar_[j * nSample_ + i]; }
  
  // Return the number of cuts for variable i
  size_t nCut(size_t vIdx) const { return cMask_[vIdx].value.size(); }

  // Return the masks associated with cut j of variable i
  const std::vector<bool> &cutMask(size_t vIdx, size_t cIdx) const {
    return cMask_[vIdx].mask[cIdx];
  }

  // Print information about cut cIdx for variable vIdx with mask m.
  // If vIdx is a continuous or ordinal variable,
  // then m = 0 means < and m = 1 means >=
  // If vIdx is a nominal variable,
  // then m = 0 means \in and m = 1 means not \in
  void cutInfo(size_t vIdx, size_t cIdx, bool m) const;

private:
  struct Meta {
    // Values of the cut for this variable
    std::vector<int> value;

    // Masks of the components that belong to each cut
    std::vector<std::vector<bool>> mask;
  };
  
  size_t nSample_ = 0; // Sample size
  size_t nCont_ = 0;   // Number of continuous variables
  size_t nOrd_ = 0;    // Number of ordinal variables
  size_t nNom_ = 0;    // Number of nominal variables
  size_t nVar_ = 0;    // Number of variables 
  size_t nResp_ = 0;   // Number of differnet responses 
  double T0_ = 0.0;    // Sum (Resp | Act = 0) 

  // Unique values of each ordinal variable  
  std::vector<std::set<int>> uniqOrd_;

  // Unique values of each nominal variable  
  std::vector<std::set<int>> uniqNom_; 
   
  // Array of subject ID
  std::vector<int> id_; 
  
  // Response matrix Y[nSample_][nResp_]
  std::vector<double> resp_; 
  
  // Action matrix A[nSample_][nAct_];
  std::vector<int> act_; 

  // Covariate matrix X[nCont_ + nOrd_ + nNom_][nSample_]. The different storage
  // layout is to faciliate the access pattern of the comprehensive search. 
  std::vector<int> cvar_;

  // Cut and mask information for each variable
  std::vector<Meta> cMask_;   

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

  // For variable i, this function sets up the masks for all the associated cut
  void setCutMasks(size_t i); 
};


} // namespace ITR

#endif 

