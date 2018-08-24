#include "pch.h"
#include "IProcess.h"

IProcess::~IProcess()
{
    //dtor
}

void IProcess::process(){
    myITR->process();
}
