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
#include "hiddevices.h"
#include "hidparser.h"
#include "hiduibase.h"
#include "usages.h"


void HIDUIBase::CreateCommonUI( HIDDevice *pDev, QGridLayout *layout, int nRow) 
{
    // UsagePage/Usage
    QString sUsagePage, sUsage;
    Usages::GetUsages( m_pItem->Attributes.UsagePage, m_pItem->Attributes.Usage, sUsagePage, sUsage );
    m_lblUsage = new QLabel( QString("%1:%2").arg(sUsagePage, sUsage) );
    layout->addWidget( m_lblUsage, nRow, 1 );

    // String
    QString sName;
    if ( m_pItem->Attributes.StringIndex != 0 )
	sName = pDev->GetDescriptorString( m_pItem->Attributes.StringIndex );
    m_lblName = new QLabel( sName );
    layout->addWidget( m_lblName, nRow, 2 );

    // PIN Mapping
    m_chkHalPin = new QCheckBox( "HAL pin:" );
    layout->addWidget( m_chkHalPin, nRow, 3 );

    m_txtHalPin = new QLineEdit();
    layout->addWidget( m_txtHalPin, nRow, 4 );

    if ( !m_lblName->text().isEmpty() )
	m_txtHalPin->setText( Utility::MakeHALSafeName(m_lblName->text()) );
    else
	m_txtHalPin->setText( Utility::MakeHALSafeName(sUsage) );
}



bool HIDUIBase::enabled()
{
    return m_chkHalPin->checkState() == Qt::Checked;
}

QString HIDUIBase::name()
{
    return m_txtHalPin->text();
}


void HIDUIBase::setEnabled(bool b)
{
    m_chkHalPin->setChecked( b );
}

void HIDUIBase::setName(QString s)
{
    m_txtHalPin->setText( s );
}

void HIDUIBase::setConfig( HIDItem *pItem )
{
    setEnabled( pItem->bEnabled );
    setName( pItem->sPinName );
}

void HIDUIBase::getConfig( HIDItem *pItem )
{
    pItem->bEnabled = enabled();
    pItem->sPinName = name().toLatin1().constData();
}
