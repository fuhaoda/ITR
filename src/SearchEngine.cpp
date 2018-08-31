#include <iostream>
#include <vector>
#include <bitset>
#include "SearchEngine.h"

namespace ITR {

SearchEngine::SearchEngine(int depth, const Data *data) {
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
  auto nChoices = choices_.size() / depth_;
  auto nSample = data_->nSample();

  result_.resize(nChoices * (1 << (depth_ + 1)));
  
  if (depth_ == 1) {
    for (auto i = 0; i < nSample; ++i) {
      int act = data_->act(i, 0);
      double resp = data_->resp(i, 0); 
      for (auto j = 0; j < nChoices; ++j) {
        bool m1 = data_->inCut(i, choices_[2 * j], choices_[2 * j + 1]);
        result_[4 * j + 2 * m1 + act] += resp; 
      }
    }

    for (auto i = 0; i < nChoices; ++i) {
      double *r = &result_[4 * i];
      r[0] = r[1] - r[0];
      r[1] = r[3] - r[2];
    }        
  } else if (depth_ == 2) {
    for (auto i = 0; i < nSample; ++i) {
      int act = data_->act(i, 0);
      double resp = data_->resp(i, 0); 
      for (auto j = 0; j < nChoices; ++j) {
        bool m1 = data_->inCut(i, choices_[2 * j], choices_[2 * j + 1]);
        bool m2 = data_->inCut(i, choices_[2 * j + 2], choices_[2 * j + 3]); 
        result_[8 * j + 4 * m1 + 2 * m2 + act] += resp;
      }
    }

    for (auto i = 0; i < nChoices; ++i) {
      double *r = &result_[8 * i];
      r[0] = r[1] - r[0];
      r[1] = r[3] - r[2];
      r[2] = r[5] - r[4];
      r[3] = r[7] - r[6];
    }
  } else { // depth_ == 3
    for (auto i = 0; i < nSample; ++i) {
      int act = data_->act(i, 0);
      double resp = data_->resp(i, 0); 
      for (auto j = 0; j < nChoices; ++j) {
        bool m1 = data_->inCut(i, choices_[2 * j], choices_[2 * j + 1]);
        bool m2 = data_->inCut(i, choices_[2 * j + 2], choices_[2 * j + 3]);
        bool m3 = data_->inCut(i, choices_[2 * j + 4], choices_[2 * j + 5]);
        result_[16 * j + 8 * m1 + 4 * m2 + 2 * m3 + act] += resp;
      }
    }

    for (auto i = 0; i < nChoices; ++i) {
      double *r = &result_[16 * i];
      r[0] = r[1] - r[0];
      r[1] = r[3] - r[2];
      r[2] = r[5] - r[4];
      r[3] = r[7] - r[6];
      r[4] = r[9] - r[8];
      r[5] = r[11] - r[10];
      r[6] = r[13] - r[12];
      r[7] = r[15] - r[14];
    }
  }
}

void SearchEngine::setDepthOneChoices() {
  auto nCont = data_->nCont();
  auto nOrd = data_->nOrd();
  auto nVar = data_->nVar();
  for (auto j = 0; j < nCont + nOrd; ++j) {
    for (auto k = 0; k < data_->nCut(j); ++k) {
      choices_.push_back(j); // variable j
      choices_.push_back(k); // cut k
    }
  }

  for (auto j = nCont + nOrd; j < nVar; ++j) {
    auto max = 1 << data_->nCut(j);
    auto half = data_->nCut(j) / 2;
    for (auto k = 0; k < max; ++k) {
      std::bitset<64> subset(k);
      if (subset.count() <= half) {
        choices_.push_back(j); // variable j
        choices_.push_back(k); // cut k
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
          choices_.push_back(j1);
          choices_.push_back(k1);
          choices_.push_back(j2);
          choices_.push_back(k2);
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
            choices_.push_back(j1);
            choices_.push_back(k1);
            choices_.push_back(j2);
            choices_.push_back(k2);
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
              choices_.push_back(j1);
              choices_.push_back(k1);
              choices_.push_back(j2);
              choices_.push_back(k2);
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
              choices_.push_back(j1);
              choices_.push_back(k1);
              choices_.push_back(j2);
              choices_.push_back(k2);
              choices_.push_back(j3);
              choices_.push_back(k3);
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
                choices_.push_back(j1);
                choices_.push_back(k1);
                choices_.push_back(j2);
                choices_.push_back(k2);
                choices_.push_back(j3);
                choices_.push_back(k3);
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
                  choices_.push_back(j1);
                  choices_.push_back(k1);
                  choices_.push_back(j2);
                  choices_.push_back(k2);
                  choices_.push_back(j3);
                  choices_.push_back(k3);
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
                    choices_.push_back(j1);
                    choices_.push_back(k1);
                    choices_.push_back(j2);
                    choices_.push_back(k2);
                    choices_.push_back(j3);
                    choices_.push_back(k3);
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
