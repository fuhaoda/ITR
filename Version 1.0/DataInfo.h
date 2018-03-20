#ifndef DATAINFO_H
#define DATAINFO_H

#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

using namespace std;

class DataInfo
{
    private:
        int varNo;
        int varType;
        vector<int> range;
        vector<set<int>> nomCuts;

    public:
        DataInfo(void){};
        ~DataInfo();
        void load_DataInfo(int no, int type, vector<int> &dataSetColumn);

        int getVarNo();
        int getVarType();
        int getCutSize();
        bool nomContains(int x,int index);
        int getRange(int i){ return range[i];};
        void printVarInfo();
        void printSet(int i);

    private:
        vector<int> uniqueValues(vector<int> &vectorIn);
        vector<set<int>> createSets(vector<int> vectorIn);
        vector<int> createNomRange(int i);
        vector<int> createOrdRange(vector<int> x);
};

#endif // DATAINFO_H
