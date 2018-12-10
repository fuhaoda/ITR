#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cassert> 
#include <sys/stat.h>
#include "data.h"

std::unique_ptr<Data> data; 

Data::Data(const std::string &input) {
  struct stat buffer{};
  if (stat(input.c_str(), &buffer) != 0)
    throw "Input file does not exist!";

  load_csv(input);
}

void Data::load_csv(const std::string &input) {
  // Open the input csv file
  std::ifstream infile(input);

  // Parse the header of the input file
  parse_csv_header(infile);

  // Resize the buffer
  cont_.resize(ncont_);
  ord_.resize(nord_);
  nom_.resize(nnom_);

  // Read raw data of the input file
  load_raw_data(infile);

  // Close the input file
  infile.close();
}

void Data::parse_csv_header(std::ifstream &infile) {
  // This function counts the number of continuous, ordinal, nominal variables.
  // It requires the input file has aligned format.  
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
      ncont_++;
    } else if (field.find("ORD") != std::string::npos) {
      nord_++;
    } else if (field.find("NOM") != std::string::npos) {
      nnom_++;
    } 
  }

  nvar_ = ncont_ + nord_ + nnom_;
}

void Data::load_raw_data(std::ifstream &infile) {
  std::string line;
  std::istringstream ss;
  std::string field;

  while (getline(infile, line)) {
    ss.clear();
    ss.str(line);

    // Read subject ID
    getline(ss, field, ',');
    id_.push_back(std::stoi(field));

    // Read continuous variables
    for (size_t i = 0; i < ncont_; ++i) {
      getline(ss, field, ',');
      cont_[i].push_back(std::stod(field));
    }

    // Read ordinal variables
    for (size_t i = 0; i < nord_; ++i) {
      getline(ss, field, ',');
      ord_[i].push_back(std::stoi(field));
    }

    // Read nominal variables
    for (size_t i = 0; i < nnom_; ++i) {
      getline(ss, field, ',');
      nom_[i].push_back(std::stoi(field));
    }

    // Read action
    getline(ss, field, ',');
    act_.push_back(std::stoi(field)); 
    
    // Read response
    getline(ss, field, ',');
    resp_.push_back(std::stod(field));

    // Read probability
    getline(ss, field, ',');
    auto prAX = std::stod(field);
    if (prAX < 0.0 || prAX > 1.0) {
      std::cerr << "The propensity score outside [0.0, 1.0]\n";
      exit(-1);
    }
    prob_.push_back(prAX);

    ++nsample_;
  }
}
