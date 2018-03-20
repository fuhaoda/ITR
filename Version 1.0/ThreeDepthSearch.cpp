#include "ThreeDepthSearch.h"

void ThreeDepthSearch :: searchPrint(){
    double v[16];   // 0000 to 1111
    double sum[8];

    bool*  x1c;
    bool*  x2c;
    bool*  x3c;
    int* row_A;
    double* row_Y;

    double bestLocal;
    int bestIndexLocal;
    int indexi, indexj, indexk;
    int cuti, cutj, cutk;


    for(int i=0; i<var_Size; ++i)   // loop1
    {
        for(int j=i+1; j<var_Size; ++j)    // loop2
        {
            for(int k=j+1; k<var_Size; ++k)    // loop3
            {
                bestLocal = 0.0;
                bestIndexLocal = 0;
                indexi = 0, indexj = 0, indexk = 0;
                cuti = 0, cutj = 0, cutk = 0;

                for(int xi=0; xi<cut_Size[i]; ++xi) // loop4
                {
                    x1c = table_X[i][xi];
                    for(int xj=0; xj<cut_Size[j]; ++xj)        // loop5
                    {
                        x2c = table_X[j][xj];
                        for(int xk=0; xk<cut_Size[k]; ++xk) // loop6
                        {
                            x3c = table_X[k][xk];

                            std::fill_n(v,16,0.0);
                            for(int p=0; p<sample_Size; ++p)
                            {
                                row_Y = var_Y[p];
                                row_A = var_A[p];
                                v[x1c[p]*8+x2c[p]*4+x3c[p]*2 + *row_A] +=  *row_Y;
                            }

                            sum[0] = v[1]-v[0];   // 000 (0001-0000)
                            sum[1] = v[3]-v[2];   // 001 (0011-0010)
                            sum[2] = v[5]-v[4];   // 010 (0101-0100)
                            sum[3] = v[7]-v[6];   // 011 (0111-0110)
                            sum[4] = v[9]-v[8];   // 100 (1001-1000)
                            sum[5] = v[11]-v[10]; // 101 (1011-1010)
                            sum[6] = v[13]-v[12]; // 110 (1101-1100)
                            sum[7] = v[15]-v[14]; // 111 (1111-1110)

                            const auto ptr = max_element(sum,sum+8);
                            double temp = *ptr + T0;

                            if(temp > bestLocal)
                            {
                                bestLocal = temp;
                                bestIndexLocal = distance(sum, ptr);
                                indexi = i;
                                indexj = j;
                                indexk = k;
                                cuti = xi;
                                cutj = xj;
                                cutk = xk;
                            }
                        } // end loop 6
                    } // end loop 5
                } // end loop 4
                cout<<bestLocal*2/sample_Size<<" Index: "
                    <<"["<<indexi<<" "<<cuti<<" "
                    <<"] ["<<indexj<<" "<<cutj<<" "
                    <<"] ["<<indexk<<" "<<cutk<<" "
                    <<"]"<<bestIndexLocal<<endl;
            } // end loop 3
        } // end loop 2
    } // end loop 1
} // end searchPrint()


vector<Res *> ThreeDepthSearch :: searchOutput(){
    vector<Res *> solutions;
    solutions.reserve(20000000);
    double v[16];

    bool*  x1c;
    bool*  x2c;
    bool*  x3c;
    int* row_A;
    double* row_Y;

    for(int i=0; i<var_Size; ++i)   // loop1
    {
        for(int j=i+1; j<var_Size; ++j)    // loop2
        {
            for(int k=j+1; k<var_Size; ++k)    // loop3
            {
                for(int xi=0; xi<cut_Size[i]; ++xi)            // loop4
                {
                    x1c = table_X[i][xi];
                    for(int xj=0; xj<cut_Size[j]; ++xj)        // loop5
                    {
                        x2c = table_X[j][xj];
                        for(int xk=0; xk<cut_Size[k]; ++xk)    // loop6
                        {
                            x3c = table_X[k][xk];

                            std::fill_n(v,16,0.0);
                            for(int p=0; p<sample_Size; ++p)
                            {
                                row_Y = var_Y[p];
                                row_A = var_A[p];
                                v[x1c[p]*8+x2c[p]*4+x3c[p]*2 + *row_A] +=  *row_Y;
                            }
                            solutions.push_back(new Res(T0+v[1]-v[0],{i,j,k},0));   // 000
                            solutions.push_back(new Res(T0+v[3]-v[2],{i,j,k},1));   // 001
                            solutions.push_back(new Res(T0+v[5]-v[4],{i,j,k},2));   // 010
                            solutions.push_back(new Res(T0+v[7]-v[6],{i,j,k},3));   // 011
                            solutions.push_back(new Res(T0+v[9]-v[8],{i,j,k},4));   // 100
                            solutions.push_back(new Res(T0+v[11]-v[10],{i,j,k},5)); // 101
                            solutions.push_back(new Res(T0+v[13]-v[12],{i,j,k},6)); // 110
                            solutions.push_back(new Res(T0+v[15]-v[14],{i,j,k},7)); // 111
                        } // end loop 6
                    } // end loop 5
                } // end loop 4
            } // end loop 3
        } // end loop 2
    } // end loop 1
    return solutions;
}
