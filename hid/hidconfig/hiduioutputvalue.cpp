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
#include "hiduioutputvalue.h"
#include "usages.h"

HIDUIOutputValue::HIDUIOutputValue(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent) 
: QObject(parent), 
  HIDUIBase( HIDItemType::OutputValue, value, item, index )
{
    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // Current value
    m_pValue = new QSlider(Qt::Horizontal);
    m_pValue->setRange( item->Attributes.LogicalMinimum, item->Attributes.LogicalMaximum );
    layout->addWidget( m_pValue, nRow, 0 );

    connect(m_pValue, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}


HIDUIOutputValue::~HIDUIOutputValue(void)
{
}



void HIDUIOutputValue::onValueChanged( int value )
{
    m_pItem->Value = value;
    emit change( m_pItem );
}


QList<QString> HIDUIOutputValue::PinNames( const QString & )
{
    QList<QString> list;
    return list;
}

