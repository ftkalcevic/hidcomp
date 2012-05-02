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

#ifndef __EMCHIDKEYBOARD_H__
#define __EMCHIDKEYBOARD_H__

#include "hid.h"
#include "hidkbdevice.h"
#include "emcpin.h"
#include "emchiditem.h"

class EMCHIDKeyMap
{
public:
    EMCHIDKeyMap( KeyMap *, int nPinIndex, HIDKBDevice &);
    bool KeysDown();
    bool ModifiersDown();
    int PinIndex() const { return m_nPinIndex; }

private:
    KeyMap *m_map;
    int m_nPinIndex;
    HIDKBDevice &m_kbDevice;
    QList<unsigned short> m_unsetModifiers;
};

class EMCHIDKeyboard: public EMCHIDItem
{
public:
    EMCHIDKeyboard(const QString &sPinPrefix, HIDItem *pCfgItem, HID_CollectionPath_t *pCollection );
    virtual ~EMCHIDKeyboard(void);

    virtual void UpdatePin();

private:
    QList<EMCHIDKeyMap *> m_map;
    HIDKBDevice m_kbDevice;
};


#endif
