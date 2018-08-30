#include <sstream>
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

  // Allocate temporary buffer to load the raw data
  std::vector<std::vector<double>> cont(nCont_);
  std::vector<std::vector<int>> ord(nOrd_);
  std::vector<std::vector<int>> nom(nNom_);

  uniqOrd_.resize(nOrd_);
  uniqNom_.resize(nNom_); 
  
  // Load raw data of the input file
  loadRawData(infile, cont, ord, nom); 
  
  // Close the input file
  infile.close(); 

  // Parse raw data of the input file
  parseRawData(cont, ord, nom); 
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

void Data::loadRawData(std::ifstream &infile,
                       std::vector<std::vector<double>> &cont,
                       std::vector<std::vector<int>> &ord,
                       std::vector<std::vector<int>> &nom) {
  std::string line; 
  std::istringstream ss;
  std::string field;

  while (getline(infile, line)) {
    nSample_++;

    ss.clear();
    ss.str(line);

    // Read subject ID
    getline(ss, field, ',');
    id_.push_back(stoi(field));

    // Read continuous variables
    for (auto i = 0; i < nCont_; ++i) {
      getline(ss, field, ',');
      cont[i].push_back(stod(field)); 
    }

    // Read ordinal variables
    for (auto i = 0; i < nOrd_; ++i) {
      getline(ss, field, ',');
      auto val = stoi(field);
      ord[i].push_back(val);
      uniqOrd_[i].insert(val);
    }

    // Read nominal variables
    for (auto i = 0; i < nNom_; ++i) {
      getline(ss, field, ',');
      auto val = stoi(field);
      nom[i].push_back(val);
      uniqNom_[i].insert(val);
    }

    // Read actions
    // TODO: handle generic case other than 0-1 boolean 
    for (auto i = 0; i < nAct_; ++i) {
      getline(ss, field, ',');
      act_.push_back(stoi(field));
    }
    
    for (auto i = 0; i < nResp_; ++i) {
      getline(ss, field, ',');
      resp_.push_back(stod(field));
    }
  }
}

void Data::parseRawData(std::vector<std::vector<double>> &cont,
                        std::vector<std::vector<int>> &ord,
                        std::vector<std::vector<int>> &nom) {
  // Parse continuous variables
  for (auto i = 0; i < nCont_; ++i)
    convertToDeciles(cont[i]);

  // Parse ordinal variables
  for (auto i = 0; i < nOrd_; ++i)
    convertToRanks(ord[i], uniqOrd_[i]);

  // Parse nominal variables
  for (auto i = 0; i < nNom_; ++i)
    convertToRanks(nom[i], uniqNom_[i]); 

  // Write the parsed values into cvar_, where variables corresponding to the
  // same sample are stored contiguously
  cvar_.resize(nSample_ * (nCont_ + nOrd_ + nNom_));

  int iter = 0;
  for (auto i = 0; i < nSample_; ++i) {
    for (auto j = 0; j < nCont_; ++j)
      cvar_[iter++] = (int) cont[j][i];

    for (auto j = 0; j < nOrd_; ++j)
      cvar_[iter++] = ord[j][i];

    for (auto j = 0; j < nNom_; ++j)
      cvar_[iter++] = nom[j][i];
  }
}

} // namespace ITR


