#include <iostream>
#include <vector>
#include <bitset>
#include "SearchEngine.h"

namespace ITR {

void SearchEngine::run(const Data *data) {
  // TODO: Extension needed. The original code assumes that action and response
  // are column vectors, i.e., only action[i][0] and response[i][0] are used. 

  switch (depth_) {
  case 1:
    depthOneSearch(data); 
    break; 
  case 2:
    depthTwoSearch(data);
    break;
  case 3:
    depthThreeSearch(data);
    break;
  default:
    break;
  }
}

inline void SearchEngine::updateResult(std::vector<double> &result,
                                       const bool *m,
                                       const Data *data) const {
  if (depth_ == 1) {
    double v[4] = {0.0};
    v[2 * m[0] + data->act(0,0)] = data->resp(0,0);

    for (auto i = 0; i < 4; ++i)
      result.push_back(v[i]); 
  } else if (depth_ == 2) {
    double v[8] = {0.0};
    v[4 * m[0] + 2 * m[1] + data->act(0,0)] = data->resp(0,0);

    for (auto i = 0; i < 8; ++i)
      result.push_back(v[i]);
  } else { // depth_ == 3
    double v[16] = {0.0};
    v[8 * m[0] + 4 * m[1] + 2 * m[2] + data->act(0,0)] = data->resp(0,0);
    
    for (auto i = 0; i < 16; ++i)
      result.push_back(v[i]);
  }
}

void SearchEngine::depthOneSearch(const Data *data) {
  std::cout << "Depth one search ...\n";

  auto nSample = data->nSample();
  auto nCont = data->nCont();
  auto nOrd = data->nOrd();
  auto nVar = data->nVar(); 

  // Vector to save the search result
  std::vector<double> result;

  // Vector to save the cut information
  std::vector<CutInfo<1>> info; 

  // Go over row zero of the continuous and ordinal variables
  for (auto j = 0; j < nCont + nOrd; ++j) {
    auto nCut = data->nCut(j);
    for (auto k = 0; k < nCut; ++k) {
      int cut[2] = {j, k}; 
      info.emplace_back(cut);

      auto mask = data->inCut(0, j, k); 
      updateResult(result, &mask, data);
    }
  }

  // Go over row zero of the nominal variable
  for (auto j = nCont + nOrd; j < nVar; ++j) {
    auto nUniq = data->nCut(j);
    auto nMax = 1 << nUniq;
    auto nUniqHalf = nUniq / 2;
    for (auto k = 0; k < nMax; ++k) {
      std::bitset<64> subset(k);
      if (subset.count() > nUniqHalf)
        continue;

      int cut[2] = {j, k};
      info.emplace_back(cut);

      auto mask = data->inCut(0, j, k);
      updateResult(result, &mask, data);
    }
  }
    
  // Go over the remaining rows of the variables
  for (auto i = 1; i < nSample; ++i) {
    int iter = 0;

    // Go over all possible cuts on row i of the variables
    for (auto const &cut : info) {
      auto mask = data->inCut(i, cut.info[0], cut.info[1]);
      result[iter + 2 * mask + data->act(i, 0)] += data->resp(i, 0);
      iter += 4;
    }
  }

  for (auto i = 0; i < info.size(); ++i) {
    double *r = &result[4 * i];
    r[0] = r[1] - r[0];
    r[2] = r[3] - r[2];
  }

  // TODO, sort and save the best value and the corresponding cut? 
}

void SearchEngine::depthTwoSearch(const Data *data) {
  std::cout << "Depth two search ...\n";

  auto nSample = data->nSample();
  auto nCont = data->nCont();
  auto nOrd = data->nOrd();
  auto nVar = data->nVar();

  // Vector to save the search result
  std::vector<double> result;

  // Vector to save the cut information
  std::vector<CutInfo<2>> info; 
  
  // Go over row zero to discover all the cuts
  for (auto j1 = 0; j1 < nCont + nOrd; ++j1) {
    auto nCut1 = data->nCut(j1);

    for (auto j2 = j1 + 1; j2 < nCont + nOrd; ++j2) {
      auto nCut2 = data->nCut(j2);

      for (auto c1 = 0; c1 < nCut1; ++c1) {
        for (auto c2 = 0; c2 < nCut2; ++c2) {
          int cut[4] = {j1, c1, j2, c2};
          info.emplace_back(cut);

          bool mask[2]{data->inCut(0, j1, c1), data->inCut(0, j2, c2)};
          updateResult(result, mask, data);
        }
      }
    }


    for (auto j2 = nCont + nOrd; j2 < nVar; ++j2) {
      auto nUniq2 = data->nCut(j2);
      auto nMax2 = 1 << nUniq2;
      auto nUniq2Half = nUniq2 / 2;

      for (auto c2 = 0; c2 < nMax2; ++c2) {
        std::bitset<64> subset2(c2);
        if (subset2.count() > nUniq2Half)
          continue;

        for (auto c1 = 0; c1 < nCut1; ++c1) {
          int cut[4] = {j1, c1, j2, c2};
          info.emplace_back(cut);

          bool mask[2]{data->inCut(0, j1, c1),  data->inCut(0, j2, c2)}; 
          updateResult(result, mask, data);
        }
      }
    }
  }

  for (auto j1 = nCont + nOrd; j1 < nVar; ++j1) {
    auto nUniq1 = data->nCut(j1);
    auto nMax1 = 1 << nUniq1;
    auto nUniq1Half = nUniq1 / 2;

    for (auto j2 = j1 + 1; j2 < nVar; ++j2) {
      auto nUniq2 = data->nCut(j2);
      auto nMax2 = 1 << nUniq2;
      auto nUniq2Half = nUniq2 / 2;

      for (auto c1 = 0; c1 < nMax1; ++c1) {
        std::bitset<64> subset1(c1);
        if (subset1.count() > nUniq1Half)
          continue;

        for (auto c2 = 0; c2 < nMax2; ++c2) {
          std::bitset<64> subset2(c2);
          if (subset2.count() > nUniq2Half)
            continue;

          int cut[4] = {j1, c1, j2, c2};
          info.emplace_back(cut);

          bool mask[2]{data->inCut(0, j1, c1), data->inCut(0, j2, c2)};
          updateResult(result, mask, data);
        }
      }
    }
  }

  // Go over the remaining rows of the variables
  for (auto i = 1; i < nSample; ++i) {
    int iter = 0;

    // Go over all possible cuts on row i of the variables
    for (auto const &cut : info) {
      auto m1 = data->inCut(i, cut.info[0], cut.info[1]);
      auto m2 = data->inCut(i, cut.info[2], cut.info[3]);
      result[iter + 4 * m1 + 2 * m2 + data->act(i, 0)] += data->resp(i, 0);
      iter += 8;
    }
  }

  for (auto i = 0; i < info.size(); ++i) {
    double *r = &result[8 *i];
    r[0] = r[1] - r[0];
    r[1] = r[3] - r[2];
    r[2] = r[5] - r[4];
    r[3] = r[7] - r[6];
  }
}

void SearchEngine::depthThreeSearch(const Data *data) {
  std::cout << "Depth three search ...\n"; 
}




} // namespace ITR
