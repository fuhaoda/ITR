#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <thread>
#include <numeric> 
#include "Data.h"

namespace ITR {

struct Treatment {
  double score; 
  std::string rule;
}; 

template<unsigned depth>
class SearchEngine {
public:
  // Constructor 
  SearchEngine(const Data *data, unsigned nThreads) {
    if (depth != 1 && depth != 2 && depth != 3)
      throw "Invalid search depth!";

    data_ = data; 
    nThreads_ = nThreads;

    // Compute the total number of searches to examine
    auto nVar = data_->nVar();
    std::vector<size_t> nCutVec(nVar);
    for (size_t i = 0; i < nVar; ++i)
      nCutVec[i] = data_->nCut(i);

    countChoices(std::vector<size_t> {}, nCutVec, depth);

    choices_.resize(totalChoices_);
    scores_.resize(totalChoices_ << depth);

    // Set all the search choices    
    setSearchChoices(); 
  }

  // Return the number of search choices to examine
  size_t nChoices() const { return choices_.size() << depth; }

  // Run the comprehensive search
  void run() {
    // Get number of cores
    unsigned nCores = std::thread::hardware_concurrency();

    if (nThreads_ > nCores)
      nThreads_ = nCores;   
    std::vector<std::thread> threads(nThreads_);

    using namespace std::chrono;   
    auto t1 = high_resolution_clock::now();

    for (size_t i = 0; i < nThreads_; ++i)
      threads[i] = std::thread(&SearchEngine::worker, this, i);

    for (auto &th : threads)
      th.join();

    auto t2 = high_resolution_clock::now();
    auto elapsed = duration_cast<duration<double>>(t2 - t1); 
    
    std::cout << "Completed in " << std::scientific
              << elapsed.count() << " seconds using "
              << nThreads_ << " threads\n"; 
  }

  // Report the best results
  std::vector<Treatment> report(size_t nTop) const {
    // Cap nTop
    if (nTop > choices_.size())
      nTop = choices_.size();

    double T0 = data_->T0();
    double scale = 1.0 / data_->nSample();

    // Sort the scores in descending order
    std::vector<size_t> index(scores_.size());
    std::iota(index.begin(), index.end(), 0);
    std::partial_sort(index.begin(), index.begin() + nTop, index.end(),
                      [&](size_t i1, size_t i2) {
                        return scores_[i1] > scores_[i2];
                      });

    std::vector<Treatment> retval(nTop);
    for (size_t i = 0; i < nTop; ++i) {
      // Each variable and cut combination corresponds to 2^depth searches
      size_t sID = index[i];            // search ID
      size_t cID = sID >> depth;        // choice ID
      size_t mask = sID % (1 << depth); // mask for cut direction 

      retval[i].score = (T0 + scores_[sID]) * scale;
      for (size_t d = 0; d < depth; ++d)
        retval[i].rule += data_->cutInfo(choices_[cID].vIdx[d],
                                         choices_[cID].cIdx[d],
                                         mask & (1u << (depth - 1u - d)));

      retval[i].rule.pop_back();
      retval[i].rule.pop_back(); 
    }
    
    return retval; 
  }


private:
  // Data associated with the search
  const Data *data_;

  // Number of threads used in the search
  unsigned nThreads_; 

  size_t totalChoices_ = 0;

  struct Meta {
    // Choices of vriables used in the search 
    size_t vIdx[depth];
    // Choices of cuts used in the search 
    size_t cIdx[depth];
  };

  // Choices of all the variables and cuts combination used in the search
  std::vector<Meta> choices_;

  // Values of the search scores_
  std::vector<double> scores_;

  // This function computes the number of search choices formed by the
  // combination of variables i1, ..., id, where 0 <= i1 < i2 < ... < id < max
  void countChoices(std::vector<size_t> sofar,
                    std::vector<size_t> rest,
                    size_t restDepth) {
    if (restDepth == 0) {
      totalChoices_ += std::accumulate(sofar.begin(), sofar.end(), 1,
                                       std::multiplies<>());
    } else if (rest.size() < restDepth) {
      return;
    } else {
      auto sofar1 = sofar;
      sofar1.insert(sofar1.end(), rest.begin(), rest.begin() + 1);
      rest.erase(rest.begin());
      countChoices(sofar, rest, restDepth);
      countChoices(sofar1, rest, restDepth - 1);
    }
  }


