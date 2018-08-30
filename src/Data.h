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
  unsigned nSample() const { return nSample_; } 

  // Return the number of continuous covariates
  unsigned nCont() const { return nCont_; }

  // Return the number of ordinal covariates
  unsigned nOrd() const { return nOrd_; }

  // Return the number of nominal covariates
  unsigned nNom() const { return nNom_; }

  // Return the number of actions
  unsigned nAct() const { return nAct_; }

  // Return the number of responses
  unsigned nResp() const { return nResp_; }

  // Return the handle to the response matrix with read permisson
  const double *resp() const { return resp_.data(); }

  // Return the handle to the action matrix with read permisson
  const int *act() const { return act_.data(); }

  // Return the handle to the covariate matrix with read permission
  const int *cvar() const { return cvar_.data(); } 

  // Return the number of cuts for variable i
  unsigned nCut(int i) const {
    if (i < nCont_) {
      return 10; 
    } else if (i < nCont_ + nOrd_) {
      return uniqOrd_[i].size();
    } else {
      // The number of cuts for a nominal variable is the number of subsets that
      // no more than half of the unique values. 

      // The subsets used to be stored explicitly. The current implementation
      // skips this storage.

      // The function here simply returns the number of unique values, denoted
      // by p.

      // The search needs to loop through [0, 2^p). Each iterator is interpreted
      // as a bitmask, where bit i corresponds to the ith element in the unique
      // set. For each integer, if the number of 1 bits is no more than p / 2,
      // then it is a valid cut, where the subset consists of the elements
      // corresponding to the 1 bits in the integer. 

      // If iterator 'iter' is a valid subset (cut), one can decide if component
      // j of the nominal variable belongs to the subset by performing bitwise
      // AND as component j saves the rank of the original value in the unique
      // set. 
      return uniqNom_[i].size();
    }
  }
        
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

  unsigned nSample_ = 0; // Sample size
  unsigned nCont_ = 0;   // # of continuous variables
  unsigned nOrd_ = 0;    // # of ordinal variables
  unsigned nNom_ = 0;    // # of nominal variables
  unsigned nAct_ = 0;    // # of different actions
  unsigned nResp_ = 0;   // # of differnet responses 

  std::vector<std::set<int>> uniqOrd_; // unique values of each ordinal variable
  std::vector<std::set<int>> uniqNom_; // unique values of each nominal variable 
  
  // Array of subject ID
  std::vector<unsigned> id_; 
  
  // Response matrix Y[nSample_][nResp_]
  std::vector<double> resp_; 
  
  // Action matrix A[nSample_][nAct_];
  std::vector<int> act_; 

  // Covariate matrix X[nSample_][nCont_ + nOrd_ + nNom_]
  std::vector<int> cvar_;
};


} // namespace ITR

#endif 


