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

#include "stdafx.h"
#include "emchidkeyboard.h"
#include "usages.h"


EMCHIDKeyMap::EMCHIDKeyMap( KeyMap *map, int nPinIndex, HIDKBDevice &device)
    : m_map( map ),
      m_nPinIndex(nPinIndex),
      m_kbDevice(device)
{
    // Make a list of the unset modifiers
    foreach ( unsigned short usage, device.Modifiers().keys() )
    {
        if ( !m_map->Modifiers().contains(usage))
            m_unsetModifiers << usage;
    }


}

bool EMCHIDKeyMap::KeysDown()
{
    foreach ( unsigned short usage, m_map->Keys())
    {
        if ( !m_kbDevice.KeysDown().contains(usage) )
        {
            return false;
        }
    }

    return true;
}

bool EMCHIDKeyMap::ModifiersDown()
{
    // All modifiers must match (Eg if the mapping has no modifiers, then no modifiers can be set)
    foreach ( unsigned short usage, m_map->Modifiers())
    {
        if ( !m_kbDevice.ModifiersDown()[usage] )
            return false;
    }
    foreach ( unsigned short usage, m_unsetModifiers)
    {
        if ( m_kbDevice.ModifiersDown()[usage] )
            return false;
    }
    return true;
}



EMCHIDKeyboard::EMCHIDKeyboard(const QString &sPinPrefix, HIDItem *pCfgItem, HID_CollectionPath_t *pCollection )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDKeyboard", pCfgItem, NULL)
, m_kbDevice(pCollection)
{
    HIDKeyboardMap *pItem = dynamic_cast<HIDKeyboardMap *>(pCfgItem);

    if ( !m_kbDevice.Init() )
    {
        LOG_MSG( m_Logger, LogTypes::Error, "Failed to initialise Keyboard device" );
        qCritical( "Failed to initialise Keyboard device" );
        return;
    }

    foreach ( KeyMap *map, pItem->KeyMappings() )
    {
        QString sPin;
        sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
        sPin += ".";
        sPin += map->PinName();
        Pins.push_back( EMCPin(sPin, HAL_BIT, HAL_OUT) );

        m_map.push_back( new EMCHIDKeyMap( map, Pins.size()-1, m_kbDevice ));
    }
}

EMCHIDKeyboard::~EMCHIDKeyboard(void)
{
    foreach ( EMCHIDKeyMap *map, m_map )
        delete map;
}


void EMCHIDKeyboard::UpdatePin()
{

    if ( m_kbDevice.ProcessKeyboardData() )
    {
        // KB data has changed - Update pins
        foreach ( EMCHIDKeyMap *map, m_map )
        {
            if ( map->KeysDown() && map->ModifiersDown() )
            {
                **(hal_bit_t **)(Pins[map->PinIndex()].pData) = 1;
            }
            else
            {
                **(hal_bit_t **)(Pins[map->PinIndex()].pData) = 0;
            }
        }
    }
}
