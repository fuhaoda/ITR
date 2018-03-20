#include "ITR.h"

ITR::ITR(DataGeneration &data, int d)
{
    depth = d;
    var_Type = data.getVarType();
    sample_Size = data.getSampleSize();
    var_Size = data.getVariateSize();
    action_Size = data.getActionSize();
    y_Size = data.getYSize();
    lookup = combine(var_Size, depth);
    T0 = data.getSumT0();

    init();

    for(int i=0; i<var_Size; ++i)
    {
        info[i].load_DataInfo(i,var_Type[i],data.getDataSet()[i]);
    }
    load_CutSize();
    load_X(data.getDataSet());
    load_Action(data.getActions());
    load_Y(data.getY());

    init_TableX();
    load_table_X(data.getDataSet());
}

ITR::~ITR()
{
    //dtor
    cleanAll();
}

void ITR::init()
{
    var_X = new int*[sample_Size] {};
    var_A = new int*[sample_Size] {};
    var_Y = new double*[sample_Size];
    table_X = new bool**[var_Size] {};
    cut_Size = new int[var_Size] {};
    info = new DataInfo[var_Size];

    for(int i=0; i<sample_Size; ++i)
    {
        var_X[i] = new int[var_Size] {};
        var_A[i] = new int[action_Size] {};
        var_Y[i] = new double[y_Size] {};
    }
}

void ITR::init_TableX()
{
    for(int i=0; i<var_Size; ++i)
    {
        table_X[i] = new bool*[cut_Size[i]] {};
        for(int j=0; j<cut_Size[i]; ++j)
        {
            table_X[i][j] = new bool[sample_Size] {};
        }
    }
}



/// Get
int ITR::getSampleSize()
{
    return sample_Size;
}
int ITR::getVarSize()
{
    return var_Size;
}
int ITR::getActionSize()
{
    return action_Size;
}
int ITR::getYSize()
{
    return y_Size;
}

/// Print
void ITR :: print_X()
{
    for(int i=0; i<sample_Size; ++i)
    {
        for(int j=0; j<var_Size; ++j)
        {
            cout<<var_X[i][j]<<' ';
        }
        cout<<endl;
    }
}

void ITR :: print_Action()
{
    for(int i=0; i<sample_Size; ++i)
    {
        for(int j=0; j<action_Size; ++j)
        {
            cout<<var_A[i][j]<<' ';
        }
        cout<<endl;
    }
}

void ITR :: print_Y()
{
    for(int i=0; i<sample_Size; ++i)
    {
        for(int j=0; j<y_Size; ++j)
        {
            cout<<var_Y[i][j]<<' ';
        }
        cout<<endl;
    }
}

void ITR :: print_All()
{
    for(int i=0; i<sample_Size; ++i)
    {
        cout<<"X: ";
        for(int x=0; x<var_Size; ++x)
        {
            cout<<var_X[i][x]<<' ';
        }
        cout<<" Action: ";
        for(int y=0; y<action_Size; ++y)
        {
            cout<<var_A[i][y]<<' ';
        }
        cout<<" Y: ";
        for(int z=0; z<y_Size; ++z)
        {
            cout<<var_Y[i][z]<<' ';
        }
        cout<<endl;
    }
}

void ITR :: print_Range()
{
    cout<<"Cut range:\n";
    for(int i=0; i<var_Size; ++i)
        cout<<cut_Size[i]<<' ';
}

void ITR :: print_Type()
{
    cout<<"Covariant type:\n";
    for(int i=0; i<var_Size; ++i)
    {
        cout<<var_Type[i]<<' ';
    }
}

void ITR :: print_CutTable()
{
    for(int i=0; i<var_Size; ++i)
    {
        cout<<"Var: "<<i
            <<"\tType: "<<var_Type[i]
            <<"\tCut Size: "<<cut_Size[i]
            <<endl;
        cout<<"\t ";

        for(int xi=0; xi<sample_Size; ++xi)
        {
            cout<<var_X[xi][i]<<' ';
        }
        cout<<endl;
        for(int j=0; j<cut_Size[i]; ++j)
        {
            if(var_Type[i]==2)
            {
                cout<<info[i].getRange(j)<<":";
                info[i].printSet(j);
            }
            else
            {
                cout<<info[i].getRange(j)<<":\t";
            }
            for(int k=0; k<sample_Size; ++k)
            {
                cout<<table_X[i][j][k]<<' ';
            }
            cout<<endl;
        }
        cout<<endl;
    }
}

void ITR :: print_VarInfo(int i)
{
    info[i].printVarInfo();
}

void ITR :: print_VarInfo()
{
    for(int i=0; i<var_Size; ++i)
    {
        info[i].printVarInfo();
        cout<<endl;
    }
}


void ITR :: print_Combinations()
{
    int no = 0;
    for(auto x : lookup)
    {
        cout<<no++<<": ";
        cout<<"[ ";
        for(const auto &row : x)
            cout<<row<<' ';
        cout<<']'<<endl;
    }
}


/// Load
void ITR :: load_CutSize()
{
    for(int i=0; i<var_Size; ++i)
        cut_Size[i] = info[i].getCutSize();
}


void ITR :: load_X(vector<vector<int>> x)
{
    for(int i=0; i<sample_Size; ++i)
    {
        for(int j=0; j<var_Size; ++j)
        {
            var_X[i][j] = x[j][i];
        }
    }
}

void ITR :: load_Action(vector<vector<int>> a)
{
    for(int i=0; i<sample_Size; ++i)
    {
        for(int j=0; j<action_Size; ++j)
        {
            var_A[i][j] = a[j][i];
        }
    }
}

void ITR :: load_Y(vector<vector<double>> y)
{
    for(int i=0; i<sample_Size; ++i)
    {
        for(int j=0; j<y_Size; ++j)
        {
            var_Y[i][j] = y[j][i];
        }
    }
}


void ITR :: load_table_X(vector<vector<int>> x)
{
    init_TableX();
    for(int i=0; i<var_Size; ++i)
    {
        for(int j=0; j<sample_Size; ++j)
        {
            for(int k=0; k<cut_Size[i]; ++k)
            {
                table_X[i][k][j] = (var_Type[i] == 2)?info[i].nomContains(x[i][j],k):x[i][j] < info[i].getRange(k);
            }
        }
    }
}

void ITR :: cleanAll()
{
    for(int i=0; i<sample_Size; ++i)
    {
        delete [] var_X[i];
        delete [] var_A[i];
        delete [] var_Y[i];
    }
    delete var_X;
    delete var_A;
    delete var_Y;

    for(int i=0; i<var_Size; ++i)
    {
        for(int j=0; j<cut_Size[i]; ++j)
        {
            delete [] table_X[i][j];
        }
        delete [] table_X[i];
    }
    delete table_X;
    delete [] cut_Size;
    delete [] info;
}

vector<vector<int>> ITR :: combine(int n, int k)
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


