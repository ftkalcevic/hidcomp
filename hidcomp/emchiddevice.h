// hidcomp/hidconfig, HID device interface for emc
// Copyright (C) 2008, Frank Tkalcevic, www.franksworkshop.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __EMCHIDDEVICE_H__
#define __EMCHIDDEVICE_H__

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
#include "emchidkeyboard.h"
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
