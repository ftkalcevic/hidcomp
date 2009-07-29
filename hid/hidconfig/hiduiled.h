#ifndef _HIDUILED_H_
#define _HIDUILED_H_

#include "hiddevices.h"
#include "hiduibase.h"

class HIDUILED: public QObject, public HIDUIBase
{
    Q_OBJECT

public:
    HIDUILED(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent = NULL);
    ~HIDUILED();

    static QPixmap *m_pOn;
    static QPixmap *m_pOff;
    bool m_bOn;
    QCheckBox *m_pLight;
    virtual void Update() {}
    virtual QList<QString> PinNames( const QString &sPrefix );

signals:
    void change( HID_ReportItem_t *m_pItem );

public slots:
    void onStateChanged( int state );
};

#endif 
