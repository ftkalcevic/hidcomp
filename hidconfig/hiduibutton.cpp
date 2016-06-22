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
#include "hiduibutton.h"
#include "usages.h"

QPixmap *HIDUIButton::m_pOn = NULL;
QPixmap *HIDUIButton::m_pOff = NULL;


HIDUIButton::HIDUIButton(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *) 
: HIDUIBase( HIDItemType::Button, value, item, index )
{
    m_bOn = false;

    if ( m_pOn == NULL )
	m_pOn = new QPixmap( ":hidconfig/GreenLight" );
    if ( m_pOff == NULL )
	m_pOff = new QPixmap( ":hidconfig/RedLight" );

    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // default name will be button.n
    if ( m_lblName->text().isEmpty() )
	m_txtHalPin->setText( QString("Button.%1").arg(m_pItem->Attributes.Usage) );

    // Current value
    m_pLight = new QLabel();
    m_pLight->setPixmap( *m_pOff );
    layout->addWidget( m_pLight, nRow, 0 );
}


HIDUIButton::~HIDUIButton()
{
}


void HIDUIButton::Update()
{
    if ( m_nValue )
    {
	if ( !m_bOn )
	{
	    m_pLight->setPixmap( *m_pOn );
	    m_bOn = true;
	}
    }
    else
    {
	if ( m_bOn )
	{
	    m_pLight->setPixmap( *m_pOff );
	    m_bOn = false;
	}
    }
}


QList<QString> HIDUIButton::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "in" );
    list.push_back( sPrefix + "." + name() + "." + "in-not" );

    return list;
}