  // This function sets all the search choices
  void setSearchChoices() {
    auto nVar = data_->nVar();
    size_t iter = 0;

    if (depth == 1) {
      for (size_t i = 0; i < nVar; ++i) {
        auto nCut = data_->nCut(i);
        for (size_t j = 0; j < nCut; ++j) {
          choices_[iter].vIdx[0] = i;
          choices_[iter].cIdx[0] = j;
          iter++;
        }
      }
    } else if (depth == 2) {
      for (size_t i1 = 0; i1 < nVar; ++i1) {
        auto nCut1 = data_->nCut(i1);
        for (size_t i2 = i1 + 1; i2 < nVar; ++i2) {
          auto nCut2 = data_->nCut(i2);
          for (size_t j1 = 0; j1 < nCut1; ++j1) {
            for (size_t j2 = 0; j2 < nCut2; ++j2) {
              choices_[iter].vIdx[0] = i1;
              choices_[iter].vIdx[1] = i2;
              choices_[iter].cIdx[0] = j1;
              choices_[iter].cIdx[1] = j2;
              iter++;
            }
          }
        }
      }
    } else {
      for (size_t i1 = 0; i1 < nVar; ++i1) {
        auto nCut1 = data_->nCut(i1);
        for (size_t i2 = i1 + 1; i2 < nVar; ++i2) {
          auto nCut2 = data_->nCut(i2); 
          for (size_t i3 = i2 + 1; i3 < nVar; ++i3) {
            auto nCut3 = data_->nCut(i3); 
            for (size_t j1 = 0; j1 < nCut1; ++j1) {
              for (size_t j2 = 0; j2 < nCut2; ++j2) {
                for (size_t j3 = 0; j3 < nCut3; ++j3) {
                  choices_[iter].vIdx[0] = i1; 
                  choices_[iter].vIdx[1] = i2; 
                  choices_[iter].vIdx[2] = i3; 
                  choices_[iter].cIdx[0] = j1; 
                  choices_[iter].cIdx[1] = j2; 
                  choices_[iter].cIdx[2] = j3; 
                  iter++; 
                }
              }
            }
          }
        }
      }
    }
  }
              
  // This function is the worker function for the search
  void worker(size_t tid) {
    size_t first = 0, last = 0;
    setRange(tid, first, last); 
    auto nSample = data_->nSample();
    size_t stride = 1u << depth;
    
    if (depth == 1) {
      for (size_t i = first; i < last; ++i) {
        double v[4] = {0.0};
        double *ans = scores_.data() + i * stride;
        const auto &m = data_->cutMask(choices_[i].vIdx[0],
                                       choices_[i].cIdx[0]);

        for (size_t j = 0; j < nSample; ++j)
          v[2 * m[j] + data_->act(j)] += data_->resp(j);

        ans[0] = v[1] - v[0];
        ans[1] = v[3] - v[2]; 
      }
    } else if (depth == 2) {
      for (size_t i = first; i < last; ++i) {
        double v[8] = {0.0};
        double *ans = scores_.data() + i * stride;
        const auto &m1 = data_->cutMask(choices_[i].vIdx[0],
                                        choices_[i].cIdx[0]);
        const auto &m2 = data_->cutMask(choices_[i].vIdx[1],
                                        choices_[i].cIdx[1]);

        for (size_t j = 0; j < nSample; ++j)
          v[4 * m1[j] + 2 * m2[j] + data_->act(j)] += data_->resp(j);

        ans[0] = v[1] - v[0];
        ans[1] = v[3] - v[2];
        ans[2] = v[5] - v[4];
        ans[3] = v[7] - v[6]; 
      }  
    } else { // depth == 3
      for (size_t i = first; i < last; ++i) {
        double v[16] = {0.0};
        double *ans = scores_.data() + i * stride;
        const auto &m1 = data_->cutMask(choices_[i].vIdx[0],
                                        choices_[i].cIdx[0]);
        const auto &m2 = data_->cutMask(choices_[i].vIdx[1],
                                        choices_[i].cIdx[1]);
        const auto &m3 = data_->cutMask(choices_[i].vIdx[2],
                                        choices_[i].cIdx[2]);

        const auto &m11 = data_->cutMask1(choices_[i].vIdx[0],
                                          choices_[i].cIdx[0]);
        const auto &m21 = data_->cutMask1(choices_[i].vIdx[1],
                                          choices_[i].cIdx[1]);
        const auto &m31 = data_->cutMask1(choices_[i].vIdx[2],
                                          choices_[i].cIdx[2]);
        
        
        
        for (size_t j = 0; j < nSample; ++j) {
          //v[8 * m1[j] + 4 * m2[j] + 2 * m3[j] + data_->act(j)] += data_->resp(j);

          v[8 * m11[j] + 4 * m21[j] + 2 * m31[j] + data_->act(j)] += data_->resp(j);
        }
        
        ans[0] = v[1] - v[0];
        ans[1] = v[3] - v[2];
        ans[2] = v[5] - v[4];
        ans[3] = v[7] - v[6];
        ans[4] = v[9] - v[8];
        ans[5] = v[11] - v[10];
        ans[6] = v[13] - v[12];
        ans[7] = v[15] - v[14]; 
      }  
    }        
  }

  // This function is a helper function that determines the range of searches
  // worker tid needs to perform
  void setRange(size_t tid, size_t &first, size_t &last) const {
    auto nChoice = choices_.size();     
    auto choicePerThread = nChoice / nThreads_;
    auto remainder = nChoice % nThreads_;

    if (tid < remainder) {
      first = (choicePerThread + 1) * tid;
      last = first + choicePerThread + 1;
    } else {
      first = choicePerThread * tid + remainder;
      last = first + choicePerThread;
    }
  }
}; 


} // namespace ITR

#endif 
