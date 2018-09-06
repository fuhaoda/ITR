#include <memory>
#include "Data.h"
#include "SearchEngine.h"
#include "Covariate.h"
#include "gtest/gtest.h"
#include <iostream>

namespace {

TEST(CovariateCovertTest, Cont) {
  std::vector<double> arr = {5.0, 1.0, 9.0, 3.0, 14.0, 9.0, 7.0};
  double fct = 10.0 / arr.size();

  std::vector<double> expect = {2.5, 0.5, 4.5, 1.5, 6.5, 5.5, 3.5}; 
  ITR::convertContToDeciles(arr);

  for (auto i = 0; i < arr.size(); ++i)
    EXPECT_EQ(fct * expect[i], arr[i]) << "Values differ at index " << i;
}  

TEST(CovariateCovertTest, Ord) {
  std::vector<int> arr = {1, 2, 3, 3, 5, 2, 9, 3, 1, 2};
  std::set<int> uniq;
  for (auto v : arr)
    uniq.insert(v);

  std::vector<int> expect = {0, 1, 2, 2, 3, 1, 4, 2, 0, 1};
  ITR::convertOrdToRanks(arr, uniq);

  for (auto i = 0; i < arr.size(); ++i)
    EXPECT_EQ(expect[i], arr[i]) << "Values differ at index " << i; 
}

TEST(CovariateConvertTest, Nom) {
  std::vector<int> arr = {3, 2, 7, 6, 6, 2}; 
  std::set<int> uniq;
  for (auto v : arr)
    uniq.insert(v);
  
  std::vector<int> expect = {2, 1, 8, 4, 4, 1};
  ITR::convertNomToBitMasks(arr, uniq);

  for (auto i = 0; i < arr.size(); ++i)
    EXPECT_EQ(expect[i], arr[i]) << "Values differ at index " << i; 
}


// TEST(DataTest, ParseCSVHeader) {
//   std::unique_ptr<ITR::Data> data = std::make_unique<ITR::Data>("sample100.csv"); 
//   //ITR::Data *data = std::make_unique<ITR::Data>("sample100.csv"); 
// }




} // namespace 

#if 0

// /**
// * \class Test
// *
// * \brief Test ITR
// *
// * \author $Jie Xue$
// *
// * \version $Revision: 1.5 $
// *
// *
// * Contact: xue_Jie@lilly.com
// *
// * Created on: Wed Mar 14 2018
// *
// *
// */

// #include "pch.h"

// #include "Data.h"
// #include "LoadSimpleTestData.h"
// #include "LoadCSVData.h"
// #include "CreateSimulationData.h"
// #include "ITR.h"
// #include "LoadData.h"
// #include "CreateX.h"
// #include "CreateLookUpTable.h"
// #include "OneDepthSearch.h"
// #include "TwoDepthSearch.h"
// #include "ThreeDepthSearch.h"
// #include "FlexDepthSearch.h"

// namespace {
// 	using ::testing::TestWithParam;
// 	using ::testing::Values;

// 	// Simple Test Data
// 	static const vector<unsigned int> id{ 1,2,3,4,5,6,7,8,9,10 };
// 	static const vector<vector<double>> y{ { 1,2,3,4,5,3,3,3,3,3 } };
// 	static const vector<vector<unsigned int>> actions{ { 1,1,1,1,1,2,2,2,2,2 } };
// 	static const vector<vector<double>> x_Cont{ {} };
// 	static const vector<vector<unsigned int>> x_Ord{ { 1,2,3,4,5,1,2,3,4,5 } };
// 	static const vector<vector<unsigned int>> x_Nom{ {} };
// 	static const vector<unsigned int> x_DataType{ 1 };
// 	static const vector<unsigned int> check_Range{ 2,3,4,5,6 };
// 	static const unsigned int check_Actions_1[10] = { 1,1,1,1,1,0,0,0,0,0 };
// 	static const unsigned int check_Actions_2[10] = { 0,0,0,0,0,1,1,1,1,1 };
// 	static const double check_Y[10] = { 1,2,3,4,5,3,3,3,3,3 };
// 	static const bool cut2[10] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 };
// 	static const bool cut3[10] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 };
// 	static const bool cut4[10] = { 1, 1, 1, 0, 0, 1, 1, 1, 0, 0 };
// 	static const bool cut5[10] = { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0 };
// 	static const bool cut6[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const float E_Adam = 3.4;                 // A= 2 for X >= 2
// 	static const float E_Barry = 3.6;                // A= 2 for X >= 3
// 	static const vector<unsigned int> index_Adam{ 0, 2 };
// 	static const vector<unsigned int> index_Barry{ 0, 3 };
// 	static const int dir_Adam = 2;
// 	static const int dir_Barry = 3;

