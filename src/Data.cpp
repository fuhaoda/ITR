#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric> 
#include "Data.h"

namespace ITR {

// Sort an input array and keep tracking of index
// https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
template <typename T>
std::vector<std::size_t> sort_indices(const std::vector<T> &v) {
  // Initialize original index locations
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // Sort indices based on comparing values in v
  std::sort(idx.begin(), idx.end(),
            [&v](std::size_t i1, std::size_t i2) {return v[i1] < v[i2];});

  return idx;
}

Data::Data(std::string input)
  : sample_size_{0}, n_cont_{0}, n_ord_{0},
    n_nom_{0}, n_act_{0}, n_resp_{0}
{
  // TODO:
  // Switch different load function based on the file format 
  loadCSV(input); 
} 

void Data::loadCSV(std::string input) {
  std::ifstream infile(input);
  std::string line; 
  std::istringstream ss;
  std::string field;

  // Parse the header of the CSV file
  getline(infile, line);
  ss.str(line);

  while (ss.good()) {
    getline(ss, field, ',');

    if (field.find("cont") != std::string::npos) {
      n_cont_++;
    } else if (field.find("ord") != std::string::npos) {
      n_ord_++;
    } else if (field.find("nom") != std::string::npos) {
      n_nom_++;
    } else if (field.find("A") != std::string::npos) {
      n_act_++;
    } else if (field.find("Y") != std::string::npos) {
      n_resp_++;
    }
  }

  // Load the remaining data 
  cvar_.resize(n_cont_ + n_ord_ + n_nom_); 
    
  // Values of the continuous covariates are saved in a temporary buffer first
  std::vector<std::vector<double>> cont_buffer; 
  
  while (getline(infile, line)) {
    sample_size_++;

    ss.clear();
    ss.str(line);

    // first read the subject ID
    getline(ss, field, ',');
    id_.push_back(stoi(field));

    // read continuous covariates
    for (auto i = 0; i < n_cont_; ++i) {
      getline(ss, field, ',');
      cont_buffer[i].push_back(stod(field));
    }

    auto iter = n_cont_;    

    // read ordinal covariates
    for (auto i = 0; i < n_ord_; ++i) {
      getline(ss, field, ',');
      cvar_[iter++].data.push_back(stoi(field));
    }

    // read nominal covariates
    for (auto i = 0; i < n_nom_; ++i) {
      getline(ss, field, ',');
      cvar_[iter++].data.push_back(stoi(field));
    }

    // read actions
    for (auto i = 0; i < n_act_; ++i) {
      getline(ss, field, ',');
      act_.push_back(stoi(field));
    }

    // read responses
    for (auto i = 0; i < n_resp_; ++i) {
      getline(ss, field, ',');
      resp_.push_back(stod(field));
    }
  }

  // Convert values of continuous covariates into deciles
  clean(cont_buffer); 
 
  // Close the input file
  infile.close();
}  

void Data::clean(const std::vector<std::vector<double>> &continuous) {
  const double scaling_factor = 10.0 / sample_size_; 
  
  for (auto i = 0; i < n_cont_; ++i) {
    auto sorted = sort_indices(continuous[i]);

    // Resize the vector for covariate i
    cvar_[i].data.resize(sample_size_);
    
    // Populate covariate i     
    for (auto j = 0; j < sample_size_; ++j) {
      auto k = sorted[j];

      // The kth component of the covariate is in the jth position of the sorted
      // value, its percentile is then 100 * (j + 1 - 0.5) / sample_size. The
      // decile value is obtained by taking the floor of the above value divided
      // by 10. As we are operating on positive values here, this is done by a
      // cast from the double value to the integer value. 
      cvar_[i].data[k] = (j + 0.5) * scaling_factor;
    }
  }
}



} // namespace ITR



// #include "pch.h"
// #include "Data.h"

// Data::Data()
// {
//     //ctor
// }

// Data::~Data()
// {
//     //dtor
// }

// const vector<unsigned int>& Data :: getID()
// {
//     return id;
// }

// const vector<vector<double>>& Data :: getY()
// {
//     return y;
// }

// const vector<vector<unsigned int>>& Data :: getActions()
// {
//     return actions;
// }


// const vector<vector<double>>& Data :: getX_Cont()
// {
//     return x_Cont;
// }

// const vector<vector<unsigned int>>& Data :: getX_Ord()
// {
//     return x_Ord;
// }

// const vector<vector<unsigned int>>& Data :: getX_Nom()
// {
//     return x_Nom;
// }

// const vector<unsigned int>& Data::getX_Type()
// {
//     return dataType;
// }

// unsigned int Data::getSampleSize(){
//     return sampleSize;
// }

// void Data::printID()
// {
//     utility::print1DVector(id);
// }

// void Data::printY()
// {
//     utility::print2DVector(y);
// }

// void Data::printActions()
// {
//     utility::print2DVector(actions);
// }

// void Data::printX_Cont()
// {
//     utility::print2DVector(x_Cont);
// }

// void Data::printX_Ord()
// {
//     utility::print2DVector(x_Ord);
// }

// void Data::printX_Nom()
// {
//     utility::print2DVector(x_Nom);
// }

// void Data::printX_Type()
// {
//     utility::print1DVector(dataType);
// }
