#include "pch.h"
#include "Res.h"

Res::Res(double res, vector<unsigned int> ind, int dir)
{
    result = (float)res;
    index = ind;
    direction = dir;
}

Res::~Res()
{
    //dtor
}
