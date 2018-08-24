#ifndef CREATESIMULATIONDATA_H
#define CREATESIMULATIONDATA_H

#include "Data.h"

class CreateSimulationData : public Data
{
    public:
        CreateSimulationData(vector<int> xCount, int sampleS);
        virtual ~CreateSimulationData();

        void loadData();

    private:
        void createID();
        void createActions();
        void createY();
        void createX();

        vector<unsigned int> createSeed();
        vector<double> createDouble(unsigned int seed, double lowerBound = 0.0, double upperBound = 100.0);
        vector<unsigned int> createInt(unsigned int seed, unsigned int lowerBound = 0, unsigned int upperBound = 4);
};

#endif // CREATESIMULATIONDATA_H
