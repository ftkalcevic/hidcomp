#ifndef __EMCHIDITEM_H__
#define __EMCHIDITEM_H__

#include "emcpin.h"
#include <vector>
#include "hid.h"
#include "hiddevices.h"
#include "hal.h"

class EMCHIDItem
{
public:
    EMCHIDItem( const QString &sModule, const QString &sComponent, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem );
    virtual ~EMCHIDItem(void);

    Logger m_Logger;
    std::vector<EMCPin> Pins;
    HIDItem *m_pCfgItem;
    HID_ReportItem_t *m_pDeviceItem;

    virtual void UpdatePin() {}
    virtual bool CheckOutputs() { return false; }
};

#endif
