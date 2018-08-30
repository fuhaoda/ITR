#include <iostream>
#include <vector>
#include <bitset>
#include "SearchEngine.h"

namespace ITR {

void SearchEngine::run(Data *data) {
  // Note
  // The original code assumes that action and response are column vectors
  // For each row i, only action[i][0] and response[i][0] are used.
  //
 
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

void SearchEngine::depthOneSearch(Data *data) {
  std::cout << "Depth one search ...\n";

  auto nSample = data->nSample();

  auto nCont = data->nCont();
  auto nOrd = data->nOrd();
  auto nNom = data->nNom();
  auto nVar = nCont + nOrd + nNom; 
  
  auto nAct = data->nAct();
  auto nResp = data->nResp();

  const auto cvar = data->cvar(); 
  const auto act = data->act();
  const auto resp = data->resp();
  
  struct CutInfo {
    CutInfo(int v, int c): vIdx{v}, cIdx{c} {}
    int vIdx; // Index of the variable
    int cIdx; // Index of the cut for the variable
  };

  // Vector to save the search result
  std::vector<double> result;

  // Vector to save the cut information
  std::vector<CutInfo> info;
  
  // Go over row zero of the continuous variable
  for (auto j = 0; j < nCont; ++j) {
    auto nCut = data->nCut(j);

    for (auto k = 0; k < nCut; ++k) {
      // Keep a record of the cut
      //info.emplace_back({j, k});

      // Save four values for each cut
      double v[4] = {0.0};

      // cvar[0][j] = cvar[0 * nVar + j]
      auto mask = cvar[j] <= k;       
      v[2 * mask + act[0]] = resp[0]; 

      result.push_back(v[0]);
      result.push_back(v[1]);
      result.push_back(v[2]);
      result.push_back(v[3]);
    }
  }

  // Go over row zero of the ordinal variable
  for (auto j = nCont; j < nCont + nOrd; ++j) {
    auto nCut = data->nCut(j);
    for (auto k = 0; k < nCut; ++k) {
      //info.emplace_back({j, k});
      double v[4] = {0.0};
      auto mask = cvar[j] <= k;
      v[2 * mask + act[0]] = resp[0]; 

      result.push_back(v[0]);
      result.push_back(v[1]);
      result.push_back(v[2]);
      result.push_back(v[3]);
    }
  }

  // Go over row zero of the nominal variable
  for (auto j = nCont + nOrd; j < nVar; ++j) {
    auto nUniq = data->nCut(j);
    auto nMax = 1 << nUniq;
    auto nUniq2 = nUniq / 2; 
    
    for (auto k = 0; k < nMax; ++k) {
      std::bitset<64> subset(k);
      if (subset.count() > nUniq2)
        continue;

      //info.emplace_back({j, k});

      double v[4] = {0.0}; 
      auto mask = cvar[j] & k;
      v[2 * mask + act[0]] = resp[0]; 

      result.push_back(v[0]);
      result.push_back(v[1]);
      result.push_back(v[2]);
      result.push_back(v[3]);
    }
  }

  // The size of result vector is fixed at this point.
  int iter = 0;
  
  // Go over the remaining row of the variables
  for (auto i = 1; i < nSample; ++i) {
    // Go over continuous variable
    for (auto j = 0; j < nCont; ++j) {
      for (auto k = 0; k < data->nCut(j); ++k) {
        auto mask = cvar[i * nVar + j] <= k;
        result[iter + 2 * mask + act[i * nAct]] += resp[i * nResp]; 
        iter += 4;
      }
    }

    // Go over ordinal variable
    for (auto j = nCont; j < nCont + nOrd; ++j) {
      for (auto k = 0; k < data->nCut(j); ++k) {
        auto mask = cvar[i * nVar + j] <= k;
        result[iter + 2 * mask + act[i * nAct]] += resp[i * nResp];
        iter += 4;
      }
    }

    // Go over nominal variable
    for (auto j = nCont + nOrd; j < nVar; ++j) {
      auto nUniq = data->nCut(j);
      auto nMax = 1 << nUniq;
      auto nUniq2 = nUniq / 2;
      for (auto k = 0; k < nMax; ++k) {
        std::bitset<64> subset(k);
        if (subset.count() > nUniq2)
          continue;

        auto mask = cvar[i * nVar + j] & k;
        result[iter + 2 * mask + act[i * nAct]] += resp[i * nResp];
        iter += 4;
      }
    }
  }

  for (iter = 0; iter < result.size() / 4; ++iter) {
    result[4 * iter] = result[4 * iter + 1] - result[4 * iter];
    result[4 * iter + 2] = result[4 * iter + 3] - result[4 * iter + 2];
  }

  // TODO
}

void SearchEngine::depthTwoSearch(Data *data) {
  std::cout << "Depth two search ...\n";
}

void SearchEngine::depthThreeSearch(Data *data) {
  std::cout << "Depth three search ...\n"; 
}




} // namespace ITR
