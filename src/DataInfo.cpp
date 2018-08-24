#include "pch.h"
#include "DataInfo.h"

DataInfo::DataInfo(unsigned int no, unsigned int type, vector<unsigned int> dataSetColumn) {
	this->varNo = no;
	this->varType = type;
	vector<unsigned int> rangeCont{ 1,2,3,4,5,6,7,8,9,10 };

	if (type == 2)
	{
		nomCuts = createSets(uniqueValues(dataSetColumn));
		range = createNomRange(nomCuts.size());
	}
	else if (type == 1)
	{
		range = createOrdRange(uniqueValues(dataSetColumn));
	}
	else if (type == 0)
	{
		range = rangeCont;
	}
	else
	{
		cout << "\n Incorrect input for data type.\n";
	}
}

DataInfo::~DataInfo()
{
    //dtor
}



unsigned int DataInfo:: getVarNo()
{
    return varNo;
}

unsigned int DataInfo:: getVarType()
{
    return varType;
}

bool DataInfo:: nomContains(unsigned int x,unsigned int index)
{
    return nomCuts[index].count(x)?true:false;
}

unsigned int DataInfo:: getCutSize()
{
    return (varType == 2)?nomCuts.size():range.size();
}

void DataInfo:: printSet(unsigned int i)
{
    cout<<"< ";
    for(auto it = nomCuts[i].cbegin(); it!=nomCuts[i].cend(); ++it)
    {
        cout<<*it<<' ';
    }
    cout<<">\t";
}

void DataInfo:: printVarInfo()
{
    cout<<"Var: "<<varNo<<"\tType: "<<varType<<endl;
    cout<<"Available cuts:\n";
    if(varType==2)
    {
        for(size_t i=0; i<nomCuts.size(); ++i)
        {
            cout<<i<<"::\t< ";
            for(set<unsigned int>::iterator it = nomCuts[i].begin(); it!=nomCuts[i].end(); ++it)
            {
                cout<<*it<<' ';
            }
            cout<<'>'<<endl;
        }
    }
    else
    {
        for(size_t i=0; i<range.size(); ++i)
        {
            cout<<range[i]<<' ';
        }
        cout<<endl;
    }
}


/// Return vector of unique values of a int vector
vector<unsigned int> DataInfo :: uniqueValues(vector<unsigned int> &vectorIn)
{
    if(!vectorIn.empty())
    {
        sort(vectorIn.begin(), vectorIn.end());
        auto last = unique(vectorIn.begin(), vectorIn.end());
        vectorIn.erase(last, vectorIn.end());
    }
    else
    {
        cout<<"Warning: Empty vector detected. Please initialize vector."<<endl;
    }
    return vectorIn;
}





vector<set<unsigned int>> DataInfo :: createSets(vector<unsigned int> setIn)
{
    vector<set<unsigned int>> out;

    unsigned int max = 1 << setIn.size();
    for(unsigned int i = 0; i < max; i++)
    {
        set<unsigned int> Second;

        int j = i;
        int index = 0;
        while (j > 0)
        {
            if((j & 1) > 0)
                Second.insert(setIn[index]);
            j >>= 1;
            index++;
        }
        if(Second.size()<=setIn.size()/2)
        {
            out.push_back(Second);
        }
    }
    return out;
}


vector<unsigned int> DataInfo :: createNomRange(unsigned int x)
{
    vector<unsigned int> out;
    for(unsigned int i=0; i<x; ++i)
    {
        out.push_back(i);
    }
    return out;
}


vector<unsigned int> DataInfo :: createOrdRange(vector<unsigned int> v)
{
    for(auto x=begin(v); x!=end(v); ++x)
    {
        *x +=1;
    }
    return v;
}
