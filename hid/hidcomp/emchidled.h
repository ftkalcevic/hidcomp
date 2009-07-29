#ifndef __EMCHIDLED_H__
#define __EMCHIDLED_H__

#include "emchiditem.h"

class EMCHIDLED : public EMCHIDItem
{
public:
    EMCHIDLED(const QString &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem );
    virtual ~EMCHIDLED(void);

    virtual bool CheckOutputs();
    bool m_bState;
};

#endif
