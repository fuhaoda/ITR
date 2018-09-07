#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include "Data.h"
#include "Covariate.h"

namespace ITR {

Data::Data(std::string input) {
  struct stat buffer;
  if (stat(input.c_str(), &buffer) != 0)
    throw "Input file does not exist!";
  
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

    // Convert field to all CAPS     
    std::transform(field.begin(), field.end(), field.begin(), ::toupper); 
    
    if (field.find("CONT") != std::string::npos) {
      nCont_++; 
    } else if (field.find("ORD") != std::string::npos) {
      nOrd_++;
    } else if (field.find("NOM") != std::string::npos) {
      nNom_++; 
    } else if (field.find("A") != std::string::npos) {
      nAct_++;
    } else if (field.find("Y") != std::string::npos) {
      nResp_++;
    }
  }

  nVar_ = nCont_ + nOrd_ + nNom_;   
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
  // Set up T0
  for (auto i = 0; i < nSample_; ++i) {
    if (!act_[i * nAct_])
      T0_ += resp_[i * nResp_];
  }
  
  // Parse continuous variables
  for (auto i = 0; i < nCont_; ++i)
    convertContToDeciles(cont[i]);

  // Parse ordinal variables
  for (auto i = 0; i < nOrd_; ++i)
    convertOrdToRanks(ord[i], uniqOrd_[i]);

  // Parse nominal variables
  for (auto i = 0; i < nNom_; ++i)
    convertNomToBitMasks(nom[i], uniqNom_[i]); 

  // Write parsed values into cvar_, where samples corresponding to the same
  // variable are stored contiguously. 
  cvar_.resize(nSample_ * nVar_);

  for (auto i = 0; i < nCont_; ++i) {
    for (auto j = 0; j < nSample_; ++j) {
      cvar_[i * nSample_ + j] = (int) cont[i][j]; 
    }
  }

  auto v = cvar_.data() + nCont_ * nSample_; 
  for (auto i = 0; i < nOrd_; ++i) {
    memcpy(v, ord[i].data(), sizeof(int) * nSample_);
    v += nSample_;
  }

  for (auto i = 0; i < nNom_; ++i) {
    memcpy(v, nom[i].data(), sizeof(int) * nSample_);
    v += nSample_; 
  }
}

int Data::nCut(int i) const {
  if (i < nCont_) {
    return 10; 
  } else if (i < nCont_ + nOrd_) {
    i -= nCont_; 
    return uniqOrd_[i].size();
  } else {
    // The number of cuts for a nominal variable is the number of subsets that
    // no more than half of the unique values. 
    
    // The subsets used to be stored explicitly. The current implementation
    // skips this storage.
    
    // The function here simply returns the number of unique values, denoted
    // by p.
    
    // The search needs to loop through [0, 2^p). Each iterator is interpreted
    // as a bitmask, where bit i corresponds to the ith element in the unique
    // set. For each integer, if the number of 1 bits is no more than p / 2,
    // then it is a valid cut, where the subset consists of the elements
    // corresponding to the 1 bits in the integer. 
    
    // If iterator 'iter' is a valid subset (cut), one can decide if component
    // j of the nominal variable belongs to the subset by performing bitwise
    // AND as component j saves the rank of the original value in the unique
    // set.
    i -= (nCont_ + nOrd_); 
    return uniqNom_[i].size();
  }
}

bool Data::inCut(int i, int j, int k) const {
  // This function compares the ith component of variable j against cut k.
  // If column j is a continuous or ordinal variable, we simply compare the value
  // If column j is a nominal variable, we return the result of bitwise AND
  auto val = cvar_[j * nSample_ + i]; 
  return (j < nCont_ + nOrd_ ? val <= k : val & k);
}

void Data::cutInfo(int i, int j, bool m) const {
  if (i < nCont_) {
    std::cout << "  X" << i << (m ? " < " : " >= ") << j << "\n";
  } else if (i < nCont_ + nOrd_) {
    auto it = uniqOrd_[i].begin();
    std::advance(it, j);     
    std::cout << "  X" << i << (m ? " < " : " >= ") << *it << "\n";
  } else {
    std::cout << "  X" << i << (m ? " in " : " not in ") << "{";
    int iter = 0;
    for (auto const &v : uniqOrd_[i]) {
      if (j & iter)
        std::cout << v << ", ";
      iter++;
    }
    std::cout << "}\n"; 
  }
}


} // namespace ITR


