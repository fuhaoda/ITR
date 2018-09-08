#ifndef __SIMULATION_DATA__
#define __SIMULATION_DATA__

#include <string> 
#include <memory> 

class SimulationData {
public:
  // Constructor 
  SimulationData(size_t nSample, size_t nCont, size_t nOrd, size_t nNom,
                 size_t nAct = 1, size_t nResp = 1); 

  // Generate continuous variable. 
  void generateContVariable(void(*func)(double *arr, size_t n));

  // Generate ordinal variable. 
  void generateOrdVariable(void(*func)(int *arr, size_t n));

  // Generate nominal variable. 
  void generateNomVariable(void(*func)(int *arr, size_t n)); 

  // Generate actions. 
  void generateAct(void(*func)(int *arr, size_t n));

  // Generate responses.
  void generateResp(void(*func)(double *arr, size_t n)); 
  
  // Save the generated data into a csv file
  void save(std::string fname = std::string{"simData.csv"}) const;   
  
private:
  size_t nSample_; // Number of samples to generate
  size_t nCont_;   // Number of continuous variables to generate
  size_t nOrd_;    // Number of ordinal variables to generate
  size_t nNom_;    // Number of nominal variables to generate
  size_t nAct_;    // Number of different actions
  size_t nResp_;   // Number of different responses

  std::unique_ptr<double []> cont_; // Buffer for continuous variable 
  std::unique_ptr<int []> ord_;     // Buffer for ordinal variable
  std::unique_ptr<int []> nom_;     // Buffer for nominal variable
  std::unique_ptr<int []> act_;     // Buffer for action
  std::unique_ptr<double []> resp_; // Buffer for response

  template<typename T> void generateData(T *matrix, size_t nCol,
                                         void(*func)(T *arr, size_t n)) {
    for (size_t i = 0; i < nCol; ++i) 
      func(matrix + i * nSample_, nSample_); 
  }  
};

#endif 