// 	// CSV Test Data
// 	static const vector<unsigned int> col_0{ 0, 9, 2, 2, 4, 9, 1, 1, 0, 8, 0, 6, 0, 5, 6, 3, 3, 9, 5, 5, 5, 3, 6,
// 		2, 0, 6, 4, 9, 5, 3, 3, 5, 3, 1, 4, 2, 4, 1, 7, 2, 4, 7, 0, 8, 7, 1,
// 		5, 8, 2, 8, 0, 7, 4, 6, 2, 9, 3, 6, 1, 4, 5, 8, 1, 1, 7, 2, 7, 2, 6,
// 		9, 9, 7, 0, 5, 8, 7, 1, 6, 3, 8, 8, 2, 5, 9, 9, 0, 8, 1, 6, 3, 4, 8,
// 		4, 7, 0, 6, 4, 3, 7, 9 };   /**< Converted continuous variable */
// 	static const vector<unsigned int> col_1{ 1, 0, 7, 3, 4, 4, 2, 8, 2, 4, 6, 3, 3, 4, 7, 9, 8, 8, 3, 6, 2, 2, 5,
// 		6, 9, 2, 5, 5, 7, 7, 2, 9, 9, 1, 9, 0, 9, 7, 3, 1, 1, 3, 0, 5, 0, 4,
// 		6, 0, 7, 1, 1, 7, 1, 7, 8, 1, 3, 8, 6, 7, 6, 0, 5, 4, 5, 5, 8, 4, 4,
// 		4, 9, 8, 1, 4, 8, 0, 2, 6, 3, 2, 2, 5, 3, 0, 0, 7, 0, 8, 6, 9, 1, 5,
// 		2, 8, 9, 3, 9, 5, 6, 6 };   /**< Converted continuous variable */
// 	static const vector<unsigned int> col_2{ 7, 3, 0, 0, 2, 1, 9, 2, 8, 3, 7, 4, 8, 5, 8, 8, 2, 9, 0, 2, 4, 2, 2,
// 		6, 6, 7, 6, 6, 7, 5, 8, 5, 1, 5, 4, 4, 4, 9, 3, 4, 4, 9, 4, 1, 7, 7,
// 		1, 9, 5, 5, 1, 6, 7, 9, 4, 2, 0, 9, 1, 8, 4, 0, 8, 0, 7, 8, 3, 5, 2,
// 		9, 1, 8, 1, 3, 9, 9, 3, 1, 0, 6, 5, 6, 5, 2, 3, 0, 3, 0, 6, 6, 0, 5,
// 		3, 7, 3, 2, 8, 7, 1, 6 };   /**< Converted continuous variable */
// 	static const vector<unsigned int> col_3{ 4, 0, 1, 0, 2, 4, 3, 3, 0, 4, 4, 4, 1, 0, 4, 3, 0, 1, 3, 4, 3, 3, 1,
// 		4, 1, 1, 2, 0, 1, 4, 3, 2, 0, 4, 0, 0, 1, 1, 3, 4, 4, 4, 2, 0, 1, 0,
// 		0, 2, 3, 0, 0, 4, 0, 4, 2, 3, 0, 2, 3, 1, 2, 2, 0, 3, 2, 2, 4, 4, 4,
// 		2, 3, 4, 2, 4, 3, 3, 3, 2, 0, 2, 2, 4, 4, 4, 1, 2, 3, 1, 0, 3, 4, 2,
// 		0, 3, 0, 3, 0, 0, 0, 4 };
// 	static const vector<unsigned int> col_4{ 2, 3, 3, 0, 3, 0, 1, 1, 2, 1, 1, 0, 3, 1, 0, 4, 0, 2, 4, 4, 3, 1, 1,
// 		3, 4, 3, 0, 2, 2, 4, 0, 3, 0, 2, 1, 4, 0, 3, 3, 0, 4, 4, 3, 0, 3, 0,
// 		0, 2, 1, 2, 4, 4, 4, 2, 1, 4, 2, 3, 3, 3, 4, 4, 2, 1, 2, 2, 3, 1, 1,
// 		3, 1, 2, 1, 4, 3, 4, 2, 4, 4, 0, 1, 4, 2, 1, 1, 0, 2, 1, 4, 3, 3, 3,
// 		0, 0, 4, 2, 2, 3, 3, 0 };
// 	static const vector<unsigned int> col_5{ 0, 3, 3, 2, 4, 2, 2, 0, 0, 4, 0, 3, 2, 1, 0, 3, 1, 2, 2, 1, 0, 0, 2,
// 		1, 3, 2, 2, 1, 0, 3, 3, 2, 0, 1, 4, 3, 2, 4, 2, 2, 3, 2, 2, 0, 2, 0,
// 		4, 4, 1, 0, 4, 4, 0, 1, 3, 0, 4, 2, 0, 1, 1, 1, 3, 2, 0, 3, 2, 4, 2,
// 		0, 4, 1, 3, 2, 1, 4, 3, 1, 4, 2, 2, 0, 0, 1, 2, 1, 0, 1, 4, 1, 2, 4,
// 		0, 3, 2, 3, 2, 2, 3, 2 };
// 	static const vector<unsigned int> col_6{ 4, 2, 4, 3, 1, 2, 2, 4, 1, 1, 2, 0, 0, 4, 0, 4, 4, 3, 2, 0, 3, 2, 4,
// 		2, 0, 1, 2, 1, 4, 1, 3, 1, 2, 1, 2, 1, 4, 3, 2, 2, 1, 1, 4, 4, 3, 1,
// 		1, 0, 1, 3, 4, 2, 1, 2, 0, 4, 3, 2, 1, 4, 3, 3, 2, 0, 1, 4, 4, 3, 3,
// 		3, 2, 3, 3, 3, 3, 3, 3, 3, 1, 2, 2, 3, 4, 1, 1, 0, 3, 2, 0, 3, 1, 3,
// 		2, 0, 1, 2, 3, 4, 0, 4 };
// 	static const vector<unsigned int> col_7{ 3, 3, 1, 2, 4, 3, 0, 3, 4, 4, 1, 4, 0, 2, 0, 0, 2, 3, 4, 0, 0, 4, 1,
// 		2, 1, 0, 1, 3, 0, 2, 2, 0, 4, 4, 3, 1, 2, 0, 3, 4, 1, 2, 3, 4, 0, 2,
// 		4, 3, 2, 4, 3, 4, 3, 0, 1, 4, 1, 1, 1, 2, 0, 3, 2, 1, 3, 3, 3, 0, 0,
// 		3, 2, 3, 4, 4, 1, 4, 1, 1, 3, 2, 3, 1, 1, 1, 2, 1, 1, 0, 4, 3, 0, 1,
// 		2, 4, 3, 3, 3, 3, 3, 1 };
// 	static const vector<unsigned int> col_8{ 3, 3, 3, 0, 1, 4, 4, 3, 1, 2, 0, 2, 2, 0, 1, 2, 1, 1, 0, 1, 1, 1, 0,
// 		2, 0, 0, 1, 1, 1, 0, 2, 2, 3, 1, 2, 0, 1, 4, 1, 2, 1, 3, 1, 2, 3, 1,
// 		3, 1, 0, 4, 2, 0, 0, 0, 2, 1, 1, 3, 1, 2, 0, 2, 0, 1, 3, 2, 0, 3, 2,
// 		2, 4, 2, 0, 0, 2, 2, 2, 1, 1, 4, 2, 3, 3, 3, 1, 3, 2, 4, 2, 2, 0, 2,
// 		4, 0, 3, 2, 2, 0, 4, 2 };
// 	static const vector<unsigned int> CSVDataType{ 0,0,0,1,1,1,2,2,2 };
// 	static const vector<unsigned int> CSVDataInfo_RangeCont{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
// 	static const vector<unsigned int> CSVDataInfo_RangeOrd{ 1, 2, 3, 4, 5 };
// 	static const vector<unsigned int> CSVDataInfo_RangeNom{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
// 	// CSV Data, LookUp Table
// 	static const bool Cont1Cut1[100] = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
// 		0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
// 		0, 0, 1, 0, 0, 0, 0, 0 };
// 	static const bool Cont1Cut4[100] = { 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0,
// 		1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1,
// 		0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0,
// 		0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0,
// 		0, 0, 1, 0, 0, 1, 0, 0 };
// 	static const bool Cont1Cut10[100] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const bool Cont2Cut1[100] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,
// 		0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0 };
// 	static const bool Cont2Cut5[100] = { 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0,
// 		0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1,
// 		0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1,
// 		1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0,
// 		1, 0, 0, 1, 0, 0, 0, 0 };
// 	static const bool Cont2Cut10[100] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const bool Cont3Cut1[100] = { 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0 };
// 	static const bool Cont3Cut6[100] = { 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1,
// 		0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0,
// 		1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1,
// 		0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1,
// 		1, 0, 1, 1, 0, 0, 1, 0 };
// 	static const bool Cont3Cut10[100] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const bool Ord1Cut1[100] = { 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
// 		1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
// 		1, 0, 1, 0, 1, 1, 1, 0 };
// 	static const bool Ord1Cut2[100] = { 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1,
// 		0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
// 		1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0,
// 		1, 0, 1, 0, 1, 1, 1, 0 };
// 	static const bool Ord1Cut5[100] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const bool Ord2Cut1[100] = { 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1,
// 		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
// 		1, 1, 0, 0, 0, 0, 0, 1 };
// 	static const bool Ord2Cut3[100] = { 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1,
// 		0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1,
// 		1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1,
// 		0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
// 		1, 1, 0, 1, 1, 0, 0, 1 };
// 	static const bool Ord2Cut5[100] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const bool Ord3Cut1[100] = { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0,
// 		0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
// 		0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
// 		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
// 		1, 0, 0, 0, 0, 0, 0, 0 };
// 	static const bool Ord3Cut4[100] = { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
// 		0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
// 		1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0,
// 		1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const bool Ord3Cut5[100] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1 };
// 	static const bool Nom1Cut0[100] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0 };
// 	static const bool Nom1Cut3[100] = { 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
// 		0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
// 		1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0,
// 		0, 1, 1, 0, 0, 0, 1, 0 };
// 	static const bool Nom1Cut15[100] = { 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1,
// 		0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0,
// 		0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
// 		1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1,
// 		0, 0, 0, 0, 1, 1, 0, 1 };
// 	static const bool Nom2Cut0[100] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0 };
// 	static const bool Nom2Cut6[100] = { 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
// 		1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
// 		0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0,
// 		0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
// 		1, 0, 0, 0, 0, 0, 0, 1 };
// 	static const bool Nom2Cut15[100] = { 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0,
// 		0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
// 		1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0,
// 		1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
// 		0, 1, 1, 1, 1, 1, 1, 0 };
// 	static const bool Nom3Cut0[100] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0 };
// 	static const bool Nom3Cut9[100] = { 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0,
// 		0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1,
// 		1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
// 		0, 0, 1, 0, 0, 0, 0, 0 };
// 	static const bool Nom3Cut15[100] = { 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0,
// 		1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
// 		0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0,
// 		1, 0, 1, 0, 0, 0, 1, 0 };

