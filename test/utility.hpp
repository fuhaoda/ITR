#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <iostream>

using namespace std;


namespace utility
{
template<class T>
void print1DVector(vector<T> const &vectIn)
{
    try
    {
        cout<<"[ ";
        for(const auto &row : vectIn)
        {
            cout<<row<<' ';
        }
        cout<<']'<<endl;
    }
    catch (const std::out_of_range& oor)
    {
        cout << "Out of Range error in print1DVector. Please check for empty input. "<<endl;
        cout << "Error message as following: "<<endl;
        cerr <<oor.what() << '\n';
    }
}

template<class T>
void print2DVector(vector<vector<T>> const &vectIn)
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
        cout << endl;
    }
    catch (const std::out_of_range& oor)
    {
        cout << "Out of Range error in print2DVector. Please check for empty input. "<<endl;
        cout << "Error message as following: "<<endl;
        cerr <<oor.what() << '\n';
    }
}
}

#endif // UTILITY_H
