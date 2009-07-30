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
#include "selectdevice.h"
#include "scanning.h"
#include "hiddevices.h"



static QString MakeDeviceName( HIDDevice &device )
{
    QString str;
    QTextStream s(&str);

    device.Open();

    if ( device.hasOpenErrors() )
        s << "* ";
    if ( device.Manufacturer().length() > 0 )
        s << device.Manufacturer() << "(" << hex << qSetFieldWidth(4) << qSetPadChar('0') << device.VID() << qSetFieldWidth(0) << ")";
    else
        s << "VID:" << qSetFieldWidth(4) << qSetPadChar('0') << hex << device.VID() << qSetFieldWidth(0);

    if ( device.Product().length() > 0 )
        s << " " << device.Product() << "(" << hex << qSetFieldWidth(4) << qSetPadChar('0') << device.PID() << qSetFieldWidth(0) << ")";
    else
        s << " PID:" << qSetFieldWidth(4) << qSetPadChar('0') << hex << device.PID() << qSetFieldWidth(0);

    if ( device.SerialNumber().length() > 0 )
        s << " S/N:" << device.SerialNumber();

    device.Close();

    return str;
}



SelectDevice::SelectDevice(HIDDevices &hidDevices, QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
, m_pSelection( NULL )
, m_hidDevices( hidDevices )
{
    ui.setupUi(this);

    m_hidDevices.FindHIDDevices();

    std::vector<HIDDevice *>::iterator it;
    for ( it = m_hidDevices.m_Devices.begin(); it != m_hidDevices.m_Devices.end(); it++)
    {
        HIDDevice *pDevice = *it;

        QString s = MakeDeviceName( *pDevice );
        ui.cboDevices->addItem(s);
    }
}

SelectDevice::~SelectDevice(void)
{
}

void SelectDevice::accept()
{
    QDialog::accept();
    //setResult( QDialog::Accepted );
    //close();
}


void SelectDevice::onDeviceSelectionChanged(int n)
{
    m_pSelection = m_hidDevices.m_Devices[n];
}
