#include "DataGeneration.h"

DataGeneration :: DataGeneration(vector<int> const &vType, vector<double> const &rY, vector<int> const &rActions, vector<double> const &rCont, vector<int> const &rOrd, vector<int> const &rNom)
{
    sampleSize = 0;

    varType = vType;
    rangesY = rY;
    rangesActions = rActions;
    rangesCont = rCont;
    rangesOrd = rOrd;
    rangesNom = rNom;
    covariateSize = vType.size();
}

DataGeneration  :: ~DataGeneration()    {   }



void DataGeneration :: creatSamples(int sSize)
{
    sampleSize = sSize;
    id = patientID(sSize);
    vector<vector<int>> seeds = assignSeed(createSeed(getVariateSize(),100), varType);
    varCont = sampleGenerator(seeds.at(0),rangesCont);
    varOrd = sampleGenerator(seeds.at(1),rangesOrd);
    varNom = sampleGenerator(seeds.at(2),rangesNom);
    varY = sampleGenerator(createSeed(getYSize(),10),rangesY);
    actions = sampleGenerator(createSeed(getActionSize(),50),rangesActions);
    preprocessing();
}


void DataGeneration :: preprocessing()
{
    varContInt = percentileVec(varCont);
    dataSet = combineData(varContInt, varOrd, varNom);
    sumTreatmentCal();
}

vector<vector<int>> DataGeneration :: combineData(vector<vector<int>> const &varCont, vector<vector<int>> const &varOrd, vector<vector<int>> const &varNom)
{
    vector<vector<int>> dataSet;
    for(int i=0; i<getContVarSize() ; ++i) dataSet.push_back(varCont.at(i));
    for(int i=0; i<getOrdVarSize() ; ++i) dataSet.push_back(varOrd.at(i));
    for(int i=0; i<getNomVarSize() ; ++i) dataSet.push_back(varNom.at(i));
    return dataSet;
}

int DataGeneration :: getSampleSize()
{
    return sampleSize;
}
int DataGeneration :: getVariateSize()
{
    return covariateSize;
}
int DataGeneration :: getYSize()
{
    return getRangeSize(rangesY);
}
int DataGeneration :: getActionSize()
{
    return getRangeSize(rangesActions);
}
int DataGeneration :: getContVarSize()
{
    return getRangeSize(rangesCont);
}
int DataGeneration :: getOrdVarSize()
{
    return getRangeSize(rangesOrd);
}
int DataGeneration :: getNomVarSize()
{
    return getRangeSize(rangesNom);
}

vector<int> DataGeneration :: getVarType()
{
    return varType;
}
vector<vector<double>> DataGeneration :: getY()
{
    return varY;
}
vector<vector<int>> DataGeneration :: getActions()
{
    return actions;
}
vector<vector<double>> DataGeneration :: getVarCont()
{
    return varCont;
}
vector<vector<int>> DataGeneration :: getVarCont(int i)
{
    return varContInt;
}
vector<vector<int>> DataGeneration :: getVarOrd()
{
    return varOrd;
}
vector<vector<int>> DataGeneration :: getVarNom()
{
    return varNom;
}
vector<vector<int>> DataGeneration :: getDataSet()
{
    return dataSet;
}
vector<int> DataGeneration :: getID()
{
    return id;
}

void DataGeneration :: printContVar()
{
    cout<<'\n'<<getContVarSize()<<" continuous variables, "<<getSampleSize()<<" samples:"<<endl;
    print2DVector(varCont);
}
void DataGeneration :: printContVar(int i)
{
    cout<<'\n'<<getContVarSize()<<" percentile continuous variables, "<<getSampleSize()<<" samples:"<<endl;
    print2DVector(varContInt);
}
void DataGeneration :: printOrdVar()
{
    cout<<'\n'<<getOrdVarSize()<<" ordinal variables, "<<getSampleSize()<<" samples:"<<endl;
    print2DVector(varOrd);
}
void DataGeneration :: printNomVar()
{
    cout<<'\n'<<getNomVarSize()<<" nominal variables, "<<getSampleSize()<<" samples:"<<endl;
    print2DVector(varNom);
}
void DataGeneration :: printY()
{
    cout<<'\n'<<getYSize()<<" Y , "<<getSampleSize()<<" samples:"<<endl;
    print2DVector(varY);
}
void DataGeneration :: printAction()
{
    cout<<'\n'<<getActionSize()<<" actions , "<<getSampleSize()<<" samples:"<<endl;
    print2DVector(actions);
}
void DataGeneration :: printDataSet()
{
    cout<<'\n'<<getVariateSize()<<" covariates , "<<getSampleSize()<<" samples:"<<endl;
    print2DVector(dataSet);
}

