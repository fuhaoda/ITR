#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <thread> 
#include "SearchEngine.h"

namespace ITR {

SearchEngine::SearchEngine(unsigned depth, const Data *data) {
  if (depth != 1 && depth != 2 && depth != 3)
    throw "Invalid search depth!"; 

  depth_ = depth;
  data_ = data;

  // Set all the possible search choices
  setSearchChoices(); 
}

void SearchEngine::setSearchChoices() {
//Todo: potentially, we can make this as a recursive call to use depth as a parameter.
  auto nVar = data_->nVar();

  if (depth_ == 1) {
    for (size_t i = 0; i < nVar; ++i) {
      auto nCut = data_->nCut(i);
      for (size_t j = 0; j < nCut; ++j) {
        Meta record{};
        record.vIdx.push_back(i);
        record.cIdx.push_back(j);
        log_.push_back(std::move(record));
      }
    }
  } else if (depth_ == 2) {
    for (size_t i1 = 0; i1 < nVar; ++i1) {
      auto nCut1 = data_->nCut(i1); 
      for (size_t i2 = i1 + 1; i2 < nVar; ++i2) {
        auto nCut2 = data_->nCut(i2);
        for (size_t j1 = 0; j1 < nCut1; ++j1) {
          for (size_t j2 = 0; j2 < nCut2; ++j2) {
            Meta record{};
            record.vIdx.push_back(i1);
            record.vIdx.push_back(i2);
            record.cIdx.push_back(j1);
            record.cIdx.push_back(j2);
            log_.push_back(std::move(record));
          }
        }
      }
    }  
  } else { // depth == 3
    for (size_t i1 = 0; i1 < nVar; ++i1) {
      auto nCut1 = data_->nCut(i1); 
      for (size_t i2 = i1 + 1; i2 < nVar; ++i2) {
        auto nCut2 = data_->nCut(i2); 
        for (size_t i3 = i2 + 1; i3 < nVar; ++i3) {
          auto nCut3 = data_->nCut(i3);
          for (size_t j1 = 0; j1 < nCut1; ++j1) {
            for (size_t j2 = 0; j2 < nCut2; ++j2) {
              for (size_t j3 = 0; j3 < nCut3; ++j3) {
                Meta record{};
                record.vIdx.push_back(i1);
                record.vIdx.push_back(i2);
                record.vIdx.push_back(i3);
                record.cIdx.push_back(j1);
                record.cIdx.push_back(j2);
                record.cIdx.push_back(j3);
                log_.push_back(std::move(record));
              }
            }
          }
        }
      }
    }
  }
}

void SearchEngine::run(unsigned nThreads) {  
  // TODO: EXTENSION NEEDED
  // The origianl code assumes that action and response are column vectors. Only
  // action[i][0] and response[i][0] are used for each row.

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
  auto nChoice = log_.size();
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
  auto T0 = data_->T0();
  auto scaling_factor = 2.0 / nSample;

  if (depth_ == 1) {
    for (size_t i = first; i < last; ++i) {
      double v[4] = {0.0};
      const auto mask = data_->cutMask(log_[i].vIdx[0], log_[i].cIdx[0]);

      for (size_t j = 0; j < nSample; ++j) 
        v[2 * mask[j] + data_->act(j)] += data_->resp(j, 0);
      

      v[0] = v[1] - v[0];
      v[1] = v[3] - v[2];
      
      if (v[0] < v[1]) {
        log_[i].result = (v[0] + T0) * scaling_factor;
        log_[i].rank = 0;
      } else {
        log_[i].result = (v[1] + T0) * scaling_factor;
        log_[i].rank = 1;
      }
    }
  } else if (depth_ == 2) {
    for (size_t i = first; i < last; ++i) {
      double v[8] = {0.0};
      const auto mask1 = data_->cutMask(log_[i].vIdx[0], log_[i].cIdx[0]);
      const auto mask2 = data_->cutMask(log_[i].vIdx[1], log_[i].cIdx[1]);
      
      for (size_t j = 0; j < nSample; ++j) 
        v[4 * mask1[j] + 2 * mask2[j] + data_->act(j)] += data_->resp(j, 0);

      v[0] = v[1] - v[0];
      v[1] = v[3] - v[2];
      v[2] = v[5] - v[4];
      v[3] = v[7] - v[6];
      
      const auto ptr = std::max_element(v, v + 4);
      log_[i].result = (*ptr + T0) * scaling_factor;
      log_[i].rank = ptr - v; 
    }  
  } else { // depth_ == 3
    for (size_t i = first; i < last; ++i) {
      double v[16] = {0.0};
      const auto mask1 = data_->cutMask(log_[i].vIdx[0], log_[i].cIdx[0]);
      const auto mask2 = data_->cutMask(log_[i].vIdx[1], log_[i].cIdx[1]);
      const auto mask3 = data_->cutMask(log_[i].vIdx[2], log_[i].cIdx[2]); 
      
      for (size_t j = 0; j < nSample; ++j) 
        v[8 * mask1[j] + 4 * mask2[j] + 2 * mask3[j] + data_->act(j)] +=
          data_->resp(j, 0);

      v[0] = v[1] - v[0];
      v[1] = v[3] - v[2];
      v[2] = v[5] - v[4];
      v[3] = v[7] - v[6];
      v[4] = v[9] - v[8];
      v[5] = v[11] - v[10];
      v[6] = v[13] - v[12];
      v[7] = v[15] - v[14];
      
      const auto ptr = std::max_element(v, v + 8);
      log_[i].result = (*ptr + T0) * scaling_factor;
      log_[i].rank = ptr - v; 
    }  
  }
}


void SearchEngine::report(size_t nTop) {
  // Cap nTop 
  if (nTop > log_.size())
    nTop = log_.size();

  // Sort the logs based on comparing values in result
  std::sort(log_.begin(), log_.end(),
            [](Meta &d1, Meta &d2) {return d1.result > d2.result;});

  for (size_t i = 0; i < nTop; ++i) {
    std::cout << "Value is " << std::scientific << log_[i].result
              << ", obtained from\n";
    for (auto d = 0; d < depth_; ++d) {
      data_->cutInfo(log_[i].vIdx[d], log_[i].cIdx[d],
                     log_[i].rank & (1 << (depth_ - 1 - d)));
    }
  }
}
  

} // namespace ITR
