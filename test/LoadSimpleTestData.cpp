#include "pch.h"
#include "LoadSimpleTestData.h"

LoadSimpleTestData::LoadSimpleTestData()
{
    //ctor
}

LoadSimpleTestData::~LoadSimpleTestData()
{
    //dtor
}

void LoadSimpleTestData::loadData(){
    vector<unsigned int> test_ID{1,2,3,4,5,6,7,8,9,10};
    vector<vector<double>> test_Y{{ 1,2,3,4,5,3,3,3,3,3}};
    vector<vector<unsigned int>> test_Actions{{1,1,1,1,1,2,2,2,2,2}};
    vector<vector<double>> test_X_Cont{{}};
    vector<vector<unsigned int>> test_X_Ord{{1,2,3,4,5,1,2,3,4,5}};
    vector<vector<unsigned int>> test_X_Nom{{}};
    vector<unsigned int> test_DataType{1};

    Data::id = test_ID;
    Data::y = test_Y;
    Data::actions = test_Actions;
    Data::x_Cont = test_X_Cont;
    Data::x_Ord = test_X_Ord;
    Data::x_Nom = test_X_Nom;
    Data::dataType = test_DataType;
    Data::sampleSize = 10;
}