void DataGeneration :: printInfo(int i)
{
    switch(i)
    {
    case 0:
        printContVar();
        break;
    case 1:
        printOrdVar();
        break;
    case 2:
        printNomVar();
        break;
    case 3:
        printAction();
        break;
    case 4:
        printY();
        break;
    case 5:
        printContVar(5);
        break;
    case 6:
        printDataSet();
        break;
    default:
        cout<<"You typed something wrong!";
    }
}

void DataGeneration :: genReport()
{
    cout<<"Generate "<<getSampleSize()<<" samples\n"
        <<"Covariate size: "<<getVariateSize()
        <<"\tY size: "<<getYSize()
        <<"\tAction size: "<<getActionSize()<<'\n'
        <<"Continuous: "<<getContVarSize()
        <<"\tOrdinal: "<<getOrdVarSize()
        <<"\tNormal: "<<getNomVarSize()<<endl;
}


vector<int> DataGeneration :: patientID(int noSample)
{
    vector<int> vectOut;
    if(noSample>0)
    {
        for(int i=0; i<noSample; ++i)
        {
            vectOut.push_back(i);
        }
    }
    else
    {
        cout<<"No. of sample much be >0"<<endl;
        return vectOut;
    }
    return vectOut;
}

template<class T>
int DataGeneration :: getRangeSize(vector<T> const &vectIn)
{
    int vectSize = vectIn.size();
    if(vectSize%2 == 0)
    {
        for(int i=0 ; i < vectSize; ++i)
        {
            T lowBound = vectIn.at(2*i);
            T upperBound = vectIn.at(2*i+1);
            if(lowBound < upperBound)
            {
                return vectSize/2;
            }
            else
            {
                cout<<"Error: Please check if lower bounds are smaller than upper bounds. \n";
            }
        }
    }
    else
    {
        cout<<"Error: Please check for complete ranges input. \n";
    }
    return 0;
}

vector<int> DataGeneration :: createSeed(int varsize, int start)
{
    vector<int> seed;
    for(int i=0 ; i<varsize; ++i)
    {
        seed.push_back(start+5*i);
    }
    return seed;
}




vector<vector<int>> DataGeneration :: assignSeed(vector<int> const &seed, vector<int> const &varType)
{
    vector<vector<int>> vectOut;
    vector<int> seedCont;
    vector<int> SeedOrd;
    vector<int> seedNom;
    for(int i=0; i<getVariateSize(); ++i)
    {
        int type = varType.at(i);
        if(type == 0)
        {
            seedCont.push_back(seed.at(i));
        }
        else if(type == 1)
        {
            SeedOrd.push_back(seed.at(i));
        }
        else if(type == 2)
        {
            seedNom.push_back(seed.at(i));
        }
        else
        {
            cout<<"Error about variable type input."<<endl;
        }
    }
    vectOut.push_back(seedCont);
    vectOut.push_back(SeedOrd);
    vectOut.push_back(seedNom);
    return vectOut;
}

vector<double> DataGeneration :: dataGenerator(int seed, double lowerBound, double upperBound){
    default_random_engine rvEngine{seed};
    mt19937 gen(rvEngine());
    uniform_real_distribution<> dis(lowerBound, upperBound);
    vector<double> vectOut;
    for(int i=0;i<sampleSize;++i){
        vectOut.push_back(dis(gen));
    }
    return vectOut;
}