// 	TEST(DataInputTest, TestSimpleInputData) {
// 		Data* data = new LoadSimpleTestData();
// 		data->loadData();

// 		EXPECT_EQ(data->getSampleSize(), 10);
// 		EXPECT_EQ(data->getID(), id);
// 		EXPECT_EQ(data->getID().size(), 10);
// 		EXPECT_EQ(data->getY(), y);
// 		EXPECT_EQ(data->getY().size(), 1);
// 		EXPECT_EQ(data->getActions(), actions);
// 		EXPECT_EQ(data->getActions().size(), 1);
// 		EXPECT_EQ(data->getX_Cont(), x_Cont);
// 		EXPECT_EQ(data->getX_Ord(), x_Ord);
// 		EXPECT_EQ(data->getX_Nom(), x_Nom);
// 		EXPECT_EQ(data->getX_Type(), x_DataType);

// 		delete data;
// 	}

// 	TEST(DataInputTest, TestReadCSV_Size) {
// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();

// 		EXPECT_EQ(data->getSampleSize(), 100);
// 		EXPECT_EQ(data->getID().size(), 100);
// 		EXPECT_EQ(data->getY().size(), 1);
// 		EXPECT_EQ(data->getY().at(0).size(), 100);
// 		EXPECT_EQ(data->getActions().size(), 1);
// 		EXPECT_EQ(data->getActions().at(0).size(), 100);
// 		EXPECT_EQ(data->getX_Cont().size(), 3);
// 		EXPECT_EQ(data->getX_Cont().at(0).size(), 100);
// 		EXPECT_EQ(data->getX_Ord().size(), 3);
// 		EXPECT_EQ(data->getX_Ord().at(0).size(), 100);
// 		EXPECT_EQ(data->getX_Nom().size(), 3);
// 		EXPECT_EQ(data->getX_Nom().at(0).size(), 100);

// 		delete data;
// 	}

// 	TEST(DataInputTest, TestReadCSV_Value) {
// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();

// 		EXPECT_EQ(data->getID().at(0), 1);
// 		EXPECT_EQ(data->getID().at(49), 50);
// 		EXPECT_EQ(data->getID().at(99), 100);
// 		EXPECT_DOUBLE_EQ(data->getX_Cont().at(0).at(0), 20.0871);
// 		EXPECT_DOUBLE_EQ(data->getX_Cont().at(1).at(49), 14.4179);
// 		EXPECT_DOUBLE_EQ(data->getX_Cont().at(2).at(99), 125.196);
// 		EXPECT_EQ(data->getX_Ord().at(0).at(0), 4);
// 		EXPECT_EQ(data->getX_Ord().at(1).at(49), 2);
// 		EXPECT_EQ(data->getX_Ord().at(2).at(99), 2);
// 		EXPECT_EQ(data->getX_Nom().at(0).at(0), 4);
// 		EXPECT_EQ(data->getX_Nom().at(1).at(49), 4);
// 		EXPECT_EQ(data->getX_Nom().at(2).at(99), 2);
// 		EXPECT_EQ(data->getActions().at(0).at(0), 0);
// 		EXPECT_EQ(data->getActions().at(0).at(49), 1);
// 		EXPECT_EQ(data->getActions().at(0).at(99), 0);
// 		EXPECT_DOUBLE_EQ(data->getY().at(0).at(0), 31.2112);
// 		EXPECT_DOUBLE_EQ(data->getY().at(0).at(49), 4.7763);
// 		EXPECT_DOUBLE_EQ(data->getY().at(0).at(99), 11.5871);

// 		delete data;
// 	}

// 	TEST(DataInputTest, TestSimulationData_Size) {
// 		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
// 		data->loadData();

// 		EXPECT_EQ(data->getSampleSize(), 200);
// 		EXPECT_EQ(data->getID().size(), 200);
// 		EXPECT_EQ(data->getActions().size(), 1);
// 		EXPECT_EQ(data->getY().size(), 1);
// 		EXPECT_EQ(data->getX_Cont().size(), 2);
// 		EXPECT_EQ(data->getX_Ord().size(), 3);
// 		EXPECT_EQ(data->getX_Nom().size(), 4);

// 		delete data;
// 	}

// 	TEST(LoadDataTest, SimpleTestData) {
// 		Data* data = new LoadSimpleTestData();
// 		data->loadData();
// 		ITR* itr = new LoadData(data);
// 		itr->process();

// 		EXPECT_EQ(itr->getData()->getSampleSize(), 10);
// 		EXPECT_EQ(itr->getData()->getID(), id);
// 		EXPECT_EQ(itr->getData()->getY(), y);
// 		EXPECT_EQ(itr->getData()->getActions(), actions);
// 		EXPECT_EQ(itr->getData()->getX_Cont(), x_Cont);
// 		EXPECT_EQ(itr->getData()->getX_Ord(), x_Ord);
// 		EXPECT_EQ(itr->getData()->getX_Nom(), x_Nom);
// 		EXPECT_EQ(itr->getData()->getX_Type(), x_DataType);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(LoadDataTest, CSVData) {
// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();
// 		ITR* itr = new LoadData(data);
// 		itr->process();

// 		EXPECT_EQ(itr->getData()->getSampleSize(), 100);
// 		EXPECT_EQ(itr->getData()->getID().size(), 100);
// 		EXPECT_EQ(itr->getData()->getY().size(), 1);
// 		EXPECT_EQ(itr->getData()->getY()[0].size(), 100);
// 		EXPECT_EQ(itr->getData()->getActions().size(), 1);
// 		EXPECT_EQ(itr->getData()->getActions()[0].size(), 100);
// 		EXPECT_EQ(itr->getData()->getX_Cont().size(), 3);
// 		EXPECT_EQ(itr->getData()->getX_Cont()[0].size(), 100);
// 		EXPECT_EQ(itr->getData()->getX_Ord().size(), 3);
// 		EXPECT_EQ(itr->getData()->getX_Ord()[0].size(), 100);
// 		EXPECT_EQ(itr->getData()->getX_Nom().size(), 3);
// 		EXPECT_EQ(itr->getData()->getX_Nom()[0].size(), 100);

// 		EXPECT_EQ(itr->getData()->getID()[0], 1);
// 		EXPECT_EQ(itr->getData()->getID()[49], 50);
// 		EXPECT_EQ(itr->getData()->getID()[99], 100);
// 		EXPECT_DOUBLE_EQ(itr->getData()->getX_Cont()[0][0], 20.0871);
// 		EXPECT_DOUBLE_EQ(itr->getData()->getX_Cont()[1][49], 14.4179);
// 		EXPECT_DOUBLE_EQ(itr->getData()->getX_Cont()[2][99], 125.196);
// 		EXPECT_EQ(itr->getData()->getX_Ord()[0][0], 4);
// 		EXPECT_EQ(itr->getData()->getX_Ord()[1][49], 2);
// 		EXPECT_EQ(itr->getData()->getX_Ord()[2][99], 2);
// 		EXPECT_EQ(itr->getData()->getX_Nom()[0][0], 4);
// 		EXPECT_EQ(itr->getData()->getX_Nom()[1][49], 4);
// 		EXPECT_EQ(itr->getData()->getX_Nom()[2][99], 2);
// 		EXPECT_EQ(itr->getData()->getActions()[0][0], 0);
// 		EXPECT_EQ(itr->getData()->getActions()[0][49], 1);
// 		EXPECT_EQ(itr->getData()->getActions()[0][99], 0);
// 		EXPECT_DOUBLE_EQ(itr->getData()->getY()[0][0], 31.2112);
// 		EXPECT_DOUBLE_EQ(itr->getData()->getY()[0][49], 4.7763);
// 		EXPECT_DOUBLE_EQ(itr->getData()->getY()[0][99], 11.5871);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(LoadDataTest, SimulationData) {
// 		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
// 		data->loadData();
// 		ITR* itr = new LoadData(data);
// 		itr->process();

