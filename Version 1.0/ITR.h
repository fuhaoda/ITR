#ifndef ITR_H
#define ITR_H

#include "DataGeneration.h"
#include "Res.h"
#include "DataInfo.h"
#include <iostream>
#include <cmath>

using namespace std;

class ITR
{
protected:
    vector<int> var_Type; // var_Size
    int depth;
    vector<vector<int>> lookup; // combination size x depth

    int** var_X;     // X 2d matrix             (sample_Size x var_Size)
    int** var_A;     // Action 2d matrix        (sample_Size x action_Size)
    double** var_Y;  // Y 2d matrix             (sample_Size x y_Size)
    bool*** table_X; // lookup table 3d matrix  (var_Size x cut_Size[i] x sample_Size)
    DataInfo* info;  // Variable info vector    (var_Size)
    int* cut_Size;   // Cut size vector         (var_Size)

    int sample_Size;
    int var_Size;
    int action_Size;
    int y_Size;

    double T0;       // Total sum of Non Treatment

public:
    ITR(DataGeneration &data, int d);  // d for depth
    ~ITR();

    int getSampleSize();
    int getVarSize();
    int getActionSize();
    int getYSize();
    int* get_CutSize(){ return cut_Size;};
    int** get_X(){ return var_X;};
    int** get_Action(){ return var_A;};
    double** get_Y(){ return var_Y;};
    bool*** get_X_Table(){ return table_X; };
    DataInfo* get_DataInfo(){ return info; };

    void print_X();
    void print_Action();
    void print_Y();
    void print_All();
    void print_Range();
    void print_Type();
    void print_CutTable();
    void print_VarInfo(int i);
    void print_VarInfo();
    void print_Combinations();

    virtual void searchPrint() = 0;
    virtual vector<Res *> searchOutput() = 0;

private:
    void init();
    void init_TableX();
    void load_CutSize();
    void load_table_X(vector<vector<int>> x);
    void load_X(vector<vector<int>> x);
    void load_Action(vector<vector<int>> a);
    void load_Y(vector<vector<double>> y);
    void cleanAll();
    vector<vector<int>> combine(int n, int k);
};

#endif // ITR_H
