#ifndef FLEXDEPTHSEARCH_H
#define FLEXDEPTHSEARCH_H

#include "ITR.h"
#include "Res.h"

class FlexDepthSearch : public ITR
{
    public:
        FlexDepthSearch(DataGeneration &data, int d):ITR(data,d){};
        virtual ~FlexDepthSearch(){};
        void searchPrint();
        vector<Res *> searchOutput();
};

#endif // FLEXDEPTHSEARCH_H
