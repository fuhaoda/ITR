#ifndef DATAINFO_H
#define DATAINFO_H

#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

using namespace std;

class DataInfo
{
    public:
        DataInfo(unsigned int no, unsigned int type, vector<unsigned int> dataSetColumn);
        ~DataInfo();

        unsigned int getVarNo();
        unsigned int getVarType();
        unsigned int getCutSize();
        bool nomContains(unsigned int x,unsigned int index);         /**< check if 'x' contains in set 'index'*/
		vector<unsigned int> getRange() { return range; };
        unsigned int getRange(unsigned int i){ return range[i];};
        void printVarInfo();
        void printSet(unsigned int i);

    private:
		unsigned int varNo;
		unsigned int varType;
		vector<unsigned int> range;
		vector<set<unsigned int>> nomCuts;

        vector<unsigned int> uniqueValues(vector<unsigned int> &vectorIn);
        vector<set<unsigned int>> createSets(vector<unsigned int> vectorIn);
        vector<unsigned int> createNomRange(unsigned int i);
        vector<unsigned int> createOrdRange(vector<unsigned int> x);
};

#endif // DATAINFO_H
