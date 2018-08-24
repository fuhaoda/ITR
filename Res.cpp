#include "Res.h"

Res::Res(double res, vector<int> ind, int dir)
{
    result = (float)res;
    index = &ind;
    direction = dir;
}

Res::~Res()
{
    //dtor
}