// 		EXPECT_EQ(itr->getData()->getSampleSize(), 200);
// 		EXPECT_EQ(itr->getData()->getID().size(), 200);
// 		EXPECT_EQ(itr->getData()->getActions().size(), 1);
// 		EXPECT_EQ(itr->getData()->getY().size(), 1);
// 		EXPECT_EQ(itr->getData()->getX_Cont().size(), 2);
// 		EXPECT_EQ(itr->getData()->getX_Ord().size(), 3);
// 		EXPECT_EQ(itr->getData()->getX_Nom().size(), 4);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateXTest, SimpleTestData) {
// 		Data* data = new LoadSimpleTestData();
// 		data->loadData();
// 		ITR* itr = new CreateX(new LoadData(data));
// 		itr->process();

// 		EXPECT_EQ(itr->getX(), x_Ord);
// 		EXPECT_EQ(itr->getData(), data);
// 		EXPECT_EQ(itr->getVarType(), x_DataType);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateXTest, CSVTestData) {
// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();
// 		ITR* itr = new CreateX(new LoadData(data));
// 		itr->process();

// 		EXPECT_EQ(itr->getData(), data);
// 		EXPECT_EQ(itr->getVarType(), CSVDataType);
// 		EXPECT_EQ(itr->getX().size(), 9);
// 		EXPECT_EQ(itr->getX()[0].size(), 100);

// 		EXPECT_EQ(itr->getX()[0], col_0);
// 		EXPECT_EQ(itr->getX()[1], col_1);
// 		EXPECT_EQ(itr->getX()[2], col_2);
// 		EXPECT_EQ(itr->getX()[3], col_3);
// 		EXPECT_EQ(itr->getX()[4], col_4);
// 		EXPECT_EQ(itr->getX()[5], col_5);
// 		EXPECT_EQ(itr->getX()[6], col_6);
// 		EXPECT_EQ(itr->getX()[7], col_7);
// 		EXPECT_EQ(itr->getX()[8], col_8);

// 		EXPECT_EQ(itr->getX()[3][0], 4);
// 		EXPECT_EQ(itr->getX()[4][49], 2);
// 		EXPECT_EQ(itr->getX()[5][99], 2);
// 		EXPECT_EQ(itr->getX()[6][0], 4);
// 		EXPECT_EQ(itr->getX()[7][49], 4);
// 		EXPECT_EQ(itr->getX()[8][99], 2);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateXTest, SimulationData) {
// 		vector<unsigned int> SimulationDataType{ 0,0,1,1,1,2,2,2,2 };    /**< Two 0, Three 1, and Four 2 */
// 		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
// 		data->loadData();
// 		ITR* itr = new CreateX(new LoadData(data));
// 		itr->process();

// 		EXPECT_EQ(itr->getData(), data);
// 		EXPECT_EQ(itr->getVarType(), SimulationDataType);
// 		EXPECT_EQ(itr->getX().size(), 2 + 3 + 4);
// 		EXPECT_EQ(itr->getX()[0].size(), 200);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateLookUpTableTest, SimpleTestData_Part_I) {
// 		Data* data = new LoadSimpleTestData();
// 		data->loadData();
// 		ITR* itr = new CreateLookUpTable(new CreateX(new LoadData(data)), 1);
// 		itr->process();
		
// 		// Test T0
// 		EXPECT_EQ(itr->getT0(), 15);

// 		// Test DataInfo
// 		EXPECT_EQ(itr->getDataInfo().size(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarNo(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getRange(), check_Range);

// 		// Test Tables
// 		EXPECT_EQ(data->getSampleSize(), 10);
// 		for (unsigned int i = 0; i < 10; ++i) {
// 			EXPECT_EQ(itr->get_var_A()[i][0], check_Actions_2[i]);
// 			EXPECT_EQ(itr->get_var_Y()[i][0], check_Y[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][0][i], cut2[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][1][i], cut3[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][2][i], cut4[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][3][i], cut5[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][4][i], cut6[i]);
// 		}
// 		EXPECT_EQ(itr->getCutSize()[0], 5);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateLookUpTableTest, SimpleTestData_Part_II) {
// 		Data* data = new LoadSimpleTestData();
// 		data->loadData();
// 		ITR* itr = new CreateLookUpTable(new CreateX(new LoadData(data)), 2);
// 		itr->process();

// 		// Test T0
// 		EXPECT_EQ(itr->getT0(), 15);

// 		// Test DataInfo
// 		EXPECT_EQ(itr->getDataInfo().size(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarNo(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getRange(), check_Range);

// 		// Test Tables
// 		EXPECT_EQ(data->getSampleSize(), 10);
// 		for (unsigned int i = 0; i < 10; ++i) {
// 			EXPECT_EQ(itr->get_var_A()[i][0], check_Actions_1[i]);
// 			EXPECT_EQ(itr->get_var_Y()[i][0], check_Y[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][0][i], cut2[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][1][i], cut3[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][2][i], cut4[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][3][i], cut5[i]);
// 			EXPECT_EQ(itr->get_Table_X()[0][4][i], cut6[i]);
// 		}
// 		EXPECT_EQ(itr->getCutSize()[0], 5);

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateLookUpTableTest, CSVData_Part_I) {
// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();
// 		ITR* itr = new CreateLookUpTable(new CreateX(new LoadData(data)));
// 		itr->process();

// 		// Test T0
// 		EXPECT_DOUBLE_EQ(itr->getT0(), 2841.77937);

// 		// Test DataInfo
// 		EXPECT_EQ(itr->getDataInfo().size(), 9);
// 		// Test DataInfo : X0
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarNo(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarType(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getCutSize(), 10);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getRange(), CSVDataInfo_RangeCont);
// 		// Test DataInfo : X1
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getVarNo(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getVarType(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getCutSize(), 10);
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getRange(), CSVDataInfo_RangeCont);
// 		// Test DataInfo : X2
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getVarNo(), 2);
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getVarType(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getCutSize(), 10);
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getRange(), CSVDataInfo_RangeCont);
// 		// Test DataInfo : X3
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getVarNo(), 3);
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getRange(), CSVDataInfo_RangeOrd);
// 		// Test DataInfo : X4
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getVarNo(), 4);
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getRange(), CSVDataInfo_RangeOrd);
// 		// Test DataInfo : X5
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getVarNo(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getRange(), CSVDataInfo_RangeOrd);
// 		// Test DataInfo : X6
// 		EXPECT_EQ(itr->getDataInfo().at(6)->getVarNo(), 6);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->getVarType(), 2);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->getCutSize(), 16);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->getRange(), CSVDataInfo_RangeNom);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(0, 0), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(1, 0), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(2, 0), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(3, 0), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(4, 0), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(5, 0), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(0, 1), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(1, 1), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(2, 1), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(3, 1), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(4, 1), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(5, 1), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(0, 2), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(1, 2), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(2, 2), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(3, 2), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(4, 2), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(5, 2), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(0, 3), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(1, 3), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(2, 3), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(3, 3), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(4, 3), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(5, 3), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(0, 15), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(1, 15), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(2, 15), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(3, 15), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(4, 15), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(6)->nomContains(5, 15), 0);
// 		// Test DataInfo : X7
// 		EXPECT_EQ(itr->getDataInfo().at(7)->getVarNo(), 7);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->getVarType(), 2);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->getCutSize(), 16);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->getRange(), CSVDataInfo_RangeNom);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(0, 5), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(1, 5), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(2, 5), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(3, 5), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(4, 5), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(5, 5), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(0, 6), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(1, 6), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(2, 6), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(3, 6), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(4, 6), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(7)->nomContains(5, 6), 0);
// 		// Test DataInfo : X8
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getVarNo(), 8);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getVarType(), 2);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getCutSize(), 16);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getRange(), CSVDataInfo_RangeNom);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(0, 10), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(1, 10), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(2, 10), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(3, 10), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(4, 10), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(5, 10), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(0, 11), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(1, 11), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(2, 11), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(3, 11), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(4, 11), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->nomContains(5, 11), 0);
				
// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateLookUpTableTest, CSVData_Part_II) {
// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();
// 		ITR* itr = new CreateLookUpTable(new CreateX(new LoadData(data)));
// 		itr->process();

// 		for (unsigned int i = 0; i < 100; ++i) {
// 			EXPECT_EQ(itr->get_Table_X()[0][0][i], Cont1Cut1[i]);    /**< Cut 1 for Cont 1, < 1 */
// 			EXPECT_EQ(itr->get_Table_X()[0][3][i], Cont1Cut4[i]);    /**< Cut 4 for Cont 1, < 4 */
// 			EXPECT_EQ(itr->get_Table_X()[0][9][i], Cont1Cut10[i]);   /**< Cut 10 for Cont 1, < 10 */
// 			EXPECT_EQ(itr->get_Table_X()[1][0][i], Cont2Cut1[i]);    /**< Cut 1 for Cont 2, < 1 */
// 			EXPECT_EQ(itr->get_Table_X()[1][4][i], Cont2Cut5[i]);    /**< Cut 5 for Cont 2, < 5 */
// 			EXPECT_EQ(itr->get_Table_X()[1][9][i], Cont2Cut10[i]);   /**< Cut 10 for Cont 2, < 10 */
// 			EXPECT_EQ(itr->get_Table_X()[2][0][i], Cont3Cut1[i]);    /**< Cut 1 for Cont 3, < 1 */
// 			EXPECT_EQ(itr->get_Table_X()[2][5][i], Cont3Cut6[i]);    /**< Cut 6 for Cont 3, < 6 */
// 			EXPECT_EQ(itr->get_Table_X()[2][9][i], Cont3Cut10[i]);   /**< Cut 10 for Cont 3, < 10 */
// 			EXPECT_EQ(itr->get_Table_X()[3][0][i], Ord1Cut1[i]);     /**< Cut 1 for Ord 1, < 1 */
// 			EXPECT_EQ(itr->get_Table_X()[3][1][i], Ord1Cut2[i]);     /**< Cut 2 for Ord 1, < 2 */
// 			EXPECT_EQ(itr->get_Table_X()[3][4][i], Ord1Cut5[i]);     /**< Cut 5 for Ord 1, < 5 */
// 			EXPECT_EQ(itr->get_Table_X()[4][0][i], Ord2Cut1[i]);     /**< Cut 1 for Ord 2, < 1 */
// 			EXPECT_EQ(itr->get_Table_X()[4][2][i], Ord2Cut3[i]);     /**< Cut 3 for Ord 2, < 3 */
// 			EXPECT_EQ(itr->get_Table_X()[4][4][i], Ord2Cut5[i]);     /**< Cut 5 for Ord 2, < 5 */
// 			EXPECT_EQ(itr->get_Table_X()[5][0][i], Ord3Cut1[i]);     /**< Cut 1 for Ord 3, < 1 */
// 			EXPECT_EQ(itr->get_Table_X()[5][3][i], Ord3Cut4[i]);     /**< Cut 4 for Ord 3, < 4 */
// 			EXPECT_EQ(itr->get_Table_X()[5][4][i], Ord3Cut5[i]);     /**< Cut 5 for Ord 3, < 5 */
// 			EXPECT_EQ(itr->get_Table_X()[6][0][i], Nom1Cut0[i]);     /**< Cut 0 for Nom 1, [] */
// 			EXPECT_EQ(itr->get_Table_X()[6][3][i], Nom1Cut3[i]);     /**< Cut 3 for Nom 1, [0, 1] */
// 			EXPECT_EQ(itr->get_Table_X()[6][15][i], Nom1Cut15[i]);   /**< Cut 15 for Nom 1, [3, 4] */
// 			EXPECT_EQ(itr->get_Table_X()[7][0][i], Nom2Cut0[i]);     /**< Cut 0 for Nom 2, [] */
// 			EXPECT_EQ(itr->get_Table_X()[7][6][i], Nom2Cut6[i]);     /**< Cut 6 for Nom 2, [1, 2] */
// 			EXPECT_EQ(itr->get_Table_X()[7][15][i], Nom2Cut15[i]);   /**< Cut 15 for Nom 2, [3, 4] */
// 			EXPECT_EQ(itr->get_Table_X()[8][0][i], Nom3Cut0[i]);     /**< Cut 0 for Nom 3, [] */
// 			EXPECT_EQ(itr->get_Table_X()[8][9][i], Nom3Cut9[i]);     /**< Cut 9 for Nom 3, [1, 3] */
// 			EXPECT_EQ(itr->get_Table_X()[8][15][i], Nom3Cut15[i]);   /**< Cut 15 for Nom 3, [3, 4] */
// 		}

// 		delete data;
// 		delete itr;
// 	}

// 	TEST(CreateLookUpTableTest, SimulatedData) {
// 		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
// 		data->loadData();
// 		ITR* itr = new CreateLookUpTable(new CreateX(new LoadData(data)));
// 		itr->process();

// 		// Test DataInfo
// 		EXPECT_EQ(itr->getDataInfo().size(), 2+3+4);
// 		// Test DataInfo : X0 (Cont)
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarNo(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getVarType(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getCutSize(), 10);
// 		EXPECT_EQ(itr->getDataInfo().at(0)->getRange(), CSVDataInfo_RangeCont);
// 		// Test DataInfo : X1 (Cont)
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getVarNo(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getVarType(), 0);
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getCutSize(), 10);
// 		EXPECT_EQ(itr->getDataInfo().at(1)->getRange(), CSVDataInfo_RangeCont);
// 		// Test DataInfo : X2 (Ord)
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getVarNo(), 2);
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(2)->getRange(), CSVDataInfo_RangeOrd);
// 		// Test DataInfo : X3 (Ord)
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getVarNo(), 3);
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(3)->getRange(), CSVDataInfo_RangeOrd);
// 		// Test DataInfo : X4 (Ord)
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getVarNo(), 4);
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getVarType(), 1);
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getCutSize(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(4)->getRange(), CSVDataInfo_RangeOrd);
// 		// Test DataInfo : X5 (Ord)
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getVarNo(), 5);
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getVarType(), 2);
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getCutSize(), 16);
// 		EXPECT_EQ(itr->getDataInfo().at(5)->getRange(), CSVDataInfo_RangeNom);
// 		// Test DataInfo : X8 (Ord)
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getVarNo(), 8);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getVarType(), 2);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getCutSize(), 16);
// 		EXPECT_EQ(itr->getDataInfo().at(8)->getRange(), CSVDataInfo_RangeNom);

// 		delete data;
// 		delete itr;
// 	}

// 	// One Depth Search Test for print (Simple Data)
// 	TEST(OneDepthSearchTest, SimpleTestData_Part_I) {
// 		Data* data = new LoadSimpleTestData();
// 		data->loadData();
// 		ITR* itr = new OneDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data)), 1));
// //		itr->process();

// 		delete data;
// 		delete itr;
// 	}

// 	// One Depth Search Test for output (Simple Data)
// 	TEST(OneDepthSearchTest, SimpleTestData_Part_II) {
// 		Data* data = new LoadSimpleTestData();
// 		data->loadData();
// 		ITR* itr = new OneDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data)), 1),false);
// 		itr->process();

