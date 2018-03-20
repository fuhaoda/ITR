#include "pch.h"
#include "OneDepthSearch.h"


OneDepthSearch::OneDepthSearch(ITR* itr, bool print) :IProcess(itr)
{
	OneDepthSearch::isPrint = print;
}


OneDepthSearch::~OneDepthSearch()
{
}

void OneDepthSearch::process() {
	IProcess::process();
	cout << "One Depth Search..." << endl;
	OneDepthSearch::sampleSize = ITR::getData()->getSampleSize();
	OneDepthSearch::varSize = ITR::getData()->getX_Type().size();
	if (isPrint) {
		OneDepthSearch::searchPrint();
	}
	else {
		ITR::searchResults = searchOutput();
	}
}

void OneDepthSearch::searchPrint() {
	double v[4];		 // 00 to 11
	double sum[2];       // Store sum info

	bool* xc;                // Cut Table
	unsigned int* row_A;     // A
	double* row_Y;           // Y
	vector<unsigned int> cutRange;        // Cut range

	for (unsigned int i = 0; i <OneDepthSearch::varSize; ++i) {
		cutRange = ITR::getDataInfo()[i]->getRange();
		for (unsigned int xi = 0; xi < ITR::cutSize[i]; ++xi) {
			xc = ITR::table_X[i][xi];
			std::fill_n(v, 4, 0.0);
			for (unsigned int p = 0; p<OneDepthSearch::sampleSize; ++p)
			{	
				row_Y = ITR::var_Y[p];
				row_A = ITR::var_A[p];
				v[xc[p] * 2 + *row_A] += *row_Y;
			}
			sum[0] = v[1] - v[0] + ITR::T0;
			sum[1] = v[3] - v[2] + ITR::T0;
			if (ITR::getVarType()[i] != 2) {
				cout << sum[0] * 2 / OneDepthSearch::sampleSize << " "
					<< "x" << i
					<< " < " << cutRange[xi]
					<< endl;
				cout << sum[1] * 2 / OneDepthSearch::sampleSize << " "
					<< "x" << i
					<< " >= " << cutRange[xi]
					<< endl;
			}
			else {
				cout << sum[0] * 2 / OneDepthSearch::sampleSize << " "
					<< "x" << i
					<< " in set " << cutRange[xi]
					<< endl;
				cout << sum[1] * 2 / OneDepthSearch::sampleSize << " "
					<< "x" << i
					<< " not in set " << cutRange[xi]
					<< endl;
			}

		}
	}

	delete xc;
	delete row_A;
	delete row_Y;
}

vector<Res *> OneDepthSearch::searchOutput() {
	vector<Res *> solutions;
	solutions.reserve(20);
	double v[4];		 // 00 to 11
	double sum[2];      // Store sum info

	bool* xc;                // Cut Table
	unsigned int* row_A;     // A
	double* row_Y;           // Y
	vector<unsigned int> cutRange;        // Cut range

	for (unsigned int i = 0; i <OneDepthSearch::varSize; ++i)
	{
		cutRange = ITR::getDataInfo()[i]->getRange();
		for (unsigned int xi = 0; xi < ITR::cutSize[i]; ++xi)
		{
			xc = ITR::table_X[i][xi];
			std::fill_n(v, 4, 0.0);
			for (unsigned int p = 0; p<OneDepthSearch::sampleSize; ++p)
			{
				row_Y = ITR::var_Y[p];
				row_A = ITR::var_A[p];
				v[xc[p] * 2 + *row_A] += *row_Y;
			}
			sum[0] = (v[1] - v[0] + ITR::T0) * 2 / OneDepthSearch::sampleSize;
			sum[1] = (v[3] - v[2] + ITR::T0) * 2 / OneDepthSearch::sampleSize;
			solutions.push_back(new Res(sum[0], { i, cutRange[xi] }, 0));
			solutions.push_back(new Res(sum[1], { i, cutRange[xi] }, 1));
		}
	}

	delete xc;
	delete row_A;
	delete row_Y;

	return solutions;
}