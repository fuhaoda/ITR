#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <cstdint>
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
  // and the number of actions and responses. It requires the input file has
  // aligned format. 
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
  // Assuming Y is a vector
  for (size_t i = 0; i < nSample_; ++i) {
    resp_[i * nResp_] /= (act_[i] ? prob_[i] : 1 - prob_[i]);
    T0_ += resp_[i * nResp_] * (1 - act_[i]);
  }

  cvar_.resize(nVar_); 

  // Parse continuous variables and set up cut masks
  for (size_t i = 0; i < nCont_; ++i) {
    convertContToDeciles(cont[i], decile_[i]);
    setContCutMasks(i, cont[i]); 
  }
  
  // Parse ordinal variables
  for (size_t i = 0; i < nOrd_; ++i) {
    convertOrdToRanks(ord[i], uniqOrd_[i]);
    setOrdCutMasks(i, ord[i]);
  }
    
  // Parse nominal variables
  for (size_t i = 0; i < nNom_; ++i) {
    convertNomToBitMasks(nom[i], uniqNom_[i]);
    setNomCutMasks(i, nom[i]);
  }
}

void Data::setContCutMasks(size_t i, const std::vector<double> &cont) {
  // Compute the overall variable index
  size_t vIdx = i;

  // Get the handle of the data
  const auto &data = cont.data();

  // Continuous variable has 10 cuts
  for (int value = 1; value <= 10; ++value) {
    size_t r = nSample_ % 2; 
    size_t nBatches = nSample_ >> 1;     
    std::vector<std::uint8_t> mask(nBatches + (r > 0)); 
    
    for (size_t j = 0; j < nBatches; ++j) {
      // Mask for sample 2j is stored in bits 4-7
      mask[j] = (data[2 * j] < value) << 4;
      // Mask for sample 2j+1 is stored in bits 0-3      
      mask[j] |= (data[2 * j + 1] < value);
    }
    
    // TODO: fix this
    if (r) {
      // Mask for the last sample is stored in bits 4-7
      mask[nBatches] = (data[nSample_ - 1] < value) << 4; 
    }

    cvar_[vIdx].mask.push_back(mask);
    cvar_[vIdx].value.push_back(value - 1);      
  }
}

void Data::setOrdCutMasks(size_t i, const std::vector<int> &ord) {
  // Compute the overall variable index
  size_t vIdx = i + nCont_;

  // Get the handle of the data
  const auto &data = ord.data();

  // The number of cuts for ordinal variable vIdx is the number of the unique
  // values.
  for (const auto &value : uniqOrd_[vIdx - nCont_]) {
    size_t r = nSample_ % 2;
    size_t nBatches = nSample_ >> 1;
    std::vector<std::uint8_t> mask(nSample_ + (r > 0));  

    for (size_t j = 0; j < nBatches; ++j) {
      // Mask for sample 2j is stored in bits 4-7
      mask[j] = (data[2 * j] <= value) << 4;         
      // Mask for sample 2j+1 is stored in bits 0-3
      mask[j] |= (data[2 * j + 1] <= value);
    }
      
    // TODO: fix this
    if (r) {
      // Mask for the last sample is stored in bits 4-7
      mask[nBatches] = (data[nSample_ - 1] <= value) << 4;
    }      
    cvar_[vIdx].mask.push_back(mask);
    cvar_[vIdx].value.push_back(value);
  }
}

void Data::setNomCutMasks(size_t i, const std::vector<int> &nom) {
  // Compute the overall variable index
  size_t vIdx = i + nCont_ + nOrd_;

  // Get the handle of the data
  const auto &data = nom.data();

  // The number of cuts for nominal variable vIdx is the number of subsets
  // that contain no more than half of the unique values.

  // Denote the number of unique values by p. Here, we loop from 0 to
  // 2^p. Each iterator is interpreted as a bitmask, where bit j means the jth
  // element in the unique set. For each integer, if the number of 1 bits is
  // no more than half of p, it then represents a valid cut, and the subset
  // consists of the elements corresponding to the 1 bits in the integer.
  size_t p = uniqNom_[i].size();
  size_t max = 1u << p;
  size_t half = p / 2; 

  for (size_t value = 0; value < max; ++value) {
    // TODO: The following works only if there are no more than 64 unique
    // values. 
    std::bitset<64> subset(value);
    if (subset.count() <= half) {
      size_t r = nSample_ % 2;
      size_t nBatches = nSample_ >> 1;
      std::vector<std::uint8_t> mask(nBatches + (r > 0));
      
      for (size_t j = 0; j < nBatches; ++j) {
        // Mask for sample 2j is stored in bits 4-7
        mask[j] = ((data[2 * j] & value) > 0 ) << 4;
        // Mask for sample 2j+1 is stored in bits 0-3
        mask[j] |= (data[2 * j + 1] & value) > 0;
      }
      
      // TODO: Fix this
      if (r) {
        // Mask for the last sample is stored in bits 4-7
        mask[nBatches] = ((data[nSample_ - 1] & value) > 0 ) << 4;
      }      
      cvar_[vIdx].mask.push_back(mask);
      cvar_[vIdx].value.push_back(static_cast<int>(value));
    }
  }   
}

std::string Data::cutInfo(size_t vIdx, size_t cIdx, bool m) const {
  std::stringstream info;
  if (vIdx < nCont_) {
    info << " X" << vIdx << (m ? " < " : " >= ")
         << decile_[vIdx][cIdx] << ", ";
  } else if (vIdx < nCont_ + nOrd_) {
    info << " X" << vIdx << (m ? " < " : " >= ")
         << cvar_[vIdx].value[cIdx] << ", ";
  } else {
    info << " X" << vIdx << (m ? " in " : " not in ") << "{";
    auto subset = cvar_[vIdx].value[cIdx];

    if (subset == 0) {
      // This represents an empty set
      info << " }\n";
    } else {   
      unsigned iter = 0; 
      for (auto const &v : uniqOrd_[vIdx - nCont_ - nOrd_]) {
        if (subset & (1 << (iter++)))
          info << v << ", "; 
      }
      info <<"\b\b}, ";
    }
  }

  return info.str(); 
}

} // namespace ITR
