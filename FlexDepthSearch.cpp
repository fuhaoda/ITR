#include "FlexDepthSearch.h"

void FlexDepthSearch :: searchPrint(){
    int v_size = pow(2,depth+1);
    int sum_size = pow(2,depth);

    double* v = new double[v_size] {};
    double* sum = new double[sum_size] {};
    int* cut = new int[depth] {};
    int* cut_No = new int[depth] {};
    int* cut_best = new int[depth] {};
    bool** f = new bool*[depth] {};
    int* power = new int[depth]{};
    int* cur = new int[depth]{};

    int max_comb, temp, index;   // index for cut number
    int vt;
    double bestLocal, bestIndexLocal, temp_res;

    for(auto const &vec : lookup)   // loop over variable combinations
    {
        max_comb = 1;
        for(int i=0; i<depth; ++i)
        {
            cut[i] = cut_Size[vec[i]];
            max_comb *= cut[i];
            power[i] = pow(2,depth-i);
        }
        bestLocal = 0.0;
        bestIndexLocal = 0.0;
        temp_res = 0.0;
        std::fill_n(cut_best,depth,0);

        for(int i=0; i<max_comb; ++i)   // Given variables, loop cuts combinations
        {
            temp = i;
            for(int j=0; j<depth; ++j)
            {
                index = temp % cut[j];
                temp /= cut[j];
                cut_No[j] = index;
                f[j] = table_X[vec[j]][index];
            }

            std::fill_n(v,v_size,0.0);
            std::fill_n(sum,sum_size,0.0);

            for(int p=0; p<sample_Size; ++p)
            {
                vt = *var_A[p];
                for(int r=0; r<depth; ++r){
                    vt += power[r]*f[r][p];
                }
                v[vt] +=  *var_Y[p];
            }

            for(int k=0; k<sum_size; ++k)
            {
                sum[k] = v[2*k+1] - v[2*k];
            }

            const auto ptr = max_element(sum,sum+sum_size);
            temp_res = *ptr + T0;
            if(temp_res > bestLocal){
                bestLocal = temp_res;
                bestIndexLocal = distance(sum, ptr);
                for(int q=0; q<depth;++q){
                    cut_best[q] = cut_No[q];
                }
            }
        } // end cut loop
        cout<<bestLocal*2/sample_Size<<" Index: ";
        for(int i=0;i<depth;++i){
            cout<<" ["<<vec[i]<<" "<<cut_best[i]<<"]";
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








vector<Res *> FlexDepthSearch :: searchOutput(){
    vector<Res *> solutions;
    solutions.reserve(20000000);
    int v_size = pow(2,depth+1);
    int sum_size = pow(2,depth);

    double* v = new double[v_size] {};
    double* sum = new double[sum_size] {};
    int* cut = new int[depth] {};
    int* cut_No = new int[depth] {};
    int* cut_best = new int[depth] {};
    bool** f = new bool*[depth] {};
    int* power = new int[depth]{};
    int* cur = new int[depth]{};

    int max_comb, temp, index;   // index for cut number
    int vt;
    double bestLocal, bestIndexLocal, temp_res;

    for(auto const &vec : lookup)   // loop over variable combinations
    {
        max_comb = 1;
        for(int i=0; i<depth; ++i)
        {
            cut[i] = cut_Size[vec[i]];
            max_comb *= cut[i];
            power[i] = pow(2,depth-i);
        }
        bestLocal = 0.0;
        bestIndexLocal = 0.0;
        temp_res = 0.0;
        std::fill_n(cut_best,depth,0);

        for(int i=0; i<max_comb; ++i)   // Given variables, loop cuts combinations
        {
            temp = i;
            for(int j=0; j<depth; ++j)
            {
                index = temp % cut[j];
                temp /= cut[j];
                cut_No[j] = index;
                f[j] = table_X[vec[j]][index];
            }

            std::fill_n(v,v_size,0.0);
            std::fill_n(sum,sum_size,0.0);

            for(int p=0; p<sample_Size; ++p)
            {
                vt = *var_A[p];
                for(int r=0; r<depth; ++r){
                    vt += power[r]*f[r][p];
                }
                v[vt] +=  *var_Y[p];
            }

            for(int k=0; k<sum_size; ++k)
            {
                solutions.push_back(new Res(v[2*k+1] - v[2*k],vec,k));
            }
        } // end cut loop
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
