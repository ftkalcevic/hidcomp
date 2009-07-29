#ifndef _HIDUIOUTPUTVALUE_H_
#define _HIDUIOUTPUTVALUE_H_

#include "hiddevices.h"
#include "hiduibase.h"

class HIDUIOutputValue: public QObject, public HIDUIBase
{
    Q_OBJECT

public:
    HIDUIOutputValue(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent=NULL);
    ~HIDUIOutputValue(void);

    QSlider *m_pValue;
    virtual void Update() {}
    virtual QList<QString> PinNames( const QString &sPrefix );

signals:
    void change( HID_ReportItem_t *m_pItem );

public slots:
    void onValueChanged( int value );
};


#endif
