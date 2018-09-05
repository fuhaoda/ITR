#include "SimulationData.h"

SimulationData::SimulationData(int nSample, int nCont, int nOrd, int nNom,
                               int nAct, int nResp):
  nSample_{nSample}, nCont_{nCont}, nOrd_{nOrd}, nNom_{nNom},
  nAct_{nAct}, nResp_{nResp}
{
  cont_ = std::unique_ptr<double []>{new double[nSample * nCont]};
  ord_ = std::unique_ptr<int []>{new int[nSample * nOrd]};
  nom_ = std::unique_ptr<int []>{new int[nSample * nNom]};
  act_ = std::unique_ptr<int []>{new int[nSample * nAct]};
  resp_ = std::unique_ptr<double []>{new double[nSample * nResp]}; 
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

void SimulationData::save(std::string ofname) const {

}
