#ifndef DATAGENERATION_H
#define DATAGENERATION_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>

using namespace std;

class DataGeneration
{
    private:
        /// Settings for generate data
        int sampleSize;
        int covariateSize;

        vector<int> varType; // 0 for cont, 1 for ordinal, 2 for nominal
        vector<double> rangesY;
        vector<int> rangesActions;
        vector<double> rangesCont;
        vector<int> rangesOrd;
        vector<int> rangesNom;

        /// Generated raw data
        vector<int> id;
        vector<vector<double>> varY;
        vector<vector<int>> actions;
        vector<vector<double>> varCont;
        vector<vector<int>> varOrd;
        vector<vector<int>> varNom;

         /// Processed data
        vector<vector<int> > varContInt;
        vector<vector<int> > dataSet;
        double sumTreatment0;

    public:
        DataGeneration(vector<int> const &vType, vector<double> const &rY, vector<int> const &rActions, vector<double> const &rCont, vector<int> const &rOrd, vector<int> const &rNom);
        ~DataGeneration();
        void creatSamples(int sSize);

        int getSampleSize();
        int getVariateSize();
        int getYSize();
        int getActionSize();
        int getContVarSize();
        int getOrdVarSize();
        int getNomVarSize();

        vector<int> getVarType();
        vector<vector<double>> getY();
        vector<vector<int>> getActions();
        vector<vector<double>> getVarCont();
        vector<vector<int>> getVarCont(int i);
        vector<vector<int>> getVarOrd();
        vector<vector<int>> getVarNom();
        vector<vector<int>> getDataSet();
        vector<int> getID();
        double getSumT0();

        void printY();
        void printAction();
        void printContVar();
        void printContVar(int i);
        void printOrdVar();
        void printNomVar();
        void printDataSet();
        void printInfo(int i);
        void genReport();

    private:
        void preprocessing();
        vector<vector<int>> combineData(vector<vector<int>> const &varCont, vector<vector<int>> const &varOrd, vector<vector<int>> const &varNom);
        vector<int> patientID(int noSample);
        template<class T> int getRangeSize(vector<T> const &vectIn);
        vector<int> createSeed(int varsize, int start);
        vector<vector<int>> assignSeed(vector<int> const &seed, vector<int> const &varType);
        vector<double> dataGenerator(int seed, double lowerBound, double upperBound);
        vector<int> dataGenerator(int seed, int lowerBound, int upperBound);
        template<class T> vector<vector<T>> sampleGenerator(vector<int> const &seed, vector<T> const &ranges);

        vector<int> percentileVec(vector<double> const &vectIn);
        vector<vector<int>> percentileVec(vector<vector<double>> const &vectIn);
        map<double, int> percentileMap(vector<double> const &vectorIn);
        double percentile(double len,double index);
        int assignPercentile(double p);

        void sumTreatmentCal();

        template<class T>   void print1DVector(vector<T> const &vectIn);
        template<class T>   void print2DVector(vector<vector<T>> const &vectIn);
};

#endif // DATAGENERATION_H

