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
#include "hiduiled.h"
#include "usages.h"

HIDUILED::HIDUILED(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent) 
: QObject( parent )
, HIDUIBase( HIDItemType::LED, value, item, index )
{
    m_bOn = false;

    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // Current value
    m_pLight = new QCheckBox();
    layout->addWidget( m_pLight, nRow, 0 );
    connect(m_pLight, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
}


HIDUILED::~HIDUILED()
{
}


void HIDUILED::onStateChanged( int state )
{
    m_pItem->Value = (state == Qt::Checked);
    emit change( m_pItem );
}



QList<QString> HIDUILED::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "out" );

    return list;
}

