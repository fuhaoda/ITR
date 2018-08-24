#ifndef LOADDATA_H
#define LOADDATA_H

#include "ITR.h"


class LoadData : public ITR
{
    public:
        LoadData(Data *data);
        virtual ~LoadData();
        void process();
    private:
        Data* m_data;
};

#endif // LOADDATA_H
