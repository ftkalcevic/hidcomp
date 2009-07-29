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
