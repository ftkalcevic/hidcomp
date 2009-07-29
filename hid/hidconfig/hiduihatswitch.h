#ifndef _HIDUIHATSWITCH_H_
#define _HIDUIHATSWITCH_H_

#include "hiddevices.h"
#include "hiduibase.h"
#include "hatswitchwidget.h"

class HIDUIHatswitch: public HIDUIBase
{
public:
    HIDUIHatswitch(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent=NULL);
    ~HIDUIHatswitch(void);

    HatSwitchWidget *m_pHatSwitch;

    virtual void Update();
    virtual QList<QString> PinNames( const QString &sPrefix );
};

#endif