// 		EXPECT_EQ(itr->getSearchResults().size(), 10);
// 		EXPECT_EQ(itr->getSearchResults()[1]->getRes(), E_Adam);
// 		EXPECT_EQ(itr->getSearchResults()[1]->getIndex(), index_Adam);
// 		EXPECT_EQ(itr->getSearchResults()[1]->getDirection(), 1);
// 		EXPECT_EQ(itr->getSearchResults()[3]->getRes(), E_Barry);
// 		EXPECT_EQ(itr->getSearchResults()[3]->getIndex(), index_Barry);
// 		EXPECT_EQ(itr->getSearchResults()[3]->getDirection(), 1);

// 		delete data;
// 		delete itr;
// 	}

// 	// One Depth Search Test for print (CSV Data)
// 	TEST(OneDepthSearchTest, CSCTestData_Part_I) {
// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();

// 		ITR* itr = new OneDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))));
// //		itr->process();

// 		delete data;
// 		delete itr;
// 	}

// 	// One Depth Search Test for output (CSV Data)
// 	TEST(OneDepthSearchTest, CSCTestData_Part_II) {
// 		static const float x0_C1_L = 35.63502;
// 		static const float x0_C1_R = 55.3229314;
// 		static const float x2_C10_L = 56.8355874;
// 		static const float x2_C10_R = 34.122364;
// 		static const float x3_C1_L = 37.333322;
// 		static const float x3_C1_R = 53.6246294;
// 		static const float x5_C5_L = 56.8355874;
// 		static const float x5_C5_R = 34.122364;
// 		static const float x6_C0_L = 34.122364;
// 		static const float x6_C0_R = 56.8355874;
// 		static const float x8_C15_L = 43.081686;
// 		static const float x8_C15_R = 47.8762654;

// 		static const vector<unsigned int> x0{ 0, 1 };
// 		static const vector<unsigned int> x2{ 2, 10 };
// 		static const vector<unsigned int> x3{ 3, 1 };
// 		static const vector<unsigned int> x5{ 5, 5 };
// 		static const vector<unsigned int> x6{ 6, 0 };
// 		static const vector<unsigned int> x8{ 8, 15 };


// 		Data* data = new LoadCSVData("TestSample_100.csv");
// 		data->loadData();

// 		ITR* itr = new OneDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), false);
// 		itr->process();

// 		EXPECT_EQ(itr->getSearchResults().size(), 20*3+10*3+32*3);
// 		// Cont var
// 		EXPECT_EQ(itr->getSearchResults()[0]->getRes(), x0_C1_L);
// 		EXPECT_EQ(itr->getSearchResults()[0]->getIndex(), x0);
// 		EXPECT_EQ(itr->getSearchResults()[0]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[1]->getRes(), x0_C1_R);
// 		EXPECT_EQ(itr->getSearchResults()[1]->getIndex(), x0);
// 		EXPECT_EQ(itr->getSearchResults()[1]->getDirection(), 1);
// 		EXPECT_EQ(itr->getSearchResults()[58]->getRes(), x2_C10_L);
// 		EXPECT_EQ(itr->getSearchResults()[58]->getIndex(), x2);
// 		EXPECT_EQ(itr->getSearchResults()[58]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[59]->getRes(), x2_C10_R);
// 		EXPECT_EQ(itr->getSearchResults()[59]->getIndex(), x2);
// 		EXPECT_EQ(itr->getSearchResults()[59]->getDirection(), 1);
// 		// Ord Var
// 		EXPECT_EQ(itr->getSearchResults()[60]->getRes(), x3_C1_L);
// 		EXPECT_EQ(itr->getSearchResults()[60]->getIndex(), x3);
// 		EXPECT_EQ(itr->getSearchResults()[60]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[61]->getRes(), x3_C1_R);
// 		EXPECT_EQ(itr->getSearchResults()[61]->getIndex(), x3);
// 		EXPECT_EQ(itr->getSearchResults()[61]->getDirection(), 1);
// 		EXPECT_EQ(itr->getSearchResults()[88]->getRes(), x5_C5_L);
// 		EXPECT_EQ(itr->getSearchResults()[88]->getIndex(), x5);
// 		EXPECT_EQ(itr->getSearchResults()[88]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[89]->getRes(), x5_C5_R);
// 		EXPECT_EQ(itr->getSearchResults()[89]->getIndex(), x5);
// 		EXPECT_EQ(itr->getSearchResults()[89]->getDirection(), 1);

// 		// Nom Var
// 		EXPECT_EQ(itr->getSearchResults()[90]->getRes(), x6_C0_L);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getIndex(), x6);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getRes(), x6_C0_R);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getIndex(), x6);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getDirection(), 1);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getRes(), x6_C0_L);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getIndex(), x6);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getRes(), x6_C0_R);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getIndex(), x6);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getDirection(), 1);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getRes(), x6_C0_L);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getIndex(), x6);
// 		EXPECT_EQ(itr->getSearchResults()[90]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getRes(), x6_C0_R);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getIndex(), x6);
// 		EXPECT_EQ(itr->getSearchResults()[91]->getDirection(), 1);
// 		EXPECT_EQ(itr->getSearchResults()[184]->getRes(), x8_C15_L);
// 		EXPECT_EQ(itr->getSearchResults()[184]->getIndex(), x8);
// 		EXPECT_EQ(itr->getSearchResults()[184]->getDirection(), 0);
// 		EXPECT_EQ(itr->getSearchResults()[185]->getRes(), x8_C15_R);
// 		EXPECT_EQ(itr->getSearchResults()[185]->getIndex(), x8);
// 		EXPECT_EQ(itr->getSearchResults()[185]->getDirection(), 1);

// 		delete data;
// 		delete itr;
// 	}

// 	// One Depth Search Test for output (Simulated Data)
// 	TEST(OneDepthSearchTest, SimulationTestData) {
// 		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
// 		data->loadData();
// 		ITR* itr = new OneDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), false);
// 		itr->process();

// 		EXPECT_EQ(itr->getSearchResults().size(), 20 * 2 + 10 * 3 + 32 * 4);

