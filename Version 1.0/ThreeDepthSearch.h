#ifndef THREEDEPTHSEARCH_H
#define THREEDEPTHSEARCH_H

#include "ITR.h"

class ThreeDepthSearch : public ITR
{
    public:
        ThreeDepthSearch(DataGeneration &data, int d):ITR(data,d){};
        virtual ~ThreeDepthSearch(){};
        void searchPrint();
        vector<Res *> searchOutput();
};

#endif // THREEDEPTHSEARCH_H
