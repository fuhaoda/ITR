#include "pch.h"
#include "LoadData.h"

LoadData::LoadData(Data *data):m_data(data)
{
    //ctor
}

LoadData::~LoadData()
{
    //dtor
}

void LoadData::process(){
    cout<<"Load Data..."<<endl;
    ITR::setData(m_data);
}
