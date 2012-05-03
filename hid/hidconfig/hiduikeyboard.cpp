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
#include "hiduikeyboard.h"
#include "keyboardconfigdlg.h"
#include "usages.h"
#include "hidkbdevice.h"

HIDUIKeyboard::HIDUIKeyboard(HIDDevice *pDev, HID_CollectionPath_t *col, int nIndex, QGridLayout *layout, QWidget *parent)
: QWidget( parent ),
  HIDUIBase( HIDItemType::KeyboardMap, m_temp, NULL, nIndex ),
  m_pCol( col ),
  m_kbData(0,0,0,true,"pin"),
  m_pDevice( pDev ),
  m_Logger( QCoreApplication::applicationName(), "HIDUIKeyboard" ),
  m_configDlg(NULL),
  m_pKeys( NULL ),
  m_kbDevice( col )
{
    int nRow = layout->rowCount();
    // Current value
    m_pKeys = new QLineEdit(  );
    layout->addWidget( m_pKeys, nRow, 0 );

    CreateKeyboardUI( pDev, layout, nRow );

    QPushButton *pButton = new QPushButton( tr("Config") );
    layout->addWidget( pButton, nRow, 5 );
    connect( pButton, SIGNAL(clicked(bool)), this, SLOT(onConfigClicked(bool)));

    // Create a keyboard device
    if ( !m_kbDevice.Init() )
    {
        LOG_MSG( m_Logger, LogTypes::Error, "Failed to initialise Keyboard device" );
        QMessageBox::critical( this, "Keyboard Error", "Failed to initialise Keyboard device" );
        return;
    }
}


HIDUIKeyboard::~HIDUIKeyboard()
{
}

void HIDUIKeyboard::CreateKeyboardUI( HIDDevice *, QGridLayout *layout, int nRow)
{
    // UsagePage/Usage
    m_lblUsage = new QLabel( "Keyboard/Keypad Mapping" );
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
	m_txtHalPin->setText( "KB" );
}


void HIDUIKeyboard::onConfigClicked(bool)
{
    KeyboardConfigDlg dlg( &m_kbData, m_pDevice, m_kbDevice, this );
    m_configDlg = &dlg;

    if ( dlg.exec() == QDialog::Accepted )
    {
        dlg.getConfig( &m_kbData );
    }
    m_configDlg = NULL;
}


void HIDUIKeyboard::setConfig( HIDItem *pItem )
{
    HIDUIBase::setConfig( pItem );
    HIDKeyboardMap *data = dynamic_cast<HIDKeyboardMap *>( pItem );
    m_kbData = *data;
}


void HIDUIKeyboard::getConfig( HIDItem *pItem )
{
    HIDUIBase::getConfig( pItem );
    HIDKeyboardMap *data = dynamic_cast<HIDKeyboardMap *>( pItem );
    *data = m_kbData;
}


QList<QString> HIDUIKeyboard::PinNames( const QString &sPrefix )
{
    QList<QString> list;
/*
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
*/
    return list;
}

// The HID Device has changed.  Repaint the display.
void HIDUIKeyboard::Update()
{
    if ( m_kbDevice.ProcessKeyboardData() )
    {
        QString sKeys;

        foreach ( unsigned short usage, m_kbDevice.ModifiersDown().keys() )
        {
            if ( m_kbDevice.ModifiersDown()[usage] )
            {
                QString sUsagePage, sUsage;
                Usages::GetUsages( USAGEPAGE_KEYBOARD_KEYPAD, usage, sUsagePage, sUsage );

                sKeys += sUsage + "+";
            }
        }

        bool bKeysDown = false;
        foreach ( unsigned short usage, m_kbDevice.KeysDown() )
        {
            QString sUsagePage, sUsage;
            Usages::GetUsages( USAGEPAGE_KEYBOARD_KEYPAD, usage, sUsagePage, sUsage );

            if ( bKeysDown )
                sKeys += ",";
            sKeys += sUsage;

            bKeysDown = true;
        }

        m_pKeys->setText( sKeys );
        if ( m_configDlg != NULL )
            m_configDlg->UpdateValue();
    }
}
