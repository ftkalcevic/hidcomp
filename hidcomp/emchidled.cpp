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

#include "emchidled.h"

#define OUT_BIT     0

EMCHIDLED::EMCHIDLED(const QString  &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDItem", pCfgItem, pDeviceItem)
{
    m_bState = false;

    // Add an input bit
    QString sPin;
    sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
    sPin += ".";
    sPin += pCfgItem->sPinName;
    sPin += ".";
    QString  s = sPin;
    s += "out";
    Pins.push_back( EMCPin(s, HAL_BIT, HAL_IN) );
}

EMCHIDLED::~EMCHIDLED(void)
{
}

bool EMCHIDLED::CheckOutputs()
{
    bool bPinValue = **(hal_bit_t **)(Pins[OUT_BIT].pData) != 0;

    if ( (bPinValue && !m_bState) || (!bPinValue && m_bState) )
    {
        m_bState = bPinValue;
        m_pDeviceItem->Value = m_bState ? 1 : 0;
        return true;            // A change, we need to send an update
    }
    return false;
}
