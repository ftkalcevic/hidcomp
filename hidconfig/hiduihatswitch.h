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
