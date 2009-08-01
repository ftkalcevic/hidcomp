#ifndef __EMCHIDDEVICE_H__
#define __EMCHIDDEVICE_H__

#include <QThread>
#include "emcpin.h"
#include <vector>
#include "hid.h"
#include "hiddevices.h"
#include "hal.h"
#include "emchiditem.h"
#include "emchidbutton.h"
#include "emchidvalue.h"
#include "emchidhatswitch.h"
#include "emchidled.h"
#include "emchidoutput.h"
#include "emchidlcd.h"
#include "emchiddevice.h"

class EMCHIDDevice: public QThread
{
public:
    EMCHIDDevice( const QString sModuleName, const QString sPinPrefix, const QString sFilename );
    ~EMCHIDDevice(void);

    Logger m_Logger;
    HID *m_pHidCfg;
    HIDDevice *m_pDevice;
    volatile bool m_bRun;
    QString m_sModuleName;
    QString m_sFileName;

    QVector<EMCHIDItem *> hid_objects;
    virtual void run();
    void stop();
};

#endif
