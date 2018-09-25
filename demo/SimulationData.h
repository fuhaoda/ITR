#ifndef __SIMULATION_DATA__
#define __SIMULATION_DATA__

#include <string> 
#include <memory> 

class SimulationData {
public:
  // Constructor 
  SimulationData(size_t nSample, size_t nCont, size_t nOrd, size_t nNom,
                 size_t nResp = 1); 

  // Generate continuous variable. 
  void generateContVariable(void(*func)(double *, size_t, double, double),
                            double a, double b) {
    generateData<double>(cont_.get(), nCont_, func, a, b);
  }

  // Generate ordinal variable. 
  void generateOrdVariable(void(*func)(int *, size_t, int, int),
                           int a, int b) {
    generateData<int>(ord_.get(), nOrd_, func, a, b);
  }

  // Generate nominal variable. 
  void generateNomVariable(void(*func)(int *, size_t, int, int),
                           int a, int b) {
    generateData<int>(nom_.get(), nNom_, func, a, b);
  }

  // Generate actions. 
  void generateAct(void(*func)(int *, size_t, int, int),
                   int a, int b) {
    generateData<int>(act_.get(), 1, func, a, b);
  }

  // Generate responses.
  void generateResp(void(*func)(double *, size_t, double, double),
                    double a, double b) {
    generateData<double>(resp_.get(), nResp_, func, a, b);
  }

  // Generate condition probability P(A = 1 | X)
  void generateProb() {
    for (size_t i = 0; i < nSample_; ++i)
      prob_[i] = 0.5;
  }
 
  // Save the generated data into a csv file
  void save(std::string fname = std::string{"simData.csv"}) const;   
  
private:
  size_t nSample_; // Number of samples to generate
  size_t nCont_;   // Number of continuous variables to generate
  size_t nOrd_;    // Number of ordinal variables to generate
  size_t nNom_;    // Number of nominal variables to generate
  size_t nResp_;   // Number of different responses

  std::unique_ptr<double []> cont_; // Buffer for continuous variable 
  std::unique_ptr<int []> ord_;     // Buffer for ordinal variable
  std::unique_ptr<int []> nom_;     // Buffer for nominal variable
  std::unique_ptr<int []> act_;     // Buffer for action
  std::unique_ptr<double []> resp_; // Buffer for response
  std::unique_ptr<double []> prob_; // Buffer for conditional probability

  template<typename T>
  void generateData(T *matrix, size_t nCol,
                    void(*func)(T *, size_t, T, T), T a, T b) {
    for (size_t i = 0; i < nCol; ++i) 
      func(matrix + i * nSample_, nSample_, a, b); 
  }  
};

#endif 
