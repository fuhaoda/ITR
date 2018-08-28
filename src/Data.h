#ifndef __DATA_H__
#define __DATA_H__

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include "Covariate.h"

namespace ITR {
  
class Data {
public:
  // Constructor
  Data(std::string input);

  // Return the number of sample size 
  unsigned nSample() const { return nSample_; } 
  
  // Return the number of covariates
  unsigned nCvar() const { return cvar_.size(); }

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
  
private:
  // Load the input data
  void loadCSV(std::string input); 

  // Parse the header of the CSV input file
  void parseCSVHeader(std::ifstream &infile); 

  // Read and parse the raw data of the CSV input file 
  void parseCSVRawData(std::ifstream &infile); 
  
  // Convert values of continuous covariates into deciles
  void convertContToDeciles(const std::vector<std::vector<double>> &cont); 

  unsigned nSample_ = 0; // Sample size
  unsigned nCont_ = 0;   // # of continuous variables
  unsigned nOrd_ = 0;    // # of ordinal variables
  unsigned nNom_ = 0;    // # of nominal variables
  unsigned nAct_ = 0;    // # of different actions
  unsigned nResp_ = 0;   // # of differnet responses 
    
  // Array of subject ID
  std::vector<unsigned> id_; 
  
  // Response matrix Y[sample_size][n_resp_]
  std::vector<double> resp_; 
  
  // Action matrix A[sample_size][n_act_];
  std::vector<int> act_; 

  // Covariates, stored in the order of continuous, ordinal, and nominal.
  // Additionally, values of the continuous covariates are converted into deciles. 
  std::vector<std::unique_ptr<Covariate>> cvar_;   
};


} // namespace ITR

#endif 


// #ifndef DATA_H
// #define DATA_H

// #include <vector>
// #include <string>
// #include "utility.hpp"

// using namespace std;

// class Data
// {
// public:
//     Data();
//     virtual ~Data();
//     virtual void loadData() = 0;                 // Load data

//     const vector<unsigned int>& getID();
//     const vector<vector<double>>& getY();
//     const vector<vector<unsigned int>>& getActions();
//     const vector<vector<double>>& getX_Cont();
//     const vector<vector<unsigned int>>& getX_Ord();
//     const vector<vector<unsigned int>>& getX_Nom();
//     const vector<unsigned int>& getX_Type();
//     unsigned int getSampleSize();

//     void printID();
//     void printY();
//     void printActions();
//     void printX_Cont();
//     void printX_Ord();
//     void printX_Nom();
//     void printX_Type();

// protected:
// 	unsigned int sampleSize;                                /**< Sample Size */
// 	vector<unsigned int> id;                                /**< 1D vector for Patient ID */
// 	vector<vector<double>> y;                               /**< 2D vector for response Y */
// 	vector<vector<unsigned int>> actions;                   /**< 2D vector for actions A */
// 	vector<vector<double>> x_Cont;                          /**< 2D vector for continuous variables */
// 	vector<vector<unsigned int>> x_Ord;                     /**< 2D vector for ordinal variables */
// 	vector<vector<unsigned int>> x_Nom;                     /**< 2D vector for nominal variables */
// 	vector<unsigned int> dataType;                          /**< 1D vector for data type info., 0 for continuous, 1 for ordinal, 2 for nominal */
// };

// #endif // DATA_H
