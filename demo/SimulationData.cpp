#include <fstream>
#include <iomanip>
#include "SimulationData.h"

SimulationData::SimulationData(size_t nSample, size_t nCont, size_t nOrd,
                               size_t nNom, size_t nResp):
  nSample_{nSample}, nCont_{nCont}, nOrd_{nOrd}, nNom_{nNom}, nResp_{nResp}
{
  cont_ = std::unique_ptr<double []>{new double[nSample * nCont]()};
  ord_ = std::unique_ptr<int []>{new int[nSample * nOrd]()};
  nom_ = std::unique_ptr<int []>{new int[nSample * nNom]()};
  act_ = std::unique_ptr<int []>{new int[nSample]()};
  resp_ = std::unique_ptr<double []>{new double[nSample * nResp]()};
  prob_ = std::unique_ptr<double []>{new double[nSample]()}; 
}

void SimulationData::save(std::string fname) const {
  std::ofstream ofile(fname);

  // Write header information to file 
  ofile << "ID, ";
  for (size_t i = 0; i < nCont_; ++i)
    ofile << "Cont" << i + 1 << ", ";
  for (size_t i = 0; i < nOrd_; ++i)
    ofile << "Ord" << i + 1 << ", ";
  for (size_t i = 0; i < nNom_; ++i) 
    ofile << "Nom" << i + 1 << ", ";
  ofile << "A1" << ", ";
  for (size_t i = 0; i < nResp_; ++i)
    ofile << "Y" << i + 1 << ", ";
  ofile << "P(A=1|X)" << "\n";

  // Write data to file
  for (size_t i = 0; i < nSample_; ++i) {
    // ID
    ofile << i + 1 << ", ";  

    // Continuous variable
    for (size_t j = 0; j < nCont_; ++j) 
      ofile << std::scientific << cont_[j * nSample_ + i] << ", ";
    
    // Ordinal variable
    for (size_t j = 0; j < nOrd_; ++j)
      ofile << ord_[j * nSample_ + i] << ", ";

    // Nominal variable
    for (size_t j = 0; j < nNom_; ++j)
      ofile << nom_[j * nSample_ + i] << ", ";

    // Action
    ofile << act_[i] << ", "; 

    // Resp
    for (size_t j = 0; j < nResp_; ++j)
      ofile << resp_[j * nSample_ + i] << ", ";

    // Probability
    ofile << prob_[i]; 

    // Change line
    ofile << "\n";
  }

  // Close the output stream
  ofile.close();     
}

