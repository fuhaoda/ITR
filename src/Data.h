#ifndef __DATA_H__
#define __DATA_H__

#include <vector>

namespace ITR {

// Forward declaration of Covariate class 
class Covariate {
public:

private:
  unsigned type_;
  unsigned cut_size_;
}; 

class Data {
public:
  // Constructor
  Data();

private:
  unsigned sample_size_;  // Size of the population 

  unsigned n_resp_;       // Number of different responses
  unsigned n_act_;        // Number of different actions
  unsigned n_cont_;       // Number of continuous variables
  unsigned n_ord_;        // Number of ordinal variables
  unsigned n_nom_;        // Number of nominal variables
  
  // Response matrix Y[sample_size][n_resp_]
  std::vector<double> resp_; 
  
  // Action matrix A[sample_size][n_act_];
  std::vector<unsigned> act_; 

  // Collection of covariates 
  std::vector<Covariate> cvar_; 
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
