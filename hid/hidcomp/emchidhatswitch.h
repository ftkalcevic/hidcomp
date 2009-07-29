#ifndef _EMCHIDHATSWITCH_H_
#define _EMCHIDHATSWITCH_H_

#include "emchiditem.h"

class EMCHIDHatSwitch : public EMCHIDItem
{
public:
    EMCHIDHatSwitch(const QString &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem );
    virtual ~EMCHIDHatSwitch(void);

    virtual void UpdatePin();
    double m_dScale;
    double m_dPhysMin;
};

#endif
