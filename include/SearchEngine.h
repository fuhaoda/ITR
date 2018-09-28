#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include <vector>
#include "Data.h"

namespace ITR {

class SearchEngine {
public:
  // Constructor
  SearchEngine(const Data *data, unsigned depth, unsigned nThreads);

  // Return the number of search choices to examine
  size_t nChoices() const { return choices_.size() << depth_; }

  // Run the comprehensive search
  void run(); 

  // Report
  void report(size_t nTop) const; 
  
private:
  const Data *data_;
  unsigned depth_;
  unsigned nThreads_; 
  size_t totalChoices_{0};
  
  struct Meta {
    // Choices of vriables used in the search 
    size_t vIdx[3];  // TODO
    // Choices of cuts used in the search 
    size_t cIdx[3]; // TODO
  };

  // Choices of all the variables and cuts combination used in the search
  std::vector<Meta> choices_;

  // Values of the search scores_
  std::vector<double> scores_;

  // This function will loop through all combinations formed by choosing 'depth'
  // number of variables out of all available variables. For each combination
  // found, depending on the mode, it will either call 'count' or 'set' method.
  void combination(std::vector<size_t> curr, std::vector<size_t> option,
                   char mode); 

  // This function counts the number of search options formed by the specified
  // choices of variables
  void countChoices(const std::vector<size_t> &choices);

  // This function sets the number of search options formed by the specified
  // choices of variables
  void setChoices(const std::vector<size_t> &choices, size_t &iter);

  // This function is the helper function for setChoices
  void setChoicesHelper(const std::vector<size_t> &vIdx,
                        std::vector<size_t> cIdx,
                        std::vector<size_t> &cBound,
                        size_t curr, size_t &iter);
  
  // This function is the worker of the search
  void worker(size_t tid); 
}; 


} // namespace ITR

#endif 


  

// template<unsigned depth>
// class SearchEngine {
// public:
//   // Constructor 


// private:



  // // This function computes the number of search choices formed by the
  // // combination of variables i1, ..., id, where 0 <= i1 < i2 < ... < id < max
  
  // // This function sets all the search choices
  // void setSearchChoices() {
  //   auto nVar = data_->nVar();
  //   size_t iter = 0;

  //   if (depth == 1) {
  //     for (size_t i = 0; i < nVar; ++i) {
  //       auto nCut = data_->nCut(i);
  //       for (size_t j = 0; j < nCut; ++j) {
  //         choices_[iter].vIdx[0] = i;
  //         choices_[iter].cIdx[0] = j;
  //         iter++;
  //       }
  //     }
  //   } else if (depth == 2) {
  //     for (size_t i1 = 0; i1 < nVar; ++i1) {
  //       auto nCut1 = data_->nCut(i1);
  //       for (size_t i2 = i1 + 1; i2 < nVar; ++i2) {
  //         auto nCut2 = data_->nCut(i2);
  //         for (size_t j1 = 0; j1 < nCut1; ++j1) {
  //           for (size_t j2 = 0; j2 < nCut2; ++j2) {
  //             choices_[iter].vIdx[0] = i1;
  //             choices_[iter].vIdx[1] = i2;
  //             choices_[iter].cIdx[0] = j1;
  //             choices_[iter].cIdx[1] = j2;
  //             iter++;
  //           }
  //         }
  //       }
  //     }
  //   } else {
  //     for (size_t i1 = 0; i1 < nVar; ++i1) {
  //       auto nCut1 = data_->nCut(i1);
  //       for (size_t i2 = i1 + 1; i2 < nVar; ++i2) {
  //         auto nCut2 = data_->nCut(i2); 
  //         for (size_t i3 = i2 + 1; i3 < nVar; ++i3) {
  //           auto nCut3 = data_->nCut(i3); 
  //           for (size_t j1 = 0; j1 < nCut1; ++j1) {
  //             for (size_t j2 = 0; j2 < nCut2; ++j2) {
  //               for (size_t j3 = 0; j3 < nCut3; ++j3) {
  //                 choices_[iter].vIdx[0] = i1; 
  //                 choices_[iter].vIdx[1] = i2; 
  //                 choices_[iter].vIdx[2] = i3; 
  //                 choices_[iter].cIdx[0] = j1; 
  //                 choices_[iter].cIdx[1] = j2; 
  //                 choices_[iter].cIdx[2] = j3; 
  //                 iter++; 
  //               }
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }
              
  // // This function is the worker function for the search
  // void worker(size_t tid) {
  //   size_t first = 0, last = 0;
  //   setRange(tid, first, last); 
  //   auto nSample = data_->nSample();
  //   size_t stride = 1u << depth;

  //   for (size_t i = first; i < last; ++i) {
  //     double v[1u << (depth + 1)] = {0.0};
  //     double *ans = scores_.data() + i * stride;

  //     const short *m[depth] = {nullptr};
  //     for (size_t d = 0; d < depth; ++d)
  //       m[d] = data_->cutMask(choices_[i].vIdx[d],
  //                             choices_[i].cIdx[d]).data(); 
      
  //     for (size_t j = 0; j < nSample; ++j) {
  //       size_t idx = data_->act(j);
  //       for (size_t d = 0; d < depth; ++d)
  //         idx += m[d][j] << (depth - d);

  //       v[idx] += data_->resp(j);
  //     }

  //     for (size_t j = 0; j < stride; ++j)
  //       ans[j] = v[2 * j + 1] - v[2 * j];
  //   }
  // }

  // // This function is a helper function that determines the range of searches
  // // worker tid needs to perform
  // void setRange(size_t tid, size_t &first, size_t &last) const {
  //   auto nChoice = choices_.size();     
  //   auto choicePerThread = nChoice / nThreads_;
  //   auto remainder = nChoice % nThreads_;

  //   if (tid < remainder) {
  //     first = (choicePerThread + 1) * tid;
  //     last = first + choicePerThread + 1;
  //   } else {
  //     first = choicePerThread * tid + remainder;
  //     last = first + choicePerThread;
  //   }
  // }

