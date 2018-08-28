#include <sstream>
#include <algorithm>
#include <numeric>
#include <set> 
#include "Data.h"
#include "Covariate.h"

namespace ITR {

Data::Data(std::string input) {
  // TODO:
  // Switch different load function based on the file format 
  loadCSV(input); 
} 

void Data::loadCSV(std::string input) {
  // Open the input CSV file
  std::ifstream infile(input);

  // Parse the header of the input file
  parseCSVHeader(infile);

  // Load the raw data of the input file 
  parseCSVRawData(infile); 
  
  // Close the input file
  infile.close(); 
}

void Data::parseCSVHeader(std::ifstream &infile) {
  // This function counts the number of continuous, ordinal, nominal variables,
  // and the number of actions and responses.
  std::string line;
  std::istringstream ss;
  std::string field;

  getline(infile, line);
  ss.str(line);

  while (ss.good()) {
    getline(ss, field, ',');
    
    if (field.find("cont") != std::string::npos) {
      nCont_++; 
    } else if (field.find("ord") != std::string::npos) {
      nOrd_++;
    } else if (field.find("nom") != std::string::npos) {
      nNom_++; 
    } else if (field.find("A") != std::string::npos) {
      nAct_++;
    } else if (field.find("Y") != std::string::npos) {
      nResp_++;
    }
  }
}

void Data::parseCSVRawData(std::ifstream &infile) {
  std::string line; 
  std::istringstream ss;
  std::string field;

  for (auto i = 0; i < nCont_; ++i)
    cvar_.push_back(std::make_unique<ContCovariate>());

  for (auto i = 0; i < nOrd_; ++i)
    cvar_.push_back(std::make_unique<OrdCovariate>());

  for (auto i = 0; i < nNom_; ++i)
    cvar_.push_back(std::make_unique<NomCovariate>());

  // Save values of the continuous variables in temporary buffer first and then
  // convert them into deciles
  std::vector<std::vector<double>> temp_cont(nCont_);

  // Save the unique values of the ordinal and nominal variables while reading
  // them in.
  std::vector<std::set<int>> temp_ord(nOrd_);
  std::vector<std::set<int>> temp_nom(nNom_);

  // Read in the data line by line 
  while (getline(infile, line)) {

    // Increment the counter for population size
    nSample_++; 

    ss.clear();
    ss.str(line);

    // Read subject ID
    getline(ss, field, ',');
    id_.push_back(stoi(field));
    
    // Read continuous variable
    for (auto i = 0; i < nCont_; ++i) {      
      getline(ss, field, ',');
      temp_cont[i].push_back(stod(field));
    }

    // Ordinal variables are saved after continuous variables    
    auto iter = nCont_;

    // Read ordinal variable and collect the unique values
    for (auto i = 0; i < nOrd_; ++i) {
      getline(ss, field, ',');

      auto val = stoi(field);       
      cvar_[iter++]->push_back(val);
      temp_ord[i].insert(val); 
    }

    // Read nominal variable and collect the unique values
    for (auto i = 0; i < nNom_; ++i) {
      getline(ss, field, ',');

      auto val = stoi(field);
      cvar_[iter++]->push_back(val);
      temp_nom[i].insert(val);
    }

    // Unlike variables that are operated in column-major, actions and responses
    // will be operated in row-major. 
    
    // NOTE: the code assumes that the input is 0-1 boolean
    // TODO: handle more generic input values
    for (auto i = 0; i < nAct_; ++i) {
      getline(ss, field, ',');
      act_.push_back(stoi(field)); 
    }

    for (auto i = 0; i < nResp_; ++i) {
      getline(ss, field, ',');
      resp_.push_back(stod(field));
    }
  }

  // The following tasks can be done in parallel 
  for (auto i = 0; i < nCont_; ++i) 
    cvar_[i]->clean(i, temp_cont, temp_ord, temp_nom);


  for (auto i = 0; i < nOrd_; ++i)
    cvar_[i]->clean(i, temp_cont, temp_ord, temp_nom);

  for (auto i = 0; i < nNom_; ++i)
    cvar_[i]->clean(i, temp_cont, temp_ord, temp_nom); 
}  

} // namespace ITR


