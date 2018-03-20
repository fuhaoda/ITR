#include "pch.h"
#include "TwoDepthSearch.h"

TwoDepthSearch::TwoDepthSearch(ITR* itr, bool print) :IProcess(itr)
{
    TwoDepthSearch::isPrint = print;
}

TwoDepthSearch::~TwoDepthSearch()
{
    //dtor
}

void TwoDepthSearch::process()
{
    IProcess::process();
    cout << "Two Depth Search..." << endl;
    TwoDepthSearch::sampleSize = ITR::getData()->getSampleSize();
    TwoDepthSearch::varSize = ITR::getData()->getX_Type().size();
    if (isPrint)
    {
        TwoDepthSearch::searchPrint();
    }
    else
    {
        ITR::searchResults = searchOutput();
    }
}

void TwoDepthSearch::searchPrint()
{
    double v[8];		 // 000 to 111
    double sum[4];       // Store sum info

    bool* x1c;                // Cut Table for var 1
    bool* x2c;                // Cut Table for var 2
    unsigned int* row_A;      // A
    double* row_Y;            // Y
    vector<unsigned int> cutRange1;        // Cut range for var 1
    vector<unsigned int> cutRange2;        // Cut range for var 2


    for (unsigned int i = 0; i <TwoDepthSearch::varSize; ++i)
    {
        cutRange1 = ITR::info[i]->getRange();
        for (unsigned int j=i+1; j <TwoDepthSearch::varSize; ++j)
        {
            cutRange2 = ITR::info[j]->getRange();

            for(unsigned int xi=0; xi<ITR::cutSize[i]; ++xi) // loop4
            {
                x1c = table_X[i][xi];
                for(unsigned int xj=0; xj<ITR::cutSize[j]; ++xj)        // loop5
                {
                    x2c = table_X[j][xj];

                    std::fill_n(v,8,0.0);
                    for(unsigned int p=0; p<sampleSize; ++p)
                    {
                        row_Y = var_Y[p];
                        row_A = var_A[p];
                        v[x1c[p]*4+x2c[p]*2 + *row_A] +=  *row_Y;
                    }
                    sum[0] = v[1]-v[0];   // 00 (001-000)
                    sum[1] = v[3]-v[2];   // 01 (011-010)
                    sum[2] = v[5]-v[4];   // 10 (101-100)
                    sum[3] = v[7]-v[6];   // 11 (111-110)
                    const auto ptr = max_element(sum,sum+4);
                    double temp = *ptr + ITR::T0;
                    cout<<temp * 2 / TwoDepthSearch::sampleSize
                        <<", var "<<i<<" cut "<<cutRange1[xi]
                        <<", var "<<j<<" cut "<<cutRange2[xj]
                        <<", index "<<distance(sum, ptr)
                        <<endl;
                }
            }
        }
    }

    delete x1c;
    delete x2c;
	delete row_A;
	delete row_Y;
}

vector<Res *> TwoDepthSearch::searchOutput()
{
	vector<Res *> solutions;
	solutions.reserve(3753);

    double v[8];		 // 000 to 111
    double sum[4];       // Store sum info

    bool* x1c;                // Cut Table for var 1
    bool* x2c;                // Cut Table for var 2
    unsigned int* row_A;     // A
    double* row_Y;           // Y
    vector<unsigned int> cutRange1;        // Cut range for var 1
    vector<unsigned int> cutRange2;        // Cut range for var 2

    for (unsigned int i = 0; i <TwoDepthSearch::varSize; ++i)
    {
        cutRange1 = ITR::info[i]->getRange();
        for (unsigned int j=i+1; j <TwoDepthSearch::varSize; ++j)
        {
            cutRange2 = ITR::info[j]->getRange();

            for(unsigned int xi=0; xi<ITR::cutSize[i]; ++xi) // loop4
            {
                x1c = table_X[i][xi];
                for(unsigned int xj=0; xj<ITR::cutSize[j]; ++xj)        // loop5
                {
                    x2c = table_X[j][xj];
                    std::fill_n(v,8,0.0);

                    for(unsigned int p=0; p<sampleSize; ++p)
                    {
                        row_Y = var_Y[p];
                        row_A = var_A[p];
                        v[x1c[p]*4+x2c[p]*2 + *row_A] +=  *row_Y;
                    }
                    sum[0] = v[1]-v[0];   // 00 (001-000)
                    sum[1] = v[3]-v[2];   // 01 (011-010)
                    sum[2] = v[5]-v[4];   // 10 (101-100)
                    sum[3] = v[7]-v[6];   // 11 (111-110)
                    const auto ptr = max_element(sum,sum+4);
                    double temp = (*ptr + ITR::T0) * 2 / TwoDepthSearch::sampleSize;
                    solutions.push_back(new Res(temp, { i, cutRange1[xi], j,  cutRange2[xj]}, distance(sum, ptr)));
                }
            }
        }
    }

    delete x1c;
    delete x2c;
	delete row_A;
	delete row_Y;


	return solutions;
}
