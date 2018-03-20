#ifndef FLEXDEPTHSEARCH_H
#define FLEXDEPTHSEARCH_H

#include "IProcess.h"


class FlexDepthSearch : public IProcess
{
public:
    FlexDepthSearch(ITR* itr, unsigned int d, bool print = true);
    virtual ~FlexDepthSearch();

    void process();

private:
    unsigned int depth;                /**< Searching depth */
    bool isPrint;                      /**< Execute option, default print */
    unsigned int sampleSize;           /**< Sample Size */
    unsigned int varSize;              /**< Covariate size */
    vector<unsigned int> varType;      /**< Variable type */
    vector<vector<int>> lookup;        /**< variable combinations, combination number x depth */


    vector<vector<int>> combine(int n, int k);
    void searchPrint();
    vector<Res *> searchOutput();
};

#endif // FLEXDEPTHSEARCH_H
