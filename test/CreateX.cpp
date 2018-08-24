#include "pch.h"
#include "CreateX.h"

CreateX::~CreateX()
{
    //dtor
}

void CreateX::process(){
    IProcess::process();
    cout<<"Create X..."<<endl;
	ITR::setVarType(ITR::getData()->getX_Type());
    ITR::setX(combineData());
}

vector<vector<unsigned int>> CreateX :: percentileVec(vector<vector<double>> const &vectIn)
{
    vector<vector<unsigned int>> vectOut;
    for(auto x : vectIn)
    {
        vectOut.push_back(percentileVec(x));
    }
    return vectOut;
}

vector<unsigned int> CreateX :: percentileVec(vector<double> const &vectIn)
{
    vector<unsigned int> vectOut;
    std::map<double, unsigned int> mapTemp = percentileMap(vectIn);
    for(const auto &row : vectIn)
    {
        vectOut.push_back(mapTemp[row]);
    }
    return vectOut;
}

std::map<double, unsigned int> CreateX :: percentileMap(vector<double> const &vectorIn)
{
    std::map<double, unsigned int> mapOut;
    vector<double> vectorSort = vectorIn;
    sort(vectorSort.begin(),vectorSort.end());
    unsigned int len = vectorSort.size();
    for(unsigned int i=0; i<len ; ++i)
        mapOut.insert(make_pair(vectorSort.at(i),assignPercentile(percentile(len,i+1))));
    return mapOut;
}


double CreateX :: percentile(double len,double index)
{
    if(index>=0)
    {
        return 100*(index-0.5)/len;
    }
    else
    {
        cout<<"Out of scope"<<endl;
        return 0.0;
    }
}

unsigned int CreateX :: assignPercentile(double p)
{
    if (p < 0)
    {
        cout<<"Out of scope"<<endl;
        return -1;
    }
    else if (p < 10)
    {
        return 0;
    }
    else if (p < 20)
    {
        return 1;
    }
    else if (p < 30)
    {
        return 2;
    }
    else if (p < 40)
    {
        return 3;
    }
    else if (p < 50)
    {
        return 4;
    }
    else if (p < 60)
    {
        return 5;
    }
    else if (p < 70)
    {
        return 6;
    }
    else if (p < 80)
    {
        return 7;
    }
    else if (p < 90)
    {
        return 8;
    }
    else
    {
        return 9;
    }
}

vector<vector<unsigned int>> CreateX :: combineData(){
    vector<vector<unsigned int>> x_ContInt;
    vector<vector<unsigned int>> vectOut;
    if(ITR::getVarType()[0] == 0) x_ContInt = percentileVec(ITR::getData()->getX_Cont());
    unsigned int i = 0,j = 0,k = 0;
    for(const auto &x : ITR::getData()->getX_Type()){
        if(x == 0){
            vectOut.push_back(x_ContInt[i]);
            ++i;
        }else if(x == 1){
            vectOut.push_back(ITR::getData()->getX_Ord()[j]);
            ++j;
        }else{
            vectOut.push_back(ITR::getData()->getX_Nom()[k]);
            ++k;
        }
    }
    return vectOut;
}