// 		delete data;
// 		delete itr;
	// }

	// // Two Depth Search Test for Output (CSV Data)
	// TEST(TwoDepthSearchTest, CSCTestData) {
	// 	static const float x01_C11 = 57.8523594;
	// 	static const float x01_C1010 = 56.8355874;
	// 	static const float x03_C11 = 57.6648694;
	// 	static const float x03_C105 = 56.8355874;
	// 	static const float x17_C10 = 58.7514954;
	// 	static const float x17_C1015 = 56.8355874;
	// 	static const float x45_C11 = 57.5749274;
	// 	static const float x45_C55 = 56.8355874;
	// 	static const float x58_C10 = 58.0366794;
	// 	static const float x58_C515 = 56.8355874;
	// 	static const float x68_C00 = 56.8355874;
	// 	static const float x68_C1515 = 55.8285714;

		// static const vector<unsigned int> x01c11{ 0, 1, 1, 1 };
		// static const vector<unsigned int> x01c1010{ 0, 10, 1, 10 };
		// static const vector<unsigned int> x03c11{ 0, 1, 3, 1 };
		// static const vector<unsigned int> x03c105{ 0, 10, 3, 5 };
		// static const vector<unsigned int> x17c10{ 1, 1, 7, 0 };
		// static const vector<unsigned int> x17c1015{ 1, 10, 7, 15 };
		// static const vector<unsigned int> x45c11{ 4, 1, 5, 1 };
		// static const vector<unsigned int> x45c55{ 4, 5, 5, 5 };
		// static const vector<unsigned int> x58c10{ 5, 1, 8, 0 };
		// static const vector<unsigned int> x58c515{ 5, 5, 8, 15 };
		// static const vector<unsigned int> x68c10{ 6, 0, 8, 0 };
		// static const vector<unsigned int> x68c515{ 6, 15, 8, 15 };

		Data* data = new LoadCSVData("TestSample_100.csv");
		data->loadData();

		ITR* itr = new TwoDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))),false);
		itr->process();

		// Check for size
		EXPECT_EQ(itr->getSearchResults().size(), 3753);
		//  var0 and var1 (Cont 1 and Cont 2)
		EXPECT_EQ(itr->getSearchResults()[0]->getRes(), x01_C11);
		EXPECT_EQ(itr->getSearchResults()[0]->getIndex(), x01c11);
		EXPECT_EQ(itr->getSearchResults()[0]->getDirection(), 3);
		EXPECT_EQ(itr->getSearchResults()[99]->getRes(), x01_C1010);
		EXPECT_EQ(itr->getSearchResults()[99]->getIndex(), x01c1010);
		EXPECT_EQ(itr->getSearchResults()[99]->getDirection(), 0);
		//  var0 and var3 (Cont 1 and Ord 1)
		EXPECT_EQ(itr->getSearchResults()[200]->getRes(), x03_C11);
		EXPECT_EQ(itr->getSearchResults()[200]->getIndex(), x03c11);
		EXPECT_EQ(itr->getSearchResults()[200]->getDirection(), 3);
		EXPECT_EQ(itr->getSearchResults()[249]->getRes(), x03_C105);
		EXPECT_EQ(itr->getSearchResults()[249]->getIndex(), x03c105);
		EXPECT_EQ(itr->getSearchResults()[249]->getDirection(), 0);
		//  var1 and var7 (Cont 2 and Nom 2)
		EXPECT_EQ(itr->getSearchResults()[1240]->getRes(), x17_C10);
		EXPECT_EQ(itr->getSearchResults()[1240]->getIndex(), x17c10);
		EXPECT_EQ(itr->getSearchResults()[1240]->getDirection(), 2);
		EXPECT_EQ(itr->getSearchResults()[1399]->getRes(), x17_C1015);
		EXPECT_EQ(itr->getSearchResults()[1399]->getIndex(), x17c1015);
		EXPECT_EQ(itr->getSearchResults()[1399]->getDirection(), 0);
		//  var4 and var5 (Ord 2 and Ord 3)
		EXPECT_EQ(itr->getSearchResults()[2480]->getRes(), x45_C11);
		EXPECT_EQ(itr->getSearchResults()[2480]->getIndex(), x45c11);
		EXPECT_EQ(itr->getSearchResults()[2480]->getDirection(), 3);
		EXPECT_EQ(itr->getSearchResults()[2504]->getRes(), x45_C55);
		EXPECT_EQ(itr->getSearchResults()[2504]->getIndex(), x45c55);
		EXPECT_EQ(itr->getSearchResults()[2504]->getDirection(), 0);
		//  var5 and var8 (Ord 3 and Nom 3)
		EXPECT_EQ(itr->getSearchResults()[2905]->getRes(), x58_C10);
		EXPECT_EQ(itr->getSearchResults()[2905]->getIndex(), x58c10);
		EXPECT_EQ(itr->getSearchResults()[2905]->getDirection(), 2);
		EXPECT_EQ(itr->getSearchResults()[2984]->getRes(), x58_C515);
		EXPECT_EQ(itr->getSearchResults()[2984]->getIndex(), x58c515);
		EXPECT_EQ(itr->getSearchResults()[2984]->getDirection(), 0);
		//  var6 and var8 (Nom 1 and Nom 3) 
		EXPECT_EQ(itr->getSearchResults()[3241]->getRes(), x68_C00);
		EXPECT_EQ(itr->getSearchResults()[3241]->getIndex(), x68c10);
		EXPECT_EQ(itr->getSearchResults()[3241]->getDirection(), 1);
		EXPECT_EQ(itr->getSearchResults()[3496]->getRes(), x68_C1515);
		EXPECT_EQ(itr->getSearchResults()[3496]->getIndex(), x68c515);
		EXPECT_EQ(itr->getSearchResults()[3496]->getDirection(), 3);

		delete data;
		delete itr;
	}

	TEST(TwoDepthSearchTest, SimulationTestData) {
		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
		data->loadData();
		ITR* itr = new TwoDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), false);
		itr->process();

		delete data;
		delete itr;
	}

	TEST(ThreeDepthSearchTest, CSVTestData) {
		static const float x012_C111 = 59.4828634;
		static const float x012_C101010 = 56.8355874;
		static const float x345_C111 = 59.4451734;
		static const float x345_C555 = 56.8355874;
		static const float x678_C000 = 56.8355874;
		static const float x678_C555 = 57.5700714;

		static const vector<unsigned int> x012c111{ 0, 1, 1, 1, 2, 1 };
		static const vector<unsigned int> x012c101010{ 0, 10, 1, 10, 2, 10 };
		static const vector<unsigned int> x345c111{ 3, 1, 4, 1, 5, 1 };
		static const vector<unsigned int> x345c555{ 3, 5, 4, 5, 5, 5 };
		static const vector<unsigned int> x678c000{ 6, 0, 7, 0, 8, 0 };
		static const vector<unsigned int> x678c151515{ 6, 15, 7, 15, 8, 15 };

		Data* data = new LoadCSVData("TestSample_100.csv");
		data->loadData();

		ITR* itr = new ThreeDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), false);
		itr->process();

		// Begin testing
		vector<Res*> check = itr->getSearchResults();
		// Check for size
		EXPECT_EQ(check.size(), 86131);	
		//  var0, var1 and var2 (Cont 1, Cont 2 and Cont 3)
		EXPECT_EQ(check.at(0)->getRes(), x012_C111);
		EXPECT_EQ(check.at(0)->getIndex(), x012c111);
		EXPECT_EQ(check.at(0)->getDirection(), 2);
		EXPECT_EQ(check.at(999)->getRes(), x012_C101010);
		EXPECT_EQ(check.at(999)->getIndex(), x012c101010);
		EXPECT_EQ(check.at(999)->getDirection(), 0);
		//  var3, var4 and var5 (Ord 1, Ord 2 and Ord 3)
		EXPECT_EQ(check.at(66790)->getRes(), x345_C111);
		EXPECT_EQ(check.at(66790)->getIndex(), x345c111);
		EXPECT_EQ(check.at(66790)->getDirection(), 5);
		EXPECT_EQ(check.at(66914)->getRes(), x345_C555);
		EXPECT_EQ(check.at(66914)->getIndex(), x345c555);
		EXPECT_EQ(check.at(66914)->getDirection(), 0);
		//  var6, var7 and var8 (Nom 1, Nom 2 and Nom 3)
		EXPECT_EQ(check.at(82035)->getRes(), x678_C000);
		EXPECT_EQ(check.at(82035)->getIndex(), x678c000);
		EXPECT_EQ(check.at(82035)->getDirection(), 1);
		EXPECT_EQ(check.at(86130)->getRes(), x678_C555);
		EXPECT_EQ(check.at(86130)->getIndex(), x678c151515);
		EXPECT_EQ(check.at(86130)->getDirection(), 6);

		delete data;
		delete itr;
	}

	TEST(ThreeDepthSearchTest, SimulationTestData) {
		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
		data->loadData();

		ITR* itr = new ThreeDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), false);
		itr->process();

		delete data;
		delete itr;
	}

	TEST(FlexDepthSearchTest, CSVTestData_1D) {
		static const float res_x0 = 58.3572554;
		static const float res_x1 = 61.284578;
		static const float res_x2 = 56.8355874;
		static const float res_x3 = 56.8355874;
		static const float res_x4 = 56.8355874;
		static const float res_x5 = 58.0366794;
		static const float res_x6 = 60.2639354;
		static const float res_x7 = 58.7563414;
		static const float res_x8 = 58.6787314;

		static const vector<unsigned int> index_x0{ 0, 9 };
		static const vector<unsigned int> index_x1{ 1, 5 };
		static const vector<unsigned int> index_x2{ 2, 10 };
		static const vector<unsigned int> index_x3{ 3, 5 };
		static const vector<unsigned int> index_x4{ 4, 5 };
		static const vector<unsigned int> index_x5{ 5, 1 };
		static const vector<unsigned int> index_x6{ 6, 2 };
		static const vector<unsigned int> index_x7{ 7, 11 };
		static const vector<unsigned int> index_x8{ 8, 2 };

		Data* data = new LoadCSVData("TestSample_100.csv");
		data->loadData();

		ITR* itr = new FlexDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), 1, false);
		itr->process();

		// Begin testing
		vector<Res*> check = itr->getSearchResults();
		// Check for size
		EXPECT_EQ(check.size(), 9);
		// Check for value
		EXPECT_EQ(check.at(0)->getRes(), res_x0);
		EXPECT_EQ(check.at(0)->getIndex(), index_x0);
		EXPECT_EQ(check.at(0)->getDirection(), 0);
		EXPECT_EQ(check.at(1)->getRes(), res_x1);
		EXPECT_EQ(check.at(1)->getIndex(), index_x1);
		EXPECT_EQ(check.at(1)->getDirection(), 1);
		EXPECT_EQ(check.at(2)->getRes(), res_x2);
		EXPECT_EQ(check.at(2)->getIndex(), index_x2);
		EXPECT_EQ(check.at(2)->getDirection(), 0);
		EXPECT_EQ(check.at(3)->getRes(), res_x3);
		EXPECT_EQ(check.at(3)->getIndex(), index_x3);
		EXPECT_EQ(check.at(3)->getDirection(), 0);
		EXPECT_EQ(check.at(4)->getRes(), res_x4);
		EXPECT_EQ(check.at(4)->getIndex(), index_x4);
		EXPECT_EQ(check.at(4)->getDirection(), 0);
		EXPECT_EQ(check.at(5)->getRes(), res_x5);
		EXPECT_EQ(check.at(5)->getIndex(), index_x5);
		EXPECT_EQ(check.at(5)->getDirection(), 1);
		EXPECT_EQ(check.at(6)->getRes(), res_x6);
		EXPECT_EQ(check.at(6)->getIndex(), index_x6);
		EXPECT_EQ(check.at(6)->getDirection(), 1);
		EXPECT_EQ(check.at(5)->getDirection(), 1);
		EXPECT_EQ(check.at(7)->getRes(), res_x7);
		EXPECT_EQ(check.at(7)->getIndex(), index_x7);
		EXPECT_EQ(check.at(7)->getDirection(), 1);
		EXPECT_EQ(check.at(8)->getRes(), res_x8);
		EXPECT_EQ(check.at(8)->getIndex(), index_x8);
		EXPECT_EQ(check.at(8)->getDirection(), 1);

		delete data;
		delete itr;
	}

	TEST(FlexDepthSearchTest, CSVTestData_2D) {
		static const float res_x01 = 62.5633994;	// 0
		static const float res_x03 = 60.4943514;    // 2
		static const float res_x17 = 64.6361894;    // 13
		static const float res_x45 = 58.7436554;	// 26
		static const float res_x58 = 59.9727634;	// 32
		static const float res_x68 = 62.5289174;	// 34

		static const vector<unsigned int> index_x01{ 0, 8, 1, 1 };
		static const vector<unsigned int> index_x03{ 0, 9, 3, 1 };
		static const vector<unsigned int> index_x17{ 1, 5, 7, 13 };
		static const vector<unsigned int> index_x45{ 4, 4, 5, 1 };
		static const vector<unsigned int> index_x58{ 5, 1, 8, 15 };
		static const vector<unsigned int> index_x68{ 6, 8, 8, 2 };

		Data* data = new LoadCSVData("TestSample_100.csv");
		data->loadData();

		ITR* itr = new FlexDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), 2, false);
		itr->process();

		// Begin testing
		vector<Res*> check = itr->getSearchResults();
		// Check for size
		EXPECT_EQ(check.size(), 36);
		// Check for value
		EXPECT_EQ(check.at(0)->getRes(), res_x01);
		EXPECT_EQ(check.at(0)->getIndex(), index_x01);
		EXPECT_EQ(check.at(0)->getDirection(), 3);
		EXPECT_EQ(check.at(2)->getRes(), res_x03);
		EXPECT_EQ(check.at(2)->getIndex(), index_x03);
		EXPECT_EQ(check.at(2)->getDirection(), 0);
		EXPECT_EQ(check.at(13)->getRes(), res_x17);
		EXPECT_EQ(check.at(13)->getIndex(), index_x17);
		EXPECT_EQ(check.at(13)->getDirection(), 3);
		EXPECT_EQ(check.at(26)->getRes(), res_x45);
		EXPECT_EQ(check.at(26)->getIndex(), index_x45);
		EXPECT_EQ(check.at(26)->getDirection(), 1);
		EXPECT_EQ(check.at(32)->getRes(), res_x58);
		EXPECT_EQ(check.at(32)->getIndex(), index_x58);
		EXPECT_EQ(check.at(32)->getDirection(), 2);
		EXPECT_EQ(check.at(34)->getRes(), res_x68);
		EXPECT_EQ(check.at(34)->getIndex(), index_x68);
		EXPECT_EQ(check.at(34)->getDirection(), 1);

		delete data;
		delete itr;
	}

	TEST(FlexDepthSearchTest, CSVTestData_3D) {
		static const float res_x012 = 63.805154;	 // 0
		static const float res_x345 = 61.4430054;    // 64
		static const float res_x678 = 63.7606934;    // 83

		static const vector<unsigned int> index_x012{ 0, 10, 1, 5, 2, 2 };
		static const vector<unsigned int> index_x345{ 3, 1, 4, 4, 5, 4 };
		static const vector<unsigned int> index_x678{ 6, 5, 7, 14, 8, 8 };

		Data* data = new LoadCSVData("TestSample_100.csv");
		data->loadData();

		ITR* itr = new FlexDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), 3, false);
		itr->process();

		// Begin testing
		vector<Res*> check = itr->getSearchResults();
		// Check for size
		EXPECT_EQ(check.size(), 84);
		// Check for value
		EXPECT_EQ(check.at(0)->getRes(), res_x012);
		EXPECT_EQ(check.at(0)->getIndex(), index_x012);
		EXPECT_EQ(check.at(0)->getDirection(), 6);
		EXPECT_EQ(check.at(64)->getRes(), res_x345);
		EXPECT_EQ(check.at(64)->getIndex(), index_x345);
		EXPECT_EQ(check.at(64)->getDirection(), 5);
		EXPECT_EQ(check.at(83)->getRes(), res_x678);
		EXPECT_EQ(check.at(83)->getIndex(), index_x678);
		EXPECT_EQ(check.at(83)->getDirection(), 2);

		delete data;
		delete itr;
	}

	TEST(FlexDepthSearchTest, SimulatedTestData_1D) {
		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
		data->loadData();

		ITR* itr = new FlexDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), 1, false);
		itr->process();

		// Begin testing
		vector<Res*> check = itr->getSearchResults();
		// Check for size
		EXPECT_EQ(check.size(), 9);

		delete data;
		delete itr;
	}

	TEST(FlexDepthSearchTest, SimulatedTestData_3D) {
		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
		data->loadData();

		ITR* itr = new FlexDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), 2, false);
		itr->process();

		// Begin testing
		vector<Res*> check = itr->getSearchResults();
		// Check for size
		EXPECT_EQ(check.size(), 36);

		delete data;
		delete itr;
	}

	TEST(FlexDepthSearchTest, SimulatedTestData_2D) {
		Data* data = new CreateSimulationData({ 2,3,4 }, 200);
		data->loadData();

		ITR* itr = new FlexDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))), 3, false);
		itr->process();

		// Begin testing
		vector<Res*> check = itr->getSearchResults();
		// Check for size
		EXPECT_EQ(check.size(), 84);

		delete data;
		delete itr;
	}

}


#endif 
