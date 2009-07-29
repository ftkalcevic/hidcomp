#ifndef __EMCHIDBUTTON_H__
#define __EMCHIDBUTTON_H__

#include "emchiditem.h"

class EMCHIDButton : public EMCHIDItem
{
public:
    EMCHIDButton(const QString &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem );
    virtual ~EMCHIDButton(void);

    virtual void UpdatePin();
};

#endif
