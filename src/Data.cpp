#include <sstream> 
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <bitset>
#include <sys/stat.h>
#include "Data.h"
#include "Covariate.h"

namespace ITR {

Data::Data(const std::string &input) {
  struct stat buffer{};
  if (stat(input.c_str(), &buffer) != 0)
    throw "Input file does not exist!";
  
  // TODO:
  // Switch different load function based on the file format 
  loadCSV(input); 
} 

void Data::loadCSV(const std::string &input) {
  // Open the input CSV file
  std::ifstream infile(input);

  // Parse the header of the input file
  parseCSVHeader(infile);

  // Allocate temporary buffer to load the raw data
  std::vector<std::vector<double>> cont(nCont_);
  std::vector<std::vector<int>> ord(nOrd_);
  std::vector<std::vector<int>> nom(nNom_);

  decile_.resize(nCont_); 
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
    } else if (field.find('Y') != std::string::npos) {
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
    for (size_t i = 0; i < nCont_; ++i) {
      getline(ss, field, ',');
      cont[i].push_back(stod(field)); 
    }

    // Read ordinal variables
    for (size_t i = 0; i < nOrd_; ++i) {
      getline(ss, field, ',');
      auto val = stoi(field);
      ord[i].push_back(val);
      uniqOrd_[i].insert(val);
    }

    // Read nominal variables
    for (size_t i = 0; i < nNom_; ++i) {
      getline(ss, field, ',');
      auto val = stoi(field);
      nom[i].push_back(val);
      uniqNom_[i].insert(val);
    }

    // Read action
    getline(ss, field, ',');
    act_.push_back(stoi(field));

    // Read responses
    for (size_t i = 0; i < nResp_; ++i) {
      getline(ss, field, ',');
      resp_.push_back(stod(field));
    }

    // Read P(A = 1| X)
    getline(ss, field, ',');
    prob_.push_back(stod(field)); 
  }
}

void Data::parseRawData(std::vector<std::vector<double>> &cont,
                        std::vector<std::vector<int>> &ord,
                        std::vector<std::vector<int>> &nom) {
  // Compute "scaled" response: Y / P(A | X) and T0
  for (size_t i = 0; i < nSample_; ++i) {
    resp_[i * nResp_] /= (act_[i] ? prob_[i] : 1 - prob_[i]);
    T0_ += resp_[i * nResp_] * (1 - act_[i]);
  }
  
  // Parse continuous variables and set up cut masks
  for (size_t i = 0; i < nCont_; ++i) 
    convertContToDeciles(cont[i], decile_[i]);

  // Parse ordinal variables
  for (size_t i = 0; i < nOrd_; ++i)
    convertOrdToRanks(ord[i], uniqOrd_[i]);

  // Parse nominal variables
  for (size_t i = 0; i < nNom_; ++i)
    convertNomToBitMasks(nom[i], uniqNom_[i]); 

  // Write parsed values into cvar_, where samples corresponding to the same
  // variable are stored contiguously. 
  cvar_.resize(nSample_ * nVar_);

  for (size_t i = 0; i < nCont_; ++i) {
    for (size_t j = 0; j < nSample_; ++j) {
      cvar_[i * nSample_ + j] = static_cast<int>(cont[i][j]); 
    }
  }

  auto v = cvar_.data() + nCont_ * nSample_; 
  for (size_t i = 0; i < nOrd_; ++i) {
    memcpy(v, ord[i].data(), sizeof(int) * nSample_);
    v += nSample_;
  }

  for (size_t i = 0; i < nNom_; ++i) {
    memcpy(v, nom[i].data(), sizeof(int) * nSample_);
    v += nSample_; 
  }

  // Set cut masks for each variable
  cMask_.resize(nVar_); 
  for (size_t i = 0; i < nVar_; ++i)
    setCutMasks(i); 
}

void Data::setCutMasks(size_t vIdx) {
  // Get the handle of the data
  const auto data = cvar_.data() + vIdx * nSample_;

  if (vIdx < nCont_) {
    // Continuous variable has 10 cuts
    for (int value = 0; value < 10; ++value) {
      std::vector<bool> mask(nSample_);
      for (size_t j = 0; j < nSample_; ++j)
        mask[j] = data[j] <= value; 
  
      cMask_[vIdx].value.push_back(value);
      cMask_[vIdx].mask.push_back(mask);
    }
  } else if (vIdx < nCont_ + nOrd_) {
    // The number of cuts for ordinal variable vIdx is the number of the unique
    // values.

    for (const auto &value : uniqOrd_[vIdx - nCont_]) {
      std::vector<bool> mask(nSample_);
      for (size_t j = 0; j < nSample_; ++j)
        mask[j] = data[j] <= value;

      cMask_[vIdx].value.push_back(value);
      cMask_[vIdx].mask.push_back(mask);
    }
  } else {
    // The number of cuts for nominal variable vIdx is the number of subsets
    // that contain no more than half of the unique values.

    // Denote the number of unique values by p. Here, we loop from 0 to
    // 2^p. Each iterator is interpreted as a bitmask, where bit j means the jth
    // element in the unique set. For each integer, if the number of 1 bits is
    // no more than half of p, it then represents a valid cut, and the subset
    // consists of the elements corresponding to the 1 bits in the integer.
    size_t p = uniqNom_[vIdx - nCont_ - nOrd_].size();
    size_t max = 1 << p;
    size_t half = p / 2; 

    for (size_t value = 0; value < max; ++value) {
      std::bitset<64> subset(value);
      if (subset.count() <= half) {
        std::vector<bool> mask(nSample_);
        for (size_t j = 0; j < nSample_; ++j)
          mask[j] = data[j] & value;

        cMask_[vIdx].value.push_back(static_cast<int>(value));
        cMask_[vIdx].mask.push_back(mask); 
      }
    }   
  }
}

void Data::cutInfo(size_t vIdx, size_t cIdx, bool m) const {
  if (vIdx < nCont_) {
    std::cout << "  X" << vIdx << (m ? " < " : " >= ")
              << decile_[vIdx][cIdx] << "\n";
  } else if (vIdx < nCont_ + nOrd_) {
    std::cout << "  X" << vIdx << (m ? " < " : " >= ")
              << cMask_[vIdx].value[cIdx] << "\n";
  } else {
    std::cout << "  X" << vIdx << (m ? " in " : " not in ") << "{";
    auto subset = cMask_[vIdx].value[cIdx];

    if (subset == 0) {
      // This represents an empty set
      std::cout << " }\n";
    } else {   
      unsigned iter = 0; 
      for (auto const &v : uniqOrd_[vIdx - nCont_ - nOrd_]) {
        if (subset & (1 << (iter++)))
          std::cout << v << ", "; 
      }
      std::cout <<"\b\b}\n";
    }
  }
}

} // namespace ITR

