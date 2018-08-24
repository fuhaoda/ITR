#include "DataInfo.h"

void DataInfo::load_DataInfo(int no, int type, vector<int> &dataSetColumn)
{
    varNo = no;
    varType = type;

    if(type==2)
    {
        nomCuts = createSets(uniqueValues(dataSetColumn));
        range = createNomRange(nomCuts.size());
    }
    else if(type==1)
    {
        range = createOrdRange(uniqueValues(dataSetColumn));
    }
    else if(type==0)
    {
        range = {1,2,3,4,5,6,7,8,9,10};
    }
    else
    {
        cout<<"\n Incorrect input for data type.\n";
    }

}

DataInfo::~DataInfo()
{
    //dtor
}



int DataInfo:: getVarNo()
{
    return varNo;
}

int DataInfo:: getVarType()
{
    return varType;
}

bool DataInfo:: nomContains(int x,int index)
{
    return nomCuts[index].count(x)?true:false;
}
int DataInfo:: getCutSize()
{
    return (varType == 2)?nomCuts.size():range.size();
}

void DataInfo:: printSet(int i)
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
            for(set<int>::iterator it = nomCuts[i].begin(); it!=nomCuts[i].end(); ++it)
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
vector<int> DataInfo :: uniqueValues(vector<int> &vectorIn)
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





vector<set<int>> DataInfo :: createSets(vector<int> setIn)
{
    vector<set<int>> out;

    int max = 1 << setIn.size();
    for(int i = 0; i < max; i++)
    {
        set<int> Second;

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


vector<int> DataInfo :: createNomRange(int x)
{
    vector<int> out;
    for(int i=0; i<x; ++i)
    {
        out.push_back(i);
    }
    return out;
}


vector<int> DataInfo :: createOrdRange(vector<int> v)
{
    for(auto x=begin(v); x!=end(v); ++x)
    {
        *x +=1;
    }
    return v;
}
