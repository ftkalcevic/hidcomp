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
#include "keyboardconfigdlg.h"
#include "usages.h"

#define DATA_KEYSTROKE_COL 	0
#define DATA_HALPIN_COL		1
#define DATA_COL 	        0


KeyboardConfigDlg::KeyboardConfigDlg(HIDKeyboardMap *pKBData, HIDDevice *pDevice, HIDKBDevice &kbDevice, QWidget *parent)
: QDialog(parent)
, m_Logger( QCoreApplication::applicationName().toAscii().constData(), "KeyboardConfigDlg" )
, m_KBDevice( kbDevice )
, m_bUpdating( false )
{
    ui.setupUi(this);

    ui.tableData->setColumnWidth(DATA_KEYSTROKE_COL, 250);
    ui.tableData->setSelectionMode( QAbstractItemView::SingleSelection );

    QGridLayout *item_layout = new QGridLayout();
    ui.grpModifiers->setLayout( item_layout );

    // Create the modifier keys from the HID Descriptor
    foreach ( const HID_ReportItem_t *item, m_KBDevice.Modifiers() )
    {
	QString sName;
	if ( item->Attributes.StringIndex != 0 )
	{
            sName = Utility::MakeHALSafeName( pDevice->GetDescriptorString( item->Attributes.StringIndex ) );
	}
	if ( sName.isEmpty() )
	{
	    QString sUsagePage, sUsage;
	    Usages::GetUsages( item->Attributes.UsagePage, item->Attributes.Usage, sUsagePage, sUsage );
	    sName = Utility::MakeHALSafeName(sUsage);
	}

	QCheckBox *modifier = new QCheckBox( sName );
	item_layout->addWidget( modifier );
        m_modifiers[item->Attributes.Usage] = modifier;
    }

    connect( ui.btnApplyMapping, SIGNAL(clicked()), this, SLOT(onAddMapping()));
    connect( ui.btnDeleteMapping, SIGNAL(clicked()), this, SLOT(onDeleteMapping()));

    setConfig( pKBData );
}

KeyboardConfigDlg::~KeyboardConfigDlg()
{
    // Delete all user data
    foreach ( KeyMap *map, m_keymap)
        delete map;
}


void KeyboardConfigDlg::onOK()
{
    accept();
}


void KeyboardConfigDlg::onCancel()
{
    reject();
}

void KeyboardConfigDlg::setConfig( HIDKeyboardMap *kbData )
{
    m_bUpdating = true;

    foreach ( KeyMap *map, kbData->KeyMappings() )
    {
        AddMapping( new KeyMap(*map));
    }
}

void KeyboardConfigDlg::getConfig( HIDKeyboardMap *kbData )
{
    kbData->KeyMappings().clear();

    foreach ( KeyMap *map, m_keymap )
    {
        KeyMap *new_map = new KeyMap( *map );
        kbData->KeyMappings() << new_map;
    }
}

void KeyboardConfigDlg::UpdateValue()
{
    LOG_MSG( m_Logger, LogTypes::Debug, QString("UpdateValue") );

    // We don't support key combinations.  Just take the first key.
    if ( m_KBDevice.KeysDown().count() > 0 )
    {
        unsigned short usage = m_KBDevice.KeysDown()[0];
        QString sUsagePage, sUsage;
        Usages::GetUsages( USAGEPAGE_KEYBOARD_KEYPAD, usage, sUsagePage, sUsage );

        ui.txtKey->setText(sUsage);
        m_Key = usage;

        foreach ( const HID_ReportItem_t *item, m_KBDevice.Modifiers() )
        {
            if ( item->Value )
                m_modifiers[item->Attributes.Usage]->setChecked( true );
            else
                m_modifiers[item->Attributes.Usage]->setChecked( false );
        }
    }
}


KeyMap *KeyboardConfigDlg::GetDataPointer( int nRow )
{
    KeyMap *pData = NULL;

    QTableWidgetItem *item  = ui.tableData->item( nRow, DATA_COL );
    if ( item != NULL )
    {
        void *ptr = qvariant_cast<void *>( item->data(Qt::UserRole) );
        pData = reinterpret_cast<KeyMap *>( ptr );
    }
    return pData;
}

void KeyboardConfigDlg::SetDataPointer( int nRow, KeyMap *pData )
{
    QTableWidgetItem *item  = ui.tableData->item( nRow, DATA_COL );
    assert( item != NULL );
    if ( item != NULL )
        item->setData(Qt::UserRole, qVariantFromValue<void *>( pData ) );
}


void KeyboardConfigDlg::AddMapping( KeyMap *map )
{
    ui.tableData->setSortingEnabled( false );

    int nRow = ui.tableData->rowCount();
    ui.tableData->insertRow( nRow );
    ui.tableData->setItem(nRow, DATA_KEYSTROKE_COL, new QTableWidgetItem(map->KeyStrokeName()) );
    ui.tableData->setItem(nRow, DATA_HALPIN_COL, new QTableWidgetItem(map->PinName()) );

    ui.tableData->item(nRow, DATA_KEYSTROKE_COL)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui.tableData->item(nRow, DATA_HALPIN_COL)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    SetDataPointer( nRow, map );
    m_keymap << map;

    ui.tableData->setSortingEnabled( true );
}

QList<unsigned short> KeyboardConfigDlg::GetKeys()
{
    QList<unsigned short> keys;

    if ( m_Key > 0 )
        keys << m_Key;

    return keys;
}


QList<unsigned short> KeyboardConfigDlg::GetModifiers()
{
    QList<unsigned short> modifiers;

    foreach ( const HID_ReportItem_t *item, m_KBDevice.Modifiers() )
    {
        if ( m_modifiers[item->Attributes.Usage]->isChecked() )
            modifiers << item->Attributes.Usage;
    }
    return modifiers;
}

QString KeyboardConfigDlg::GetPinName()
{
    return ui.txtHalPin->text();
}

void KeyboardConfigDlg::onAddMapping()
{
    QList<unsigned short> keys = GetKeys();
    QList<unsigned short> modifiers = GetModifiers();
    QString pinname = GetPinName();

    if ( keys.count() > 0 && pinname.length() > 0 )
    {
        KeyMap *map = new KeyMap( keys, modifiers, pinname );
        AddMapping( map );
    }
}


void KeyboardConfigDlg::onDeleteMapping()
{
    int nRow = ui.tableData->currentRow();
    if ( nRow >= 0 )
    {
        // remove from data set
        KeyMap *map = GetDataPointer( nRow );
        m_keymap.removeOne( map );
        delete map;

        // remove from table
        ui.tableData->removeRow(nRow);
    }
}