vector<int> DataGeneration :: dataGenerator(int seed, int lowerBound, int upperBound){
    default_random_engine rvEngine{seed};
    mt19937 gen(rvEngine());
    uniform_int_distribution<> dis(lowerBound, upperBound);
    vector<int> vectOut;
    for(int i=0;i<sampleSize;++i){
        vectOut.push_back(dis(gen));
    }
    return vectOut;
}

template<class T>
vector<vector<T>> DataGeneration :: sampleGenerator(vector<int> const &seed, vector<T> const &ranges){
    vector<vector<T>> vectOut;
    for(int  i=0 ; i < getRangeSize(ranges); ++i)
    {
        T lowBound = ranges[2*i];
        T upperBound = ranges[2*i+1];
        vector<T>   vectTemp = dataGenerator(seed[i],lowBound,upperBound);
        vectOut.push_back(vectTemp);
    }
    return vectOut;
}


vector<int> DataGeneration :: percentileVec(vector<double> const &vectIn)
{
    vector<int> vectOut;
    map<double, int> mapTemp = percentileMap(vectIn);
    for(const auto &row : vectIn)
    {
        vectOut.push_back(mapTemp[row]);
    }
    return vectOut;
}

vector<vector<int>> DataGeneration :: percentileVec(vector<vector<double>> const &vectIn)
{
    vector<vector<int>> vectOut;
    for(auto x : vectIn)
    {
        vectOut.push_back(percentileVec(x));
    }
    return vectOut;
}

map<double, int> DataGeneration :: percentileMap(vector<double> const &vectorIn)
{
    map<double, int> mapOut;
    vector<double> vectorSort = vectorIn;
    sort(vectorSort.begin(),vectorSort.end());
    int len = vectorSort.size();
    for(int i=0; i<len ; ++i)
        mapOut.insert(make_pair(vectorSort.at(i),assignPercentile(percentile(len,i+1))));
    return mapOut;
}

double DataGeneration :: percentile(double len,double index)
{
    if(index>=0)
    {
        return 100*(index-0.5)/len;
    }
    else
    {
        cout<<"Out of scope"<<endl;
        return 0.0;
    }
}

int DataGeneration :: assignPercentile(double p)
{
    if (p < 0)
    {
        cout<<"Out of scope"<<endl;
        return -1;
    }
    else if (p < 10)
    {
        return 0;
    }
    else if (p < 20)
    {
        return 1;
    }
    else if (p < 30)
    {
        return 2;
    }
    else if (p < 40)
    {
        return 3;
    }
    else if (p < 50)
    {
        return 4;
    }
    else if (p < 60)
    {
        return 5;
    }
    else if (p < 70)
    {
        return 6;
    }
    else if (p < 80)
    {
        return 7;
    }
    else if (p < 90)
    {
        return 8;
    }
    else
    {
        return 9;
    }
}



void DataGeneration :: sumTreatmentCal()
{
    double sum0 = 0.0;
    for(int i=0; i<sampleSize; ++i)
    {
        if(actions[0][i] != 1)
        {
            sum0 += varY[0][i];
        }
    }
    sumTreatment0 = sum0;
}


double DataGeneration :: getSumT0()
{
    return sumTreatment0;
}




template<class T>
void DataGeneration :: print1DVector(vector<T> const &vectIn)
{
    cout<<"[ ";
    for(const auto &row : vectIn)
    {
        cout<<row<<' ';
    }
    cout<<']';
}

template<class T>
void DataGeneration :: print2DVector(vector<vector<T>> const &vectIn)
{
    try
    {
        for(unsigned int j=0; j<vectIn.at(0).size(); ++j)
        {
            for(unsigned int i=0; i<vectIn.size(); ++i)
            {
                cout<<vectIn[i][j]<<' ';
            }
            cout<<endl;
        }
    }
    catch (const std::out_of_range& oor)
    {
        cerr << "Out of Range error in print2DVector. Please check empty input: " << oor.what() << '\n';
    }
}
