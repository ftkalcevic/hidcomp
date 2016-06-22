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

#ifndef KEYBOARDCONFIGDLG_H
#define KEYBOARDCONFIGDLG_H

#include <QWidget>
#include "ui_keyboardconfigdlg.h"
#include "comboboxdelegate.h"
#include "hiddevices.h"
#include "hid.h"
#include "hidkbdevice.h"




class KeyboardConfigDlg : public QDialog
{
    Q_OBJECT

public:
    KeyboardConfigDlg(HIDKeyboardMap *pKBData, HIDDevice *pDevice, HIDKBDevice &kbDevice, QWidget *parent=0);
    ~KeyboardConfigDlg();

    void setConfig( HIDKeyboardMap *pKBData );
    void getConfig( HIDKeyboardMap *pKBData );
    void UpdateValue();


public slots:
    void onOK();
    void onCancel();
    void onAddMapping();
    void onDeleteMapping();

private:
    void AddMapping( KeyMap *map );
    QList<unsigned short> GetKeys();
    QList<unsigned short> GetModifiers();
    QString GetPinName();
    KeyMap *GetDataPointer( int nRow );
    void SetDataPointer( int nRow, KeyMap *pData );

    Logger m_Logger;
    HIDDevice *m_pDevice;
    HID_CollectionPath_t *m_pCol;
    Ui::KeyboardConfigDlgClass ui;
    QMap<unsigned short, QCheckBox *> m_modifiers;
    QList<KeyMap *> m_keymap;

    HIDKBDevice &m_KBDevice;
    bool m_bUpdating;
    int m_nSampleRate; 
    unsigned short m_Key;

};

#endif // KeyboardConfigDlg_H

