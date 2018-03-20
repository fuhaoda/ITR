#ifndef TWODEPTHSEARCH_H
#define TWODEPTHSEARCH_H

#include "IProcess.h"


class TwoDepthSearch : public IProcess
{
public:
    TwoDepthSearch(ITR* itr, bool print = true);
    virtual ~TwoDepthSearch();
    void process();
private:
    unsigned int sampleSize;           /**< Sample Size */
    unsigned int varSize;              /**< Covariate size */
    bool isPrint;                      /**< Execute option, default print */

    void searchPrint();
    vector<Res *> searchOutput();
};

#endif // TWODEPTHSEARCH_H
