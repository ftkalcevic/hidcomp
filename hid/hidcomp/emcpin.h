#ifndef __EMCPIN_H__
#define __EMCPIN_H__

#include <QString>
#include "hal.h"

struct EMCPin
{
    EMCPin( const QString &sName, hal_type_t type, hal_pin_dir_t dir )
    {
        sPinName = sName;
        nHalType = type;
        nHalDir = dir;
        pData = NULL;
    }

    QString sPinName;
    hal_type_t nHalType;
    hal_pin_dir_t nHalDir;
    void **pData;
};

#endif
