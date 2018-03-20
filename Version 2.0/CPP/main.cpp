#include "Data.h"
#include "LoadSimpleTestData.h"
#include "LoadCSVData.h"
#include "CreateSimulationData.h"

#include "ITR.h"
#include "LoadData.h"
#include "CreateX.h"
#include "CreateLookUpTable.h"
#include "OneDepthSearch.h"
#include "TwoDepthSearch.h"
#include "ThreeDepthSearch.h"
#include "FlexDepthSearch.h"

#include <iostream>

using namespace std;





int main()
{
//    Data* data = new LoadSimpleTestData();
//    data->loadData();
//    utility::print1DVector(data1->getID());

    Data* data = new LoadCSVData("TestSample_100.csv");
    data->loadData();
//    utility::print1DVector(data2->getID());
//    utility::print1DVector(data2->getX_Type());
//    utility::print2DVector(data2->getActions());
//    utility::print2DVector(data2->getY());
//    utility::print2DVector(data2->getX_Cont());
//    utility::print2DVector(data2->getX_Ord());
//    utility::print2DVector(data2->getX_Nom());

//    Data* data3 = new CreateSimulationData({3,3,3},10);
//    data3->loadData();
//    utility::print1DVector(data3->getX_Type());
//    utility::print1DVector(data3->getID());
//    utility::print2DVector(data3->getActions());
//    utility::print2DVector(data3->getY());
//    utility::print2DVector(data3->getX_Cont());
//    utility::print2DVector(data3->getX_Ord());
//    utility::print2DVector(data3->getX_Nom());

//    ITR* itr_cs = new CreateX(data3);
//    itr_cs->runPreProcess();
//    utility::print1DVector(itr_cs->getData()->getX_Type());
//    utility::print2DVector(itr_cs->getData()->getX_Cont());
//    utility::print2DVector(itr_cs->getData()->getX_Ord());
//    utility::print2DVector(itr_cs->getData()->getX_Nom());
//    utility::print2DVector(itr_cs->getX());

//    ITR* itr_cs = new LoadData(data3);
//    ITR* itr_cs = new CreateX(new LoadData(data3));
//    ITR* itr_cs = new CreateLookUpTable(new CreateX(new LoadData(data)));
//    ITR* itr_cs = new OneDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data)), 1));
//    ITR* itr_cs = new TwoDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))),false);
//    ITR* itr_cs = new ThreeDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))),false);
    ITR* itr_cs = new FlexDepthSearch(new CreateLookUpTable(new CreateX(new LoadData(data))),3,true);

    itr_cs->process();
//    utility::print1DVector(itr_cs->getData()->getX_Type());
//    utility::print2DVector(itr_cs->getX());

    delete data;
    delete itr_cs;

    return 0;
}

