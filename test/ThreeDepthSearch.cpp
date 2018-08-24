#include "pch.h"
#include "ThreeDepthSearch.h"


ThreeDepthSearch::ThreeDepthSearch(ITR* itr, bool print) :IProcess(itr)
{
	ThreeDepthSearch::isPrint = print;
}

ThreeDepthSearch::~ThreeDepthSearch()
{
	//dtor
}

void ThreeDepthSearch::process()
{
	IProcess::process();
	cout << "Three Depth Search..." << endl;
	ThreeDepthSearch::sampleSize = ITR::getData()->getSampleSize();
	ThreeDepthSearch::varSize = ITR::getData()->getX_Type().size();
	if (isPrint)
	{
		ThreeDepthSearch::searchPrint();
	}
	else
	{
		ITR::searchResults = ThreeDepthSearch::searchOutput();
	}
}

void ThreeDepthSearch::searchPrint()
{
	double v[16];   // 0000 to 1111
	double sum[8];  // Store sum info

	bool*  x1c;     // Cut Table for var 1
	bool*  x2c;     // Cut Table for var 2
	bool*  x3c;     // Cut Table for var 3
	unsigned int* row_A;
	double* row_Y;

	for (unsigned int i = 0; i <ThreeDepthSearch::varSize; ++i)   // loop1
	{
		for (unsigned int j = i + 1; j <ThreeDepthSearch::varSize; ++j)   // loop2
		{
			for (unsigned int k = j + 1; k <ThreeDepthSearch::varSize; ++k) // loop3
			{
				for (unsigned int xi = 0; xi<cutSize[i]; ++xi) // loop4
				{
					x1c = table_X[i][xi];
					for (unsigned int xj = 0; xj<cutSize[j]; ++xj)        // loop5
					{
						x2c = table_X[j][xj];
						for (unsigned int xk = 0; xk<cutSize[k]; ++xk) // loop6
						{
							x3c = table_X[k][xk];

							std::fill_n(v, 16, 0.0);
							for (unsigned int p = 0; p<sampleSize; ++p)
							{
								row_Y = var_Y[p];
								row_A = var_A[p];
								v[x1c[p] * 8 + x2c[p] * 4 + x3c[p] * 2 + *row_A] += *row_Y;
							}

							sum[0] = v[1] - v[0];   // 000 (0001-0000)
							sum[1] = v[3] - v[2];   // 001 (0011-0010)
							sum[2] = v[5] - v[4];   // 010 (0101-0100)
							sum[3] = v[7] - v[6];   // 011 (0111-0110)
							sum[4] = v[9] - v[8];   // 100 (1001-1000)
							sum[5] = v[11] - v[10]; // 101 (1011-1010)
							sum[6] = v[13] - v[12]; // 110 (1101-1100)
							sum[7] = v[15] - v[14]; // 111 (1111-1110)

							const auto ptr = max_element(sum, sum + 8);
							double temp = *ptr + ITR::T0;
							cout << temp * 2 / ThreeDepthSearch::sampleSize
								//                                <<" [ "<<i<<' '<<xi<<' '<<j<<' '<<xj<<' '<<k<<' '<<xk<<" ]"
								<< ' ' << distance(sum, ptr)
								<< endl;
						} // end loop 6
					} // end loop 5
				} // end loop 4
			} // end loop 3
		} // end loop 2
	} // end loop 1
} // end searchPrint()

vector<Res *> ThreeDepthSearch::searchOutput() {
	vector<Res *> solutions;
	solutions.reserve(86131);

	double v[16];			// 0000 to 1111
	double sum[8];			// Store sum info

	bool*  x1c;				// Cut Table for var 1
	bool*  x2c;				// Cut Table for var 2
	bool*  x3c;				// Cut Table for var 3
	unsigned int* row_A;	// A
	double* row_Y;			// Y

	vector<unsigned int> cutRange1;        // Cut range for var 1
	vector<unsigned int> cutRange2;        // Cut range for var 2
	vector<unsigned int> cutRange3;        // Cut range for var 3


	for (unsigned int i = 0; i <ThreeDepthSearch::varSize; ++i)   // loop1
	{
		cutRange1 = ITR::info[i]->getRange();
		for (unsigned int j = i + 1; j <ThreeDepthSearch::varSize; ++j)   // loop2
		{
			cutRange2 = ITR::info[j]->getRange();
			for (unsigned int k = j + 1; k <ThreeDepthSearch::varSize; ++k) // loop3
			{
				cutRange3 = ITR::info[k]->getRange();
				for (unsigned int xi = 0; xi<ITR::cutSize[i]; ++xi) // loop4
				{
					x1c = table_X[i][xi];
					for (unsigned int xj = 0; xj<ITR::cutSize[j]; ++xj)        // loop5
					{
						x2c = table_X[j][xj];
						for (unsigned int xk = 0; xk<ITR::cutSize[k]; ++xk) // loop6
						{
							x3c = table_X[k][xk];

							std::fill_n(v, 16, 0.0);
							for (unsigned int p = 0; p<sampleSize; ++p)
							{
								row_Y = var_Y[p];
								row_A = var_A[p];
								v[x1c[p] * 8 + x2c[p] * 4 + x3c[p] * 2 + *row_A] += *row_Y;
							}

							sum[0] = v[1] - v[0];   // 000 (0001-0000)
							sum[1] = v[3] - v[2];   // 001 (0011-0010)
							sum[2] = v[5] - v[4];   // 010 (0101-0100)
							sum[3] = v[7] - v[6];   // 011 (0111-0110)
							sum[4] = v[9] - v[8];   // 100 (1001-1000)
							sum[5] = v[11] - v[10]; // 101 (1011-1010)
							sum[6] = v[13] - v[12]; // 110 (1101-1100)
							sum[7] = v[15] - v[14]; // 111 (1111-1110)

							const auto ptr = max_element(sum, sum + 8);
							double temp = (*ptr + ITR::T0) * 2 / ThreeDepthSearch::sampleSize;
							solutions.push_back(new Res(temp, { i, cutRange1[xi], j,  cutRange2[xj], k, cutRange3[xk] }, distance(sum, ptr)));
						} // end loop 6
					} // end loop 5
				} // end loop 4
			} // end loop 3
		} // end loop 2
	} // end loop 1


	return solutions;
}
