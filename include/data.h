#ifndef __DATA_H__
#define __DATA_H__

#include <string>
#include <fstream>
#include <vector>
#include <cassert> 

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
  std::vector<std::int> act_; 

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
  



// class Data {
// public:
//   // Constructor
//   explicit Data(std::string const &input);
  
//   // Return the number of sample size
//   size_t nSample() const { return nSample_; }
  
//   // Return the number of continuous covariates
//   size_t nCont() const { return nCont_; }

//   // Return the number of ordinal covariates
//   size_t nOrd() const { return nOrd_; }
  
//   // Return the number of nominal covariates
//   size_t nNom() const { return nNom_; }
  
//   // Return the total number of covariates
//   size_t nVar() const { return nVar_; }
  
//   // Return the number of responses
//   size_t nResp() const { return nResp_; }
  
//   // Return the sum of (Resp | Act = 0)
//   double T0() const { return T0_; }
  
//   // Return the ith component of the jth response vector
//   // Response is one dimensional in the current implementation and will be
//   // extended to multi-dimensional cases.
//   // Multi-dimensional responses will be read by rows for efficiency.
//   double resp(size_t i, size_t j = 0) const { return resp_[i * nResp_ + j]; }

//   // Return the ith batch of the action vector
//   std::uint32_t act(size_t i) const { return act_[i]; }
  
//   // Return the number of cuts for variable i
//   size_t nCut(size_t vIdx) const { return cvar_[vIdx].value.size(); }
  
//   // Return the masks associated with cut j of variable i
//   const std::vector<std::uint32_t> &cutMask(size_t vIdx, size_t cIdx) const {
//     return cvar_[vIdx].mask[cIdx];
//   }

//   // Return the value of cut cIdx for variable vIdx
//   std::string cutVal(size_t vIdx, size_t cIdx) const;

//   // Return the string representation for the cut direction of variable vIdx
//   std::string cutDir(size_t vIdx, size_t m) const; 

// private:
//   size_t nSample_ = 0; // Sample size
//   size_t nCont_ = 0;   // Number of continuous variables
//   size_t nOrd_ = 0;    // Number of ordinal variables
//   size_t nNom_ = 0;    // Number of nominal variables
//   size_t nVar_ = 0;    // Number of variables
//   size_t nResp_ = 0;   // Number of responses for each subject
//   double T0_ = 0.0;    // Sum (Resp | Act = 0)

//   // Array of subject ID
//   std::vector<int> id_;
  
//   // Decile values of each continuous variable
//   std::vector<std::vector<double>> decile_;
  
//   // Unique values of each ordinal variable
//   std::vector<std::set<int>> uniqOrd_;
  
//   // Unique values of each nominal variable
//   std::vector<std::set<int>> uniqNom_;
  
//   struct Cvar {
//     // Values of the cut for this variable
//     std::vector<int> value;
    
//     // Masks of the components that belong to each cut
//     std::vector<std::vector<std::uint32_t>> mask;
//   };
  
//   // Cut and mask information for each variable
//   std::vector<Cvar> cvar_;
  
//   // Action vector A[nSample_];
//   // We focus on the case of developing on optimal action each time, i.e.,
//   // action is a vector. Our design uses 4 bits to store each action to balance
//   // the needs between storage and computation. Type uint32_t is used such that
//   // 8 actions can be packed/unpacked at a time. 
//   std::vector<std::uint32_t> act_;

//   // Response matrix Y[nSample_][nResp_]
//   // Y can be a matrix, such as survival outcomes, incorporate multi-dimensional
//   // measurements.
//   // In current implementation, the memory space and data access are done as if
//   // Y is a matrix. However, the computation involved treats Y as if it is a
//   // single column vector.
//   std::vector<double> resp_;
  
//   // Probability of P(A = 1 | X)
//   std::vector<double> prob_;
  
//   // This function loads the input file. It assumes that fields of the same type
//   // are in consecutive columns. It also assumes that the fields are given in
//   // the order of subjectID, continuous variable, ordinal variable, nominal
//   // variable, action, and responses.
//   void loadCSV(const std::string &input);

//   // This function parses the header of the input file. It counts the number of
//   // continuous, ordinal, and nominal variables, the number of different types
//   // of actions and responses.
//   void parseCSVHeader(std::ifstream &infile);
  
//   // This function reads the raw data of the input file. Covariates are read
//   // into temporary buffer while actions and responses are read into the
//   // internal buffer directly. For ordinal and nominal variables, this function
//   // also collects the unique values.
//   void loadRawData(std::ifstream &infile,
//                    std::vector<std::vector<double>> &cont,
//                    std::vector<std::vector<int>> &ord,
//                    std::vector<std::vector<int>> &nom);

//   // This function parses the raw data of the covariates into the format needed
//   // by the SearchEngine. Values of the continuous variables are converted into
//   // deciles. Values of the ordinal and nominal variables are converted into the
//   // order they appear in the unique sets. Variables corresponding to the same
//   // sample will be stored contiguously at the end of this function.
//   void parseRawData(std::vector<std::vector<double>> &cont,
//                     std::vector<std::vector<int>> &ord,
//                     std::vector<std::vector<int>> &nom);

//   // This function sets up the cut masks for the ith continuous variable
//   void setContCutMasks(size_t vIdx, const std::vector<double> &data);
  
//   // This function sets up the cut masks for the ith ordinal variable
//   void setOrdCutMasks(size_t i, const std::vector<int> &data);
  
//   // This function sets up the cut masks for the ith nominal variable
//   void setNomCutMasks(size_t i, const std::vector<int> &data);
// };



#endif 
