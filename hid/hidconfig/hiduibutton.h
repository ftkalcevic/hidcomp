#ifndef _HIDUIBUTTON_H_
#define _HIDUIBUTTON_H_

#include "hiddevices.h"
#include "hiduibase.h"

class HIDUIButton: public HIDUIBase
{
public:
    HIDUIButton(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent = NULL);
    ~HIDUIButton();

    virtual void Update();
    virtual QList<QString> PinNames( const QString &sPrefix );

    static QPixmap *m_pOn;
    static QPixmap *m_pOff;
    bool m_bOn;
    QLabel *m_pLight;
};

#endif 
