#ifndef IPROCESS_H
#define IPROCESS_H

#include "ITR.h"


class IProcess : public ITR
{
public:
    IProcess(ITR* itr):myITR(itr) {};
    virtual ~IProcess();
    virtual void process() = 0;
protected:
    ITR* myITR;
};

#endif // IPROCESS_H
