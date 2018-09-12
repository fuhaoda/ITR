#include <memory>
#include <iostream>
#include "Data.h"
#include "SearchEngine.h"
#include "Covariate.h"
#include "gtest/gtest.h"

namespace {

TEST(CovariateCovertTest, Cont) {
  std::vector<double> arr = {5.0, 1.0, 9.0, 3.0, 14.0, 9.0, 7.0};
  double fct = 10.0 / arr.size();

  std::vector<double> expect = {2.5, 0.5, 4.5, 1.5, 6.5, 5.5, 3.5}; 
  ITR::convertContToDeciles(arr);

  for (size_t i = 0; i < arr.size(); ++i)
    EXPECT_EQ(fct * expect[i], arr[i]) << "Values differ at index " << i;
}  

TEST(CovariateCovertTest, Ord) {
  std::vector<int> arr = {1, 2, 3, 3, 5, 2, 9, 3, 1, 2};
  std::set<int> uniq;
  for (auto v : arr)
    uniq.insert(v);

  std::vector<int> expect = {0, 1, 2, 2, 3, 1, 4, 2, 0, 1};
  ITR::convertOrdToRanks(arr, uniq);

  for (size_t i = 0; i < arr.size(); ++i)
    EXPECT_EQ(expect[i], arr[i]) << "Values differ at index " << i; 
}

TEST(CovariateConvertTest, Nom) {
  std::vector<int> arr = {3, 2, 7, 6, 6, 2}; 
  std::set<int> uniq;
  for (auto v : arr)
    uniq.insert(v);
  
  std::vector<int> expect = {2, 1, 8, 4, 4, 1};
  ITR::convertNomToBitMasks(arr, uniq);

  for (size_t i = 0; i < arr.size(); ++i)
    EXPECT_EQ(expect[i], arr[i]) << "Values differ at index " << i; 
}

TEST(DataTest, Load) {
  ITR::Data data("sample100.csv");  
  std::vector<int> nCut = {10, 10, 10, 5, 5, 5, 16, 16, 16}; 
  const double T0 = 2841.77937;
  const double a28 = 0;
  const double y_r11_c0 = 12.4483;
  
  EXPECT_EQ(100, data.nSample());
  EXPECT_EQ(3, data.nCont());
  EXPECT_EQ(3, data.nOrd());
  EXPECT_EQ(3, data.nNom());
  EXPECT_EQ(1, data.nResp());
  EXPECT_EQ(T0, data.T0()); 
  EXPECT_EQ(a28, data.act(28));
  EXPECT_EQ(y_r11_c0, data.resp(11, 0)); 

  for (size_t i = 0; i < nCut.size(); ++i)
    EXPECT_EQ(nCut[i], data.nCut(i)) << "Values differ at index " << i;

  // Check which cuts does each component of continuous variable #1 fall in
  std::vector<int> cidx = {12, 37, 67, 16, 92, 28, 88, 71, 74, 1};
  for (size_t i = 0; i < cidx.size(); ++i) {
    const auto mask = data.cutMask(0, i);
    EXPECT_EQ(true, mask[cidx[i]]) << "Values differ at index " << i;
  }
}

} // namespace 

