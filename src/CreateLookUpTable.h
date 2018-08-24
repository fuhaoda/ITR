#ifndef CREATELOOKUPTABLE_H
#define CREATELOOKUPTABLE_H

#include "IProcess.h"

class CreateLookUpTable : public IProcess
{
public:
    CreateLookUpTable(ITR* itr, unsigned int index = 0);
    virtual ~CreateLookUpTable();
    void process();

private:
    unsigned int actionIndex;
    unsigned int sampleSize;
    unsigned int varSize;
    unsigned int actionSize;
    unsigned int ySize;

    void init();
    void load_CutSize();
    void init_TableX();
    void load_table_X(const vector<vector<unsigned int>> &data_X);
    void load_Action(const vector<vector<unsigned int>> &data_A);
    void load_Y(const vector<vector<double>> &data_Y);
    void cleanAll();
    double sumTreatmentCal();

	void print_info();
	void print_Action();
	void print_Y();
	void print_CutSize();
};

#endif // CREATELOOKUPTABLE_H
