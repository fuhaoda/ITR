#ifndef __COVARIATE_H__
#define __COVARIATE_H__

#include <vector>
#include <set> 


namespace ITR {

class Covariate {
public:
  // Constructor 
  Covariate() {}

  // Insert value 
  void push_back(int v) { data_.push_back(v); }

  // Overload [] operator
  int& operator [] (int idx) { return data_[idx]; }
  int operator [] (int idx) const { return data_[idx]; }

  // Clean up the raw data
  virtual void clean(int idx,
                     const std::vector<std::vector<double>> &temp_cont,
                     const std::vector<std::set<int>> &temp_ord,
                     const std::vector<std::set<int>> &temp_nom);
protected:
  // Value of the covariate
  std::vector<int> data_;

  // Number of cuts
  unsigned nCut_ = 0;

  // Range of the values
  std::vector<int> range_;   
};

class ContCovariate : public Covariate {
public:
  void clean(int idx,
             const std::vector<std::vector<double>> &temp_cont,
             const std::vector<std::set<int>> &temp_ord,
             const std::vector<std::set<int>> &temp_nom) override; 
private:
  void convertContToDeciles(const std::vector<double> &cont); 
};

class OrdCovariate : public Covariate {
public:
  void clean(int idx,
             const std::vector<std::vector<double>> &temp_cont,
             const std::vector<std::set<int>> &temp_ord,
             const std::vector<std::set<int>> &temp_nom) override;
};

class NomCovariate : public Covariate {
public:
  void clean(int idx,
             const std::vector<std::vector<double>> &temp_cont,
             const std::vector<std::set<int>> &temp_ord,
             const std::vector<std::set<int>> &temp_nom) override;
}; 


} // namespace ITR

#endif
