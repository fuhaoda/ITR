#include "pch.h"
#include "ITR.h"

Data* ITR::mydata;
vector<unsigned int> ITR::var_Type;
vector<vector<unsigned int>> ITR::var_X;
unsigned int** ITR::var_A;
double** ITR::var_Y;
bool*** ITR::table_X;
unsigned int* ITR::cutSize;
vector<Res *> ITR::searchResults;
double ITR::T0;
vector<DataInfo *> ITR::info;


ITR::ITR()
{
    //ctor
}

ITR::~ITR()
{
    //dtor
}


void ITR::setData(Data* data) {
	ITR::mydata = data;
}

void ITR::setVarType(vector<unsigned int> vectIn) {
	ITR::var_Type.clear();
	ITR::var_Type = vectIn;
}

void ITR::setX(vector<vector<unsigned int>> vectIn) {
	ITR::var_X.clear();
	ITR::var_X = vectIn;
}

Data* ITR::getData(){
    return ITR::mydata;
}

vector<vector<unsigned int>>& ITR::getX(){
    return ITR::var_X;
}

double ITR::getT0(){
    return ITR::T0;
}

vector<unsigned int> ITR::getVarType(){
    return ITR::var_Type;
}

vector<DataInfo *> ITR::getDataInfo() {
	return ITR::info;
}

vector<Res *> ITR::getSearchResults() {
	return ITR::searchResults;
}