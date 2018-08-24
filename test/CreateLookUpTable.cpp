#include "pch.h"
#include "CreateLookUpTable.h"


CreateLookUpTable::CreateLookUpTable(ITR* itr, unsigned int index):IProcess(itr){
    CreateLookUpTable::actionIndex = index;
}

CreateLookUpTable::~CreateLookUpTable()
{
    //dtor
}

void CreateLookUpTable::process(){
    IProcess::process();
    cout<<"Create LookUp Table... "<<endl;

	CreateLookUpTable::sampleSize = ITR::getData()->getSampleSize();
	CreateLookUpTable::varSize = ITR::getVarType().size();
	CreateLookUpTable::actionSize = ITR::getData()->getActions().size();
	CreateLookUpTable::ySize = ITR::getData()->getY().size();

    init();
	info.clear();
    for(unsigned int i=0; i<varSize; ++i){
		info.push_back(new DataInfo(i, ITR::getVarType()[i], ITR::getX()[i]));
    }

    load_CutSize();
    load_Action(ITR::getData()->getActions());
    load_Y(ITR::getData()->getY());
    load_table_X(ITR::getX());
    ITR::T0 = sumTreatmentCal();
//	print_info();
//	print_Action();
//  print_Y();
//	print_CutSize();
}


void CreateLookUpTable::init_TableX()
{
    for(unsigned int i=0; i<varSize; ++i)
    {
        table_X[i] = new bool*[cutSize[i]] {};
        for(unsigned int j=0; j<cutSize[i]; ++j)
        {
            table_X[i][j] = new bool[sampleSize] {};
        }
    }
}

void CreateLookUpTable::init(){
    var_A = new unsigned int*[sampleSize] {};
    var_Y = new double*[sampleSize];
    table_X = new bool**[varSize] {};
    cutSize = new unsigned int[varSize] {};

    for(int unsigned i=0; i<sampleSize; ++i){
        var_A[i] = new unsigned int[actionSize] {};
        var_Y[i] = new double[ySize] {};
    }
}

void CreateLookUpTable :: load_CutSize()
{
    for(unsigned int i=0; i<varSize; ++i)
        cutSize[i] = info[i]->getCutSize();
}



void CreateLookUpTable :: load_Action(const vector<vector<unsigned int>>& a)
{
    for(unsigned int i=0; i<sampleSize; ++i)
    {
        for(unsigned int j=0; j<actionSize; ++j)
        {
            var_A[i][j] = (a[j][i] == CreateLookUpTable::actionIndex) ? 0 : 1;
        }
    }
}

void CreateLookUpTable :: load_Y(const vector<vector<double>>& y)
{
    for(unsigned int i=0; i<sampleSize; ++i)
    {
        for(unsigned int j=0; j<ySize; ++j)
        {
            var_Y[i][j] = y[j][i];
        }
    }
}


void CreateLookUpTable :: load_table_X(const vector<vector<unsigned int>> &data_X)
{
    init_TableX();
    for(unsigned int i=0; i<varSize; ++i)
    {
        for(unsigned int j=0; j<sampleSize; ++j)
        {
            for(unsigned int k=0; k<cutSize[i]; ++k)
            {
                table_X[i][k][j] = (ITR::getVarType()[i] == 2)?info[i]->nomContains(ITR::getX()[i][j],k):ITR::getX()[i][j] < info[i]->getRange(k);
            }
        }
    }
}

void CreateLookUpTable :: cleanAll()
{
    for(unsigned int i=0; i<sampleSize; ++i)
    {
        delete [] var_A[i];
        delete [] var_Y[i];
    }
    delete var_A;
    delete var_Y;

    for(unsigned int i=0; i<varSize; ++i)
    {
        for(unsigned int j=0; j<cutSize[i]; ++j)
        {
            delete [] table_X[i][j];
        }
        delete [] table_X[i];
    }
    delete table_X;
    delete [] cutSize;
}

double CreateLookUpTable :: sumTreatmentCal()
{
    double sum0 = 0.0;
    for(unsigned int i=0; i<sampleSize; ++i)
    {
        if(var_A[i][0] == 0)
        {
            sum0 += var_Y[i][0];
        }
    }
    return sum0;
}

void CreateLookUpTable::print_info() {
	cout << "Print private variables info:" << endl;
	cout << "Action Index: " << CreateLookUpTable::actionIndex << endl;
	cout << "Sample Size: " << CreateLookUpTable::sampleSize << endl;
	cout << "Variable Size: " << CreateLookUpTable::varSize << endl;
	cout << "Action Size: " << CreateLookUpTable::actionSize << endl;
	cout << "Y Size: " << CreateLookUpTable::ySize << endl;
}

void CreateLookUpTable::print_Action(){
	cout << "Print action matrix:" << endl;
	for (unsigned int i = 0; i<sampleSize; ++i)
	{
		for (unsigned int j = 0; j<actionSize; ++j)
		{
			cout << var_A[i][j] << ' ';
		}
		cout << endl;
	}
}

void CreateLookUpTable::print_Y(){
	cout << "Print Y matrix:" << endl;
	for (unsigned int i = 0; i<sampleSize; ++i)
	{
		for (unsigned int j = 0; j<ySize; ++j)
		{
			cout << var_Y[i][j] << ' ';
		}
		cout << endl;
	}
}

void CreateLookUpTable::print_CutSize()
{
	cout << "Cut range:" << endl;
	for (unsigned int i = 0; i<varSize; ++i)
		cout << cutSize[i] << ' ';
	cout << endl;
}