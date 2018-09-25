#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <thread>
#include <numeric> 
#include "SearchEngine.h"

namespace ITR {

SearchEngine::SearchEngine(unsigned depth, const Data *data) {
  if (depth != 1 && depth != 2 && depth != 3)
    throw "Invalid search depth!"; 

  depth_ = depth;
  data_ = data;

  // Compute the total number of searches to examine
  auto nVar = data_->nVar();
  std::vector<size_t> nCutVec(nVar);
  totalChoices_ = 0;
  for(size_t i=0; i < nVar; ++i){
      nCutVec[i]=data_->nCut(i);
  }
  countChoices(std::vector<size_t>{}, nCutVec, depth_);
  choices_.resize(totalChoices_);
  results_.resize(totalChoices_<< depth_);

  // Set all the search choices
  setSearchChoices(); 
}


void SearchEngine::countChoices(std::vector<size_t> sofar, std::vector<size_t> rest, size_t restDepth){
    if(restDepth==0){
        totalChoices_+=std::accumulate(sofar.begin(),sofar.end(),1, std::multiplies<>());
    } else if(rest.size()<restDepth){
        return;
    } else {
        auto sofar1=sofar;
        sofar1.insert(sofar1.end(),rest.begin(),rest.begin()+1);
        rest.erase(rest.begin());
        countChoices(sofar, rest, restDepth);
        countChoices(sofar1,rest,restDepth-1);
    }
}



void SearchEngine::setSearchChoices() {
  auto nVar = data_->nVar();
  size_t iter = 0; 

  if (depth_ == 1) {
    for (size_t i = 0; i < nVar; ++i) {
      auto nCut = data_->nCut(i);
      for (size_t j = 0; j < nCut; ++j) {
        choices_[iter].vIdx.push_back(i);
        choices_[iter].cIdx.push_back(j); 
        iter++; 
      }
    }
  } else if (depth_ == 2) {
    for (size_t i1 = 0; i1 < nVar; ++i1) {
      auto nCut1 = data_->nCut(i1);
      for (size_t i2 = i1 + 1; i2 < nVar; ++i2) {
        auto nCut2 = data_->nCut(i2);
        for (size_t j1 = 0; j1 < nCut1; ++j1) {
          for (size_t j2 = 0; j2 < nCut2; ++j2) {
            choices_[iter].vIdx.push_back(i1);
            choices_[iter].vIdx.push_back(i2);
            choices_[iter].cIdx.push_back(j1);
            choices_[iter].cIdx.push_back(j2);
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
                choices_[iter].vIdx.push_back(i1);
                choices_[iter].vIdx.push_back(i2);
                choices_[iter].vIdx.push_back(i3);
                choices_[iter].cIdx.push_back(j1);
                choices_[iter].cIdx.push_back(j2);
                choices_[iter].cIdx.push_back(j3);
                iter++; 
              }
            }
          }
        }
      }
    }
  }               
}

void SearchEngine::run(unsigned nThreads) {  
  // TODO: Reponse is currently used only as a column vector, EXTENSION NEEDED

  // Get number of cores
  unsigned nCores = std::thread::hardware_concurrency();

  if (nThreads > nCores)
    nThreads = nCores;   
  std::vector<std::thread> threads(nThreads);

  using namespace std::chrono;   
  auto t1 = high_resolution_clock::now();

  for (size_t i = 0; i < nThreads; ++i)
    threads[i] = std::thread(&SearchEngine::worker, this, i, nThreads);

  for (auto &th : threads)
    th.join();

  auto t2 = high_resolution_clock::now();
  auto elapsed = duration_cast<duration<double>>(t2 - t1); 

  std::cout << "Completed in " << std::scientific
            << elapsed.count() << " seconds using "
            << nThreads << " threads\n"; 
}

void SearchEngine::setRange(size_t tid, unsigned nThreads,
                            size_t &first, size_t &last) {
  auto nChoice = choices_.size();     
  auto choicePerThread = nChoice / nThreads;
  auto remainder = nChoice % nThreads;

  if (tid < remainder) {
    first = (choicePerThread + 1) * tid;
    last = first + choicePerThread + 1;
  } else {
    first = choicePerThread * tid + remainder;
    last = first + choicePerThread;
  }
}

void SearchEngine::worker(size_t tid, unsigned nThreads) {
  size_t first = 0, last = 0;
  setRange(tid, nThreads, first, last); 
  auto nSample = data_->nSample(); 
  size_t stride = 1u << depth_;
  
  if (depth_ == 1) {
    for (size_t i = first; i < last; ++i) {
      double v[4] = {0.0};
      double *ans = results_.data() + i * stride;
      const auto & m = data_->cutMask(choices_[i].vIdx[0], choices_[i].cIdx[0]);

      for (size_t j = 0; j < nSample; ++j)
        v[2 * m[j] + data_->act(j)] += data_->resp(j);

      ans[0] = v[1] - v[0];
      ans[1] = v[3] - v[2]; 
    }
  } else if (depth_ == 2) {
    for (size_t i = first; i < last; ++i) {
      double v[8] = {0.0};
      double *ans = results_.data() + i * stride;
      const auto & m1 = data_->cutMask(choices_[i].vIdx[0], choices_[i].cIdx[0]);
      const auto & m2 = data_->cutMask(choices_[i].vIdx[1], choices_[i].cIdx[1]);

      for (size_t j = 0; j < nSample; ++j)
        v[4 * m1[j] + 2 * m2[j] + data_->act(j)] += data_->resp(j);

      ans[0] = v[1] - v[0];
      ans[1] = v[3] - v[2];
      ans[2] = v[5] - v[4];
      ans[3] = v[7] - v[6]; 
    }  
  } else { // depth_ == 3
    for (size_t i = first; i < last; ++i) {
      double v[16] = {0.0};
      double *ans = results_.data() + i * stride;
      const auto & m1 = data_->cutMask(choices_[i].vIdx[0], choices_[i].cIdx[0]);
      const auto & m2 = data_->cutMask(choices_[i].vIdx[1], choices_[i].cIdx[1]);
      const auto & m3 = data_->cutMask(choices_[i].vIdx[2], choices_[i].cIdx[2]);

      for (size_t j = 0; j < nSample; ++j)
        v[8 * m1[j] + 4 * m2[j] + 2 * m3[j] + data_->act(j)] += data_->resp(j); 

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

void SearchEngine::report(size_t nTop) {
  // Cap nTop
  if (nTop > choices_.size())
    nTop = choices_.size(); 

  double T0 = data_->T0();
  double scale = 1.0 / data_->nSample(); 

  // Sort the search results in decending order. 
  std::vector<size_t> index(results_.size());
  std::iota(index.begin(), index.end(), 0);
  std::partial_sort(index.begin(), index.begin()+nTop, index.end(),
            [&](size_t i1, size_t i2) {return results_[i1] > results_[i2];});

  
  for (size_t i = 0; i < nTop; ++i) {
    // Each variable and cut combination corresponds to 2^depth searches
    size_t sID = index[i];              // search ID
    size_t cID = sID / (1 << depth_);   // choice ID
    size_t mask = sID % (1 << depth_);  // mask for cut direction
    
    std::cout << "Value = " << std::scientific
              << (T0 + results_[index[i]]) * scale << "\n"; 
    for (auto d = 0u; d < depth_; ++d) {
      data_->cutInfo(choices_[cID].vIdx[d], choices_[cID].cIdx[d],
                     mask & (1u << (depth_ - 1u - d)));
    }
  }
}
  

} // namespace ITR
