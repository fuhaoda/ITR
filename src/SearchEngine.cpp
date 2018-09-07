#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include "SearchEngine.h"

namespace ITR {

SearchEngine::SearchEngine(int depth, const Data *data) {
  if (depth != 1 && depth != 2 && depth != 3)
    throw "Invalid search depth!"; 

  depth_ = depth;
  data_ = data;

  // Go over the data and find all the possible search choices
  if (depth == 1) {
    setDepthOneChoices();
  } else if (depth == 2) {
    setDepthTwoChoices();
  } else { // depth == 3
    setDepthThreeChoices();
  }
}

void SearchEngine::run() {
  // TODO: EXTENSION NEEDED
  // The origianl code assumes that action and response are column vectors. Only
  // action[i][0] and response[i][0] are used for each row. 
  auto nChoice = log_.size();   
  auto nSample = data_->nSample();
  auto T0 = data_->T0();
  auto scaling_factor = 2.0 / nSample; 

  if (depth_ == 1) {
    for (auto i = 0; i < nChoice; ++i) {
      double v[4] = {0.0};
      auto vIdx = log_[i].vIdx[0];
      auto cIdx = log_[i].cIdx[0]; 
      for (auto j = 0; j < nSample; ++j) {
        auto m = data_->inCut(j, vIdx, cIdx);
        auto act = data_->act(j, 0);
        auto resp = data_->resp(j, 0);
        v[2 * m + act] += resp;
      }

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
    for (auto i = 0; i < nChoice; ++i) {
      double v[8] = {0.0};
      auto vIdx1 = log_[i].vIdx[0];
      auto vIdx2 = log_[i].vIdx[1];
      auto cIdx1 = log_[i].cIdx[0];
      auto cIdx2 = log_[i].cIdx[1]; 
      for (auto j = 0; j < nSample; ++j) {
        auto m1 = data_->inCut(j, vIdx1, cIdx1);
        auto m2 = data_->inCut(j, vIdx2, cIdx2);
        auto act = data_->act(j, 0);
        auto resp = data_->resp(j, 0);
        v[4 * m1 + 2 * m2 + act] += resp;
      }

      v[0] = v[1] - v[0];
      v[1] = v[3] - v[2];
      v[2] = v[5] - v[4];
      v[3] = v[7] - v[6];

      const auto ptr = std::max_element(v, v + 4);
      log_[i].result = (*ptr + T0) * scaling_factor;
      log_[i].rank = ptr - v; 
    }
  } else { // depth_ == 3
    for (auto i = 0; i < nChoice; ++i) {
      double v[16] = {0.0};
      auto vIdx1 = log_[i].vIdx[0];
      auto vIdx2 = log_[i].vIdx[1];
      auto vIdx3 = log_[i].vIdx[2];
      auto cIdx1 = log_[i].cIdx[0];
      auto cIdx2 = log_[i].cIdx[1];
      auto cIdx3 = log_[i].cIdx[2];
      for (auto j = 0; j < nSample; ++j) {
        auto m1 = data_->inCut(j, vIdx1, cIdx1);
        auto m2 = data_->inCut(j, vIdx2, cIdx2);
        auto m3 = data_->inCut(j, vIdx3, cIdx3);
        auto act = data_->act(j, 0);
        auto resp = data_->resp(j, 0);
        v[8 * m1 + 4 * m2 + 2 * m3 + act] += resp;
      }

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

void SearchEngine::report(int nTop) {
  assert(nTop <= log_.size()); 

  // Sort the logs based on comparing values in result
  std::sort(log_.begin(), log_.end(),
            [](MetaData &d1, MetaData &d2) {return d1.result > d2.result;});

  int iter = 0; 
  for (auto it = log_.begin(); it != log_.end(); ++it) {
    std::cout << "Value is " << std::scientific << it->result
              << ", obtained from\n";
    for (auto d = 0; d < depth_; ++d) {
      auto vIdx = it->vIdx[d];
      auto cIdx = it->cIdx[d];
      auto m = it->rank & (1 << (depth_ - 1 - d));
      data_->cutInfo(vIdx, cIdx, m); 
    }

    if (iter++ >= nTop)
      break;
  }
}


void SearchEngine::setDepthOneChoices() {
  auto nCont = data_->nCont();
  auto nOrd = data_->nOrd();
  auto nVar = data_->nVar();
  for (auto j = 0; j < nCont + nOrd; ++j) {
    for (auto k = 0; k < data_->nCut(j); ++k) {
      MetaData record; 
      record.vIdx.push_back(j);
      record.cIdx.push_back(k);
      log_.push_back(std::move(record)); 
    }
  }

  for (auto j = nCont + nOrd; j < nVar; ++j) {
    auto max = 1 << data_->nCut(j);
    auto half = data_->nCut(j) / 2;
    for (auto k = 0; k < max; ++k) {
      std::bitset<64> subset(k);
      if (subset.count() <= half) {
        MetaData record;
        record.vIdx.push_back(j);
        record.cIdx.push_back(k);
        log_.push_back(std::move(record));       
      }
    }
  }            
}

void SearchEngine::setDepthTwoChoices() {
  auto nCont = data_->nCont();
  auto nOrd = data_->nOrd();
  auto nVar = data_->nVar();

  for (auto j1 = 0; j1 < nCont + nOrd; ++j1) {
    for (auto j2 = j1 + 1; j2 < nCont + nOrd; ++j2) {
      for (auto k1 = 0; k1 < data_->nCut(j1); ++k1) {
        for (auto k2 = 0; k2 < data_->nCut(j2); ++k2) {
          MetaData record;
          record.vIdx.push_back(j1);
          record.vIdx.push_back(j2);
          record.cIdx.push_back(k1);
          record.cIdx.push_back(k2);
          log_.push_back(std::move(record));           
        }
      }
    }

    for (auto j2 = nCont + nOrd; j2 < nVar; ++j2) {
      auto max2 = 1 << data_->nCut(j2);
      auto half2 = data_->nCut(j2) / 2;
      for (auto k2 = 0; k2 < max2; ++k2) {
        std::bitset<64> subset2(k2);
        if (subset2.count() <= half2) {
          for (auto k1 = 0; k1 < data_->nCut(j1); ++k1) {
            MetaData record;
            record.vIdx.push_back(j1);
            record.vIdx.push_back(j2);
            record.cIdx.push_back(k1);
            record.cIdx.push_back(k2);
            log_.push_back(std::move(record));           
          }
        }
      }
    }
  }

  for (auto j1 = nCont + nOrd; j1 < nVar; ++j1) {
    auto max1 = 1 << data_->nCut(j1);
    auto half1 = data_->nCut(j1) / 2;
    for (auto j2 = j1 + 1; j2 < nVar; ++j2) {
      auto max2 = 1 << data_->nCut(j2);
      auto half2 = data_->nCut(j2) / 2;
      for (auto k1 = 0; k1 < max1; ++k1) {
        std::bitset<64> subset1(k1);
        if (subset1.count() <= half1) {
          for (auto k2 = 0; k2 < max2; ++k2) {
            std::bitset<64> subset2(k2);
            if (subset2.count() <= half2) {
              MetaData record;
              record.vIdx.push_back(j1);
              record.vIdx.push_back(j2);
              record.cIdx.push_back(k1);
              record.cIdx.push_back(k2);
              log_.push_back(std::move(record));                         
            }
          }
        }
      }
    }
  }
}

void SearchEngine::setDepthThreeChoices() {
  auto nCont = data_->nCont();
  auto nOrd = data_->nOrd();
  auto nVar = data_->nVar();

  for (auto j1 = 0; j1 < nCont + nOrd; ++j1) {
    for (auto j2 = j1 + 1; j2 < nCont + nOrd; ++j2) {
      for (auto j3 = j2 + 1; j3 < nCont + nOrd; ++j3) {
        for (auto k1 = 0; k1 < data_->nCut(j1); ++k1) {
          for (auto k2 = 0; k2 < data_->nCut(j2); ++k2) {
            for (auto k3 = 0; k3 < data_->nCut(j3); ++k3) {
              MetaData record;
              record.vIdx.push_back(j1);
              record.vIdx.push_back(j2);
              record.vIdx.push_back(j3);
              record.cIdx.push_back(k1);
              record.cIdx.push_back(k2);
              record.cIdx.push_back(k3); 
              log_.push_back(std::move(record));                         
            }
          }
        }
      }

      for (auto j3 = nCont + nOrd; j3 < nVar; ++j3) {
        auto max3 = 1 << data_->nCut(j3);
        auto half3 = data_->nCut(j3) / 2;
        for (auto k3 = 0; k3 < max3; ++k3) {
          std::bitset<64> subset3(k3);
          if (subset3.count() <= half3) {
            for (auto k1 = 0; k1 < data_->nCut(j1); ++k1) {
              for (auto k2 = 0; k2 < data_->nCut(j2); ++k2) {
                MetaData record;
                record.vIdx.push_back(j1);
                record.vIdx.push_back(j2);
                record.vIdx.push_back(j3);
                record.cIdx.push_back(k1);
                record.cIdx.push_back(k2);
                record.cIdx.push_back(k3); 
                log_.push_back(std::move(record));                           
              }
            }
          }
        }
      }
    }

    for (auto j2 = nCont + nOrd; j2 < nVar; ++j2) {
      auto max2 = 1 << data_->nCut(j2);
      auto half2 = data_->nCut(j2) / 2; 
      for (auto j3 = j2 + 1; j3 < nVar; ++j3) {
        auto max3 = 1 << data_->nCut(j3);
        auto half3 = data_->nCut(j3) / 2;
        for (auto k2 = 0; k2 < max2; ++k2) {
          std::bitset<64> subset2(k2);
          if (subset2.count() <= half2) {
            for (auto k3 = 0; k3 < max3; ++k3) {
              std::bitset<64> subset3(k3);
              if (subset3.count() <= half3) {
                for (auto k1 = 0; k1 < data_->nCut(j1); ++k1) {
                  MetaData record;
                  record.vIdx.push_back(j1);
                  record.vIdx.push_back(j2);
                  record.vIdx.push_back(j3);
                  record.cIdx.push_back(k1);
                  record.cIdx.push_back(k2);
                  record.cIdx.push_back(k3); 
                  log_.push_back(std::move(record));           
                }
              }
            }
          }
        }
      }
    }
  }

  for (auto j1 = nCont + nOrd; j1 < nVar; ++j1) {
    auto max1 = 1 << data_->nCut(j1);
    auto half1 = data_->nCut(j1) / 2; 
    for (auto j2 = j1 + 1; j2 < nVar; ++j2) {
      auto max2 = 1 << data_->nCut(j2);
      auto half2 = data_->nCut(j2) / 2; 
      for (auto j3 = j2 + 1; j3 < nVar; ++j3) {
        auto max3 = 1 << data_->nCut(j3);
        auto half3 = data_->nCut(j3) / 2;
        for (auto k1 = 0; k1 < max1; ++k1) {
          std::bitset<64> subset1(k1);
          if (subset1.count() <= half1) {
            for (auto k2 = 0; k2 < max2; ++k2) {
              std::bitset<64> subset2(k2);
              if (subset2.count() <= half2) {
                for (auto k3 = 0; k3 < max3; ++k3) {
                  std::bitset<64> subset3(k3);
                  if (subset3.count() <= half3) {
                    MetaData record;
                    record.vIdx.push_back(j1);
                    record.vIdx.push_back(j2);
                    record.vIdx.push_back(j3);
                    record.cIdx.push_back(k1);
                    record.cIdx.push_back(k2);
                    record.cIdx.push_back(k3); 
                    log_.push_back(std::move(record));
                  }
                }
              }
            }
          }
        }
      }
    }
  }  
}

} // namespace ITR
