#ifndef CREATEX_H
#define CREATEX_H

#include "IProcess.h"
#include <map>
#include <algorithm>

class CreateX : public IProcess
{
    public:
        CreateX(ITR* itr):IProcess(itr){};
        virtual ~CreateX();
        void process();

    private:
        vector<vector<unsigned int>> percentileVec(vector<vector<double>> const &vectIn);
        vector<unsigned int> percentileVec(vector<double> const &vectIn);
        std::map<double, unsigned int> percentileMap(vector<double> const &vectorIn);
        double percentile(double len,double index);
        unsigned int assignPercentile(double p);
        vector<vector<unsigned int>> combineData();
};

#endif // CREATEX_H
