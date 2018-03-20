#include "pch.h"
#include "Data.h"

Data::Data()
{
    //ctor
}

Data::~Data()
{
    //dtor
}

const vector<unsigned int>& Data :: getID()
{
    return id;
}

const vector<vector<double>>& Data :: getY()
{
    return y;
}

const vector<vector<unsigned int>>& Data :: getActions()
{
    return actions;
}


const vector<vector<double>>& Data :: getX_Cont()
{
    return x_Cont;
}

const vector<vector<unsigned int>>& Data :: getX_Ord()
{
    return x_Ord;
}

const vector<vector<unsigned int>>& Data :: getX_Nom()
{
    return x_Nom;
}

const vector<unsigned int>& Data::getX_Type()
{
    return dataType;
}

unsigned int Data::getSampleSize(){
    return sampleSize;
}

void Data::printID()
{
    utility::print1DVector(id);
}

void Data::printY()
{
    utility::print2DVector(y);
}

void Data::printActions()
{
    utility::print2DVector(actions);
}

void Data::printX_Cont()
{
    utility::print2DVector(x_Cont);
}

void Data::printX_Ord()
{
    utility::print2DVector(x_Ord);
}

void Data::printX_Nom()
{
    utility::print2DVector(x_Nom);
}

void Data::printX_Type()
{
    utility::print1DVector(dataType);
}
