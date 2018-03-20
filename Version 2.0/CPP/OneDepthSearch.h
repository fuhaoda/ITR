#ifndef ONEDEPTHSEARCH_H
#define ONEDEPTHSEARCH_H

#include "IProcess.h"

class OneDepthSearch : public IProcess{
public:
	OneDepthSearch(ITR* itr, bool print = true);
	virtual ~OneDepthSearch();
	void process();
private:
	unsigned int sampleSize;           /**< Sample Size */
	unsigned int varSize;              /**< Covariate size */
	bool isPrint;                      /**< Execute option, default print */

	void searchPrint();
	vector<Res *> searchOutput();
};

#endif // ONEDEPTHSEARCH_H