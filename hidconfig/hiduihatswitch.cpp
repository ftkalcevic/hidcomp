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
#include "hiduihatswitch.h"
#include "usages.h"
#include "hatswitchwidget.h"


HIDUIHatswitch::HIDUIHatswitch(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *) 
: HIDUIBase( HIDItemType::Hatswitch, value, item, index )
{
    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // Current value
    m_pHatSwitch = new HatSwitchWidget();
    m_pHatSwitch->setValue( 0 );
    layout->addWidget( m_pHatSwitch, nRow, 0 );
}


HIDUIHatswitch::~HIDUIHatswitch(void)
{
}

void HIDUIHatswitch::Update()
{
    // adjust the value to ...
    // -1, no selection
    // 0 - 7, 0 degrees to 315 degrees
    if ( m_nValue < m_pItem->Attributes.LogicalMinimum )
        m_nValue = -1;
    else if ( m_nValue > m_pItem->Attributes.LogicalMaximum )
        m_nValue = -1;
    else 
        m_nValue -= m_pItem->Attributes.LogicalMinimum;
    m_pHatSwitch->setValue( m_nValue );
}


QList<QString> HIDUIHatswitch::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "raw" );
    list.push_back( sPrefix + "." + name() + "." + "direction" );

    return list;
}

