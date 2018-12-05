#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <iostream>
#include "SearchEngine.h"

SearchEngine::SearchEngine(const Data *data, unsigned depth,
                           unsigned nThreads)
  : data_{data}, depth_{depth}
{
  if (depth != 1 && depth != 2 && depth != 3)
    throw "Invalid search depth!";
  
  nThreads_ = std::min(nThreads, std::thread::hardware_concurrency());
  
  // Compute the total number of searches to examine
  std::vector<size_t> vIdx(data_->nVar());
  std::iota(vIdx.begin(), vIdx.end(), 0);
  combination(std::vector<size_t>{}, vIdx, 'c');
  
  // Resize the buffer
  choices_.resize(totalChoices_);
  scores_.resize(totalChoices_ << depth_);
  
  // Set all the search choices
  iter_ = 0; 
  combination(std::vector<size_t>{}, vIdx, 's');

  // None of the search scores have been sorted
  nTop_ = 0;
  index_.resize(scores_.size()); 
}

void SearchEngine::run() {
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

void SearchEngine::reportHelper(size_t &nTop) {
  // Cap nTop
  nTop = std::min(nTop, scores_.size() - 1);

  if (nTop > nTop_) {
    // We need to sort again
    nTop_ = nTop;
    std::iota(index_.begin(), index_.end(), 0);
    std::partial_sort(index_.begin(), index_.begin() + nTop_, index_.end(),
                      [&](size_t i1, size_t i2) {
                        return scores_[i1] > scores_[i2];
                      });
  }  
}

rVector SearchEngine::score(size_t nTop) {
  reportHelper(nTop);

  rVector retval(nTop); 
  double T0 = data_->T0();
  double scale = 1.0 / data_->nSample();
  for (size_t i = 0; i < nTop; ++i) {
    size_t sID = index_[i]; // search ID
    retval[i] = (T0 + scores_[sID]) * scale;
  }

  return retval;
}
    
uMatrix SearchEngine::var(size_t nTop) {
  reportHelper(nTop);
     
  uMatrix retval(nTop, depth_);  
  for (size_t i = 0; i < nTop; ++i) {
    size_t sID = index_[i];     // search ID
    size_t cID = sID >> depth_; // choice ID
    for (size_t d = 0; d < depth_; ++d)
      retval(i, d) = choices_[cID].vIdx[d];
  }
  
  return retval; 
}

sVector SearchEngine::cut(size_t i) {
  reportHelper(i);

#ifdef USE_RCPP   
  // i is passed through R where array index starts from 1. We need to use
  // (i - 1) to access the value
  i--; 
#endif   
  size_t sID = index_[i]; //index_[i - 1];     // search ID
  size_t cID = sID >> depth_; // choice ID
  sVector out(depth_);
  for (size_t d = 0; d < depth_; ++d)
    out[d] = data_->cutVal(choices_[cID].vIdx[d], choices_[cID].cIdx[d]);

  return out;
}
  
sVector SearchEngine::dir(size_t i) {
  reportHelper(i);

#ifdef USE_RCPP
  // i is passed through R where array index starts from 1. We need to use
  // (i - 1) to access the value 
  i--; 
#endif
  size_t sID = index_[i]; // search ID
  size_t cID = sID >> depth_; // choice ID  
  size_t mask = sID % (1 << depth_); 
  sVector out(depth_);

  for (size_t d = 0; d < depth_; ++d) 
    out[d] = data_->cutDir(choices_[cID].vIdx[d],
                           mask & (1u << (depth_ - 1u - d)));

  return out;
}
  
void SearchEngine::combination(std::vector<size_t> curr,
                               std::vector<size_t> option, char mode) {
  // Get the number of variables to choose
  size_t nchoices = depth_ - curr.size();
  if (nchoices == 0) {
    // Need to choose no more
    if (mode == 'c') {
      countChoices(curr);
    } else {
      setChoices(curr); 
    }
  } else if (option.size() < nchoices) {
    // Does not have enough to choose
    return;
  } else {
    auto curr1 = curr;
    curr1.insert(curr1.end(), option.begin(), option.begin() + 1);
    option.erase(option.begin());
    
    // Choose 'nchoices' from options
    combination(curr, option, mode);
    // Choose 'nchoices - 1' from options
    combination(curr1, option, mode);
  }
}

void SearchEngine::countChoices(const std::vector<size_t> &choices) {
  size_t nchoices = 1;
  for (const auto &vidx : choices)
    nchoices *= data_->nCut(vidx);
  totalChoices_ += nchoices;
}

void SearchEngine::setChoices(const std::vector<size_t> &choices) {
  std::vector<size_t> cIdx;
  std::vector<size_t> cBound;
  for (const auto &vidx : choices) {
    cIdx.push_back(0);
    cBound.push_back(data_->nCut(vidx));
  }
  
  setChoicesHelper(choices, cIdx, cBound, 0); 
}

void SearchEngine::setChoicesHelper(const std::vector<size_t> &vIdx,
                                    std::vector<size_t> cIdx,
                                    std::vector<size_t> &cBound,
                                    size_t curr) {
  if (cIdx[curr] == cBound[curr]) {
    return;
  } else {
    if (curr < depth_ - 1) {
      setChoicesHelper(vIdx, cIdx, cBound, curr + 1); 
    } else {
      for (size_t d = 0; d < depth_; ++d) {
        choices_[iter_].vIdx[d] = vIdx[d];
        choices_[iter_].cIdx[d] = cIdx[d];
      }
      iter_++;
    }
    
    cIdx[curr]++;
    setChoicesHelper(vIdx, cIdx, cBound, curr); 
  }
}

void SearchEngine::worker(size_t tid) {
  // Compute the range of searches the worker needs to perform
  size_t first{0}, last{0};
  auto nChoice = choices_.size();
  auto choicePerWorker = nChoice / nThreads_;
  auto choiceRemainder = nChoice % nThreads_;
  
  if (tid < choiceRemainder) {
    first = (choicePerWorker + 1) * tid;
    last = first + choicePerWorker + 1;
  } else {
    first = choicePerWorker * tid + choiceRemainder;
    last = first + choicePerWorker;
  }
  
  auto nSample = data_->nSample();
  size_t stride = 1u << depth_;
  
  for (size_t i = first; i < last; ++i) {
    std::vector<double> v(1u << (depth_ + 1), 0.0);
    double *ans = scores_.data() + i * stride;
    std::vector<const std::uint32_t *> m(depth_);

    for (size_t d = 0; d < depth_; ++d)
      m[d] = data_->cutMask(choices_[i].vIdx[d],
                            choices_[i].cIdx[d]).data();
    
    size_t remainder = nSample % 8;
    size_t nBatches = nSample >> 3;

    // We sort data into different buckets based on the values of resposne and
    // cut masks. For example, there are 16 buckets if depth is 3:
    // (0000)_2 (0001)_2 (0010)_2 (0011)_2
    // (0100)_2 (0101)_2 (0110)_2 (0111)_2
    // (1000)_2 (1001)_2 (1010)_2 (1011)_2
    // (1100)_2 (1101)_2 (1110)_2 (1111)_2 
    for (size_t j = 0; j < nBatches; ++j) {
      size_t idx = data_->act(j);
      for (size_t d = 0; d < depth_; ++d)
        idx += m[d][j] << (depth_ - d);
      
      size_t j8 = j << 3;
      for (int k = 7; k >= 0; --k) {
        // Read the bottom 4 bits
        v[idx & 0xF] += data_->resp(j8 + k);
        // Drop the bottom 4 bits
        idx >>= 4;
      }
    }

    if (remainder) {
      size_t idx = data_->act(nBatches);
      for (size_t d = 0; d < depth_; ++d)
        idx += m[d][nBatches] << (depth_ - d);
      
      // Drop the bottom bits that are all zero
      idx >>= (32 - 4 * remainder);
      
      size_t j8 = nBatches << 3;
      for (int k = static_cast<int>(remainder) - 1; k >= 0; --k) {
        v[idx & 0xF] += data_->resp(j8 + k);
        idx >>= 4;
      }
    }
    
    for (size_t j = 0; j < stride; ++j)
      ans[j] = v[2 * j + 1] - v[2 * j];
  }
}
