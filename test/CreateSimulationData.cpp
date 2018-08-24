#include "pch.h"
#include "CreateSimulationData.h"
#include <algorithm>
#include <random>

CreateSimulationData::CreateSimulationData(vector<int> xCount, int sampleS)
{
    Data::sampleSize = sampleS;
    unsigned int i = 0;
    Data::dataType.clear();
    while(i < xCount.size())
    {
        if(xCount[i] == 0)
        {
            ++i;
        }
        else
        {
            dataType.push_back(i);
            --xCount[i];
        }
    }
}

CreateSimulationData::~CreateSimulationData()
{
    //dtor
}


void CreateSimulationData::loadData()
{
    createID();
    createActions();
    createY();
    createX();
}


void CreateSimulationData::createID()
{
    Data::id.clear();
    if(sampleSize>0){
        for(unsigned int i=1; i<=sampleSize; ++i)
        {
            id.push_back(i);
        }
    }
}

void CreateSimulationData::createActions(){
    Data::actions.clear();
    Data::actions.push_back(createInt(50,0,1));
}

void CreateSimulationData::createY(){
    Data::y.clear();
    Data::y.push_back(createDouble(10,0.0,100));
}

void CreateSimulationData::createX(){
    Data::x_Cont.clear();
    Data::x_Ord.clear();
    Data::x_Nom.clear();
    vector<unsigned int> seed = createSeed();
    for(unsigned int i = 0; i<Data::dataType.size(); ++i){
        if(dataType[i] == 0){
            Data::x_Cont.push_back(createDouble(seed[i]));
        }else if(dataType[i] == 1){
            Data::x_Ord.push_back(createInt(seed[i]));
        }else if(dataType[i] == 2){
            Data::x_Nom.push_back(createInt(seed[i]));
        }else{
            cout<<"Wrong setting for simulation data. "<<endl;
        }
    }
}

vector<unsigned int> CreateSimulationData :: createSeed()
{
    vector<unsigned int> seed;
    for(unsigned int i=0 ; i<dataType.size(); ++i)
    {
        seed.push_back(5*i);
    }
    return seed;
}

vector<double> CreateSimulationData :: createDouble(unsigned int seed, double lowerBound, double upperBound){
    default_random_engine rvEngine{seed};
    mt19937 gen(rvEngine());
    uniform_real_distribution<> dis(lowerBound, upperBound);
    vector<double> vectOut;
    for(unsigned int i=0;i<sampleSize;++i){
        vectOut.push_back(dis(gen));
    }
    return vectOut;
}

vector<unsigned int> CreateSimulationData :: createInt(unsigned int seed, unsigned int lowerBound, unsigned int upperBound){
    default_random_engine rvEngine{seed};
    mt19937 gen(rvEngine());
    uniform_int_distribution<> dis(lowerBound, upperBound);
    vector<unsigned int> vectOut;
    for(unsigned int i=0;i<sampleSize;++i){
        vectOut.push_back(dis(gen));
    }
    return vectOut;
}
