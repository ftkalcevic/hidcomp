#ifndef __EMCHIDOUTPUT_H__
#define __EMCHIDOUTPUT_H__

#include "emchiditem.h"

class EMCHIDOutput : public EMCHIDItem
{
public:
    EMCHIDOutput(const QString &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem );
    virtual ~EMCHIDOutput(void);

    virtual bool CheckOutputs();
    int m_nState;
};

#endif
