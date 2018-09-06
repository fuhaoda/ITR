#include <fstream>
#include <iomanip>
#include "SimulationData.h"

SimulationData::SimulationData(int nSample, int nCont, int nOrd, int nNom,
                               int nAct, int nResp):
  nSample_{nSample}, nCont_{nCont}, nOrd_{nOrd}, nNom_{nNom},
  nAct_{nAct}, nResp_{nResp}
{
  cont_ = std::unique_ptr<double []>{new double[nSample * nCont]()};
  ord_ = std::unique_ptr<int []>{new int[nSample * nOrd]()};
  nom_ = std::unique_ptr<int []>{new int[nSample * nNom]()};
  act_ = std::unique_ptr<int []>{new int[nSample * nAct]()};
  resp_ = std::unique_ptr<double []>{new double[nSample * nResp]()}; 
}

void SimulationData::generateContVariable(void(*func)(double *arr, int n)) {
  generateData<double>(cont_.get(), nCont_, func); 
}
  
void SimulationData::generateOrdVariable(void(*func)(int *arr, int n)) {
  generateData<int>(ord_.get(), nOrd_, func); 
}

void SimulationData::generateNomVariable(void(*func)(int *arr, int n)) {
  generateData<int>(nom_.get(), nNom_, func);
}

void SimulationData::generateAct(void(*func)(int *arr, int n)) {
  generateData<int>(act_.get(), nAct_, func);
}

void SimulationData::generateResp(void(*func)(double *arr, int n)) {
  generateData<double>(resp_.get(), nResp_, func);
}

void SimulationData::save(std::string fname) const {
  std::ofstream ofile(fname);

  // Write header information to file 
  ofile << "ID, ";
  for (auto i = 0; i < nCont_; ++i)
    ofile << "Cont" << i + 1 << ", ";
  for (auto i = 0; i < nOrd_; ++i)
    ofile << "Ord" << i + 1 << ", ";
  for (auto i = 0; i < nNom_; ++i) 
    ofile << "Nom" << i + 1 << ", ";
  for (auto i = 0; i < nAct_; ++i)
    ofile << "A" << i + 1 << ", ";
  for (auto i = 0; i < nResp_; ++i)
    ofile << "Y" << i + 1 << ", ";
  ofile << "\n";

  // Write data to file
  for (auto i = 0; i < nSample_; ++i) {
    // ID
    ofile << i + 1 << ", ";  

    // Continuous variable
    for (auto j = 0; j < nCont_; ++j) 
      ofile << std::scientific << cont_[j * nSample_ + i] << ", ";
    
    // Ordinal variable
    for (auto j = 0; j < nOrd_; ++j)
      ofile << ord_[j * nSample_ + i] << ", ";

    // Nominal variable
    for (auto j = 0; j < nNom_; ++j)
      ofile << nom_[j * nSample_ + i] << ", ";

    // Action
    for (auto j = 0; j < nAct_; ++j)
      ofile << act_[j * nSample_ + i] << ", ";

    // Resp
    for (auto j = 0; j < nResp_; ++j)
      ofile << resp_[j * nSample_ + i] << ", ";

    // Change line
    ofile << "\n";
  }

  // Close the output stream
  ofile.close();     
}

