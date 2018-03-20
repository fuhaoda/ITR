/**
 * @file main.cpp
 * @brief Test of ITR comprehensive search implementation
 *        Test 1: fixed search depth = 3
 *        Test 2: flexible search depth
 * @author Jie Xue  <xue_jie@lilly.com>
 * @version 1.0
 * @date 2018-02-12
 */

#include <iostream>
#include <omp.h>

#include "DataGeneration.h"
#include "ITR.h"
#include "ThreeDepthSearch.h"
#include "FlexDepthSearch.h"

using namespace std;

int main()
{
    // Test data generation class
    vector<int> varType(25,0); // 0 for cont, 1 for ordinal, 2 for nominal
    vector<int> varType1(5,1);
    vector<int> varType2(5,2);
    varType.insert(varType.end(),varType1.begin(),varType1.end());
    varType.insert(varType.end(),varType2.begin(),varType2.end());

    vector<double> rangesCont = {50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0,0.0,100.0,10.0,200.0,50.0,300.0};
    vector<int> rangesOrd = {0,4,0,4,0,4,0,4,0,4};
    vector<int> rangesNom = {0,4,0,4,0,4,0,4,0,4};
    vector<double> rangesY = {0.0,100.0};
    vector<int> rangesAction = {0,1};

    DataGeneration data(varType,rangesY,rangesAction,rangesCont,rangesOrd,rangesNom);
    data.creatSamples(3000);

    ITR* ITR_CS = new ThreeDepthSearch(data,3);
//    (ITR_CS->get_DataInfo()[31]).printVarInfo();
//    ITR_CS->get_DataInfo()[31].
//    ITR_CS->searchPrint();
//    ITR_CS->searchOutput();
//    ITR_CS = new FlexDepthSearch(data,3);
//    ITR_CS->searchPrint();
//    ITR_CS->searchOutput();
}



