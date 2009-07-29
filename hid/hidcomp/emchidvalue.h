#ifndef __EMCHIDVALUE_H__
#define __EMCHIDVALUE_H__

#include "emchiditem.h"

class EMCHIDValue : public EMCHIDItem
{
public:
    EMCHIDValue(const QString &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem );
    virtual ~EMCHIDValue(void);

    virtual void UpdatePin();
    virtual bool CheckOutputs();

    bool m_bIsEncoder;
    int m_nLastDeviceCount;
    int m_nEncoderCount;
    bool m_bFirst;
};

#endif
