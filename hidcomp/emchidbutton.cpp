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

#include "emchidbutton.h"

#define IN_PIN		0
#define IN_NOT_PIN	1


EMCHIDButton::EMCHIDButton(const QString  &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDButton", pCfgItem, pDeviceItem)
{
    // Add a bit and an inverted bit.
    QString sPin;
    sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
    sPin += ".";
    sPin += pCfgItem->sPinName;
    sPin += ".";
    QString  s = sPin;
    s += "in";
    Pins.push_back( EMCPin(s, HAL_BIT, HAL_OUT) );
    s = sPin;
    s += "in-not";
    Pins.push_back( EMCPin(s, HAL_BIT, HAL_OUT) );
}

EMCHIDButton::~EMCHIDButton(void)
{
}


void EMCHIDButton::UpdatePin()
{
    hal_bit_t n;
    if ( m_pDeviceItem->Value )
	n = 1;
    else
	n = 0;

    **(hal_bit_t **)(Pins[IN_PIN].pData) = n;
    **(hal_bit_t **)(Pins[IN_NOT_PIN].pData) = !n;
}

