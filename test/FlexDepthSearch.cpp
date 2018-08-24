#include "pch.h"
#include "FlexDepthSearch.h"
#include <cmath>

FlexDepthSearch::FlexDepthSearch(ITR* itr, unsigned int d, bool print):IProcess(itr)
{
    FlexDepthSearch::depth = d;
    FlexDepthSearch::isPrint = print;
}

FlexDepthSearch::~FlexDepthSearch()
{
    //dtor
}

void FlexDepthSearch::process()
{
    IProcess::process();
    cout<<"Flex search of depth "<<FlexDepthSearch::depth<<"..."<<endl;
    FlexDepthSearch::sampleSize = ITR::getData()->getSampleSize();
    FlexDepthSearch::varSize = ITR::getData()->getX_Type().size();
    FlexDepthSearch::varType = ITR::getVarType();
    FlexDepthSearch::lookup = combine(varSize, depth);;

    if (FlexDepthSearch::isPrint)
    {
        FlexDepthSearch::searchPrint();
    }
    else
    {
        ITR::searchResults = FlexDepthSearch::searchOutput();
    }
}


void FlexDepthSearch :: searchPrint()
{
    int v_size = pow(2,depth+1);
    unsigned int sum_size = pow(2,depth);

    double* v = new double[v_size] {};
    double* sum = new double[sum_size] {};
    int* cut = new int[depth] {};
    int* cut_No = new int[depth] {};
    int* cut_best = new int[depth] {};
    bool** f = new bool*[depth] {};
    int* power = new int[depth] {};
    int* cur = new int[depth] {};

    unsigned int max_comb, temp, index;   // index for cut number
    int vt;
    double bestLocal, bestIndexLocal, temp_res;


    for(auto const &vec : lookup)   // loop over variable combinations
    {
        max_comb = 1;
        for(unsigned int i=0; i<depth; ++i)
        {
            cut[i] = cutSize[vec[i]];
            max_comb *= cut[i];
            power[i] = pow(2,depth-i);
        }
        bestLocal = 0.0;
        bestIndexLocal = 0.0;
        temp_res = 0.0;
        std::fill_n(cut_best,depth,0);

        for(unsigned int i=0; i<max_comb; ++i)   // Given variables, loop cuts combinations
        {
            temp = i;
            for(unsigned int j=0; j<depth; ++j)
            {
                index = temp % cut[j];
                temp /= cut[j];
                cut_No[j] = index;
                f[j] = table_X[vec[j]][index];
            }

            std::fill_n(v,v_size,0.0);
            std::fill_n(sum,sum_size,0.0);

            for(unsigned int p=0; p<sampleSize; ++p)
            {
                vt = *var_A[p];
                for(unsigned int r=0; r<depth; ++r)
                {
                    vt += power[r]*f[r][p];
                }
                v[vt] +=  *var_Y[p];
            }

            for(unsigned int k=0; k<sum_size; ++k)
            {
                sum[k] = v[2*k+1] - v[2*k];
            }

            const auto ptr = max_element(sum,sum+sum_size);
            temp_res = *ptr + ITR::T0;
            if(temp_res > bestLocal)
            {
                bestLocal = temp_res;
                bestIndexLocal = distance(sum, ptr);
                for(unsigned int q=0; q<depth; ++q)
                {
                    cut_best[q] = cut_No[q];
                }
            }
        } // end cut loop
        cout<<bestLocal*2/sampleSize<<" Index: ";
        for(unsigned int i=0; i<depth; ++i)
        {
            if(FlexDepthSearch::varType[vec[i]] == 2)
            {
                cout<<" ["<<vec[i]<<" "<<cut_best[i]<<"]";
            }
            else
            {
                cout<<" ["<<vec[i]<<" "<<cut_best[i]+1<<"]";
            }

        }
        cout<<bestIndexLocal<<endl;
    } // end lookup loop



    delete [] v;
    delete [] sum;
    delete [] cut;
    delete [] cut_No;
    delete [] cut_best;
    delete [] f;
    delete [] power;
    delete [] cur;

}


vector<Res *> FlexDepthSearch :: searchOutput()
{
    vector<Res *> solutions;
    solutions.reserve(FlexDepthSearch :: lookup.size());

    int v_size = pow(2,depth+1);
    int sum_size = pow(2,depth);

    double* v = new double[v_size] {};
    double* sum = new double[sum_size] {};
    int* cut = new int[depth] {};
    int* cut_No = new int[depth] {};
    int* cut_best = new int[depth] {};
    bool** f = new bool*[depth] {};
    int* power = new int[depth] {};
    int* cur = new int[depth] {};
    vector<unsigned int> indexOut;

    unsigned int max_comb, temp, index;   // index for cut number
    int vt;
    double bestLocal, bestIndexLocal, temp_res;

    for(auto const &vec : lookup)   // loop over variable combinations
    {
        max_comb = 1;
        for(unsigned int i=0; i<depth; ++i)
        {
            cut[i] = cutSize[vec[i]];
            max_comb *= cut[i];
            power[i] = pow(2,depth-i);
        }
        bestLocal = 0.0;
        bestIndexLocal = 0.0;
        temp_res = 0.0;
        std::fill_n(cut_best,depth,0);

        for(unsigned int i=0; i<max_comb; ++i)   // Given variables, loop cuts combinations
        {
            temp = i;
            for(unsigned int j=0; j<depth; ++j)
            {
                index = temp % cut[j];
                temp /= cut[j];
                cut_No[j] = index;
                f[j] = table_X[vec[j]][index];
            }

            std::fill_n(v,v_size,0.0);
            std::fill_n(sum,sum_size,0.0);

            for(unsigned int p=0; p<sampleSize; ++p)
            {
                vt = *var_A[p];
                for(unsigned int r=0; r<depth; ++r)
                {
                    vt += power[r]*f[r][p];
                }
                v[vt] +=  *var_Y[p];
            }

            for(int k=0; k<sum_size; ++k)
            {
                sum[k] = v[2*k+1] - v[2*k];
            }

            const auto ptr = max_element(sum,sum+sum_size);
            temp_res = *ptr + ITR::T0;
            if(temp_res > bestLocal)
            {
                bestLocal = temp_res;
                bestIndexLocal = distance(sum, ptr);
                for(unsigned int q=0; q<depth; ++q)
                {
                    cut_best[q] = cut_No[q];
                }
            }
        } // end cut loop
        indexOut.clear();

        for(unsigned int i=0; i<depth; ++i)
        {
            indexOut.push_back(vec[i]);
            if(FlexDepthSearch::varType[vec[i]] == 2)
            {
                indexOut.push_back(cut_best[i]);
            }
            else
            {
                indexOut.push_back(cut_best[i]+1);
            }

        }
        solutions.push_back(new Res(bestLocal*2/sampleSize,indexOut,bestIndexLocal));
    } // end lookup loop

    delete [] v;
    delete [] sum;
    delete [] cut;
    delete [] cut_No;
    delete [] cut_best;
    delete [] f;
    delete [] power;
    delete [] cur;
    return solutions;
}


vector<vector<int>> FlexDepthSearch :: combine(int n, int k)
{
    vector<vector<int>> vectOut;
    int i = 0;
    vector<int> p(k, -1);
    while (i >= 0)
    {
        p[i]++;
        if (p[i] > n-1) --i;
        else if (i == k - 1) vectOut.push_back(p);
        else
        {
            ++i;
            p[i] = p[i - 1];
        }
    }
    return vectOut;
}

