#pragma once
#include "IProcess.h"
class ThreeDepthSearch : public IProcess
{
public:
	ThreeDepthSearch(ITR* itr, bool print = true);
	virtual ~ThreeDepthSearch();
	void process();
private:
	unsigned int sampleSize;           /**< Sample Size */
	unsigned int varSize;              /**< Covariate size */
	bool isPrint;                      /**< Execute option, default print */

	void searchPrint();
	vector<Res *> searchOutput();
};

