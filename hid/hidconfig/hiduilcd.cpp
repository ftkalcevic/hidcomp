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
#include "hiduilcd.h"
#include "usages.h"
#include "lcdconfigdlg.h"

HIDUILCD::HIDUILCD(HIDDevice *pDev, HID_CollectionPath_t *col, int nIndex, QGridLayout *layout, QWidget *parent) 
: QWidget( parent ),
  HIDUIBase( HIDItemType::LCD, m_temp, NULL, nIndex ),
  m_pCol( col ),
  m_lcdData(0,0,0,true,"pin"),
  m_pDevice( pDev ), 
  m_Logger( QCoreApplication::applicationName(), "HIDUILCD" )
{
    int nRow = layout->rowCount();
    CreateLCDUI( pDev, layout, nRow );

    // Current value
    QLabel *pLCD = new QLabel( "LCD" );
    layout->addWidget( pLCD, nRow, 0 );

    QPushButton *pButton = new QPushButton( tr("Config") );
    layout->addWidget( pButton, nRow, 5 );
    connect( pButton, SIGNAL(clicked(bool)), this, SLOT(onConfigClicked(bool)));

    m_lcdData.setSamplePeriod( 100 );
    m_lcdData.setLCDProcPort( 0 );
}


HIDUILCD::~HIDUILCD()
{
}

void HIDUILCD::CreateLCDUI( HIDDevice *, QGridLayout *layout, int nRow) 
{
    // UsagePage/Usage
    QString sUsagePage, sUsage;
    Usages::GetUsages( m_pCol->UsagePage, m_pCol->Usage, sUsagePage, sUsage );
    m_lblUsage = new QLabel( QString("%1:%2").arg(sUsagePage, sUsage) );
    layout->addWidget( m_lblUsage, nRow, 1 );

    // String
    QString sName;
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
	m_txtHalPin->setText( "LCD" );
}


void HIDUILCD::onConfigClicked(bool)
{
    // retreive the display attributes
    HIDLCDDevice lcdDevice( m_pDevice, m_pCol );
    if ( !lcdDevice.Init() )
    {
	LOG_MSG( m_Logger, LogTypes::Error, "Failed to initialise LCD device" );
	QMessageBox::critical( this, "LCD Error", "Failed to initialise LCD device" );
	return;
    }

    LCDConfigDlg dlg( &m_lcdData, lcdDevice, this );

    if ( dlg.exec() == QDialog::Accepted )
    {
        dlg.getConfig( &m_lcdData );
    }
}


void HIDUILCD::setConfig( HIDItem *pItem )
{
    HIDUIBase::setConfig( pItem );
    HIDLCD *lcdData = dynamic_cast<HIDLCD *>( pItem );
    m_lcdData = *lcdData;
}


void HIDUILCD::getConfig( HIDItem *pItem )
{
    HIDUIBase::getConfig( pItem );
    HIDLCD *lcdData = dynamic_cast<HIDLCD *>( pItem );
    *lcdData = m_lcdData;
}


QList<QString> HIDUILCD::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "page-select" );
    list.push_back( sPrefix + "." + name() + "." + "pages" );
    list.push_back( sPrefix + "." + name() + "." + "max-page" );

    for ( int p = 0; p < m_lcdData.pages().count(); p++ )
	for ( int e = 0; e < m_lcdData.pages()[p]->data().count(); e++ )
	{
	    LCDEntry *pEntry = m_lcdData.pages()[p]->data()[e];
	    
	    QString s;
	    switch ( pEntry->data() )
	    {
		case ELCDDisplayData::UserBit:	    s = QString("in-bit-%1").arg(pEntry->index()); break;
		case ELCDDisplayData::UserFloat:    s = QString("in-float-%1").arg(pEntry->index()); break;
		case ELCDDisplayData::UserS32:	    s = QString("in-s32-%1").arg(pEntry->index()); break;
		case ELCDDisplayData::UserU32:	    s = QString("in-u32-%1").arg(pEntry->index()); break;
		default: break;
	    }
	    if ( !s.isEmpty() )
	    {
		s = sPrefix + "." + name() + "." + s;
		// We can get duplicates at this point (display fields shown in different places)
		if ( !list.contains( s ) )
		    list.push_back( s );
	    }
	}

    return list;
}

