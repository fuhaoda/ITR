#ifndef __SIMULATION_DATA__
#define __SIMULATION_DATA__

#include <string> 
#include <memory> 

class SimulationData {
public:
  // Constructor 
  SimulationData(int nSample, int nCont, int nOrd, int nNom,
                 int nAct = 1, int nResp = 1); 

  // Generate continuous variable. 
  void generateContVariable(void(*func)(double *arr, int n));

  // Generate ordinal variable. 
  void generateOrdVariable(void(*func)(int *arr, int n));

  // Generate nominal variable. 
  void generateNomVariable(void(*func)(int *arr, int n)); 

  // Generate actions. 
  void generateAct(void(*func)(int *arr, int n));

  // Generate responses.
  void generateResp(void(*func)(double *arr, int n)); 
  
  // Save the generated data into a csv file
  void save(std::string fname = std::string{"simData.csv"}) const;   
  
private:
  int nSample_; // Number of samples to generate
  int nCont_;   // Number of continuous variables to generate
  int nOrd_;    // Number of ordinal variables to generate
  int nNom_;    // Number of nominal variables to generate
  int nAct_;    // Number of different actions
  int nResp_;   // Number of different responses

  std::unique_ptr<double []> cont_; // Buffer for continuous variable 
  std::unique_ptr<int []> ord_;     // Buffer for ordinal variable
  std::unique_ptr<int []> nom_;     // Buffer for nominal variable
  std::unique_ptr<int []> act_;     // Buffer for action
  std::unique_ptr<double []> resp_; // Buffer for response

  template<typename T> void generateData(T *matrix, int nCol,
                                         void(*func)(T *arr, int n)) {
    for (auto i = 0; i < nCol; ++i) 
      func(matrix + i * nSample_, nSample_); 
  }  
};

#endif 
