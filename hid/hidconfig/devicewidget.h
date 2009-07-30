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

#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include "hiddevices.h"
#include "hid.h"

class DeviceWidget : public QGroupBox
{
    Q_OBJECT

public:
    DeviceWidget(QWidget *parent, HIDDevice *pDevice);
    ~DeviceWidget();

    void setCriteria( HIDDeviceCriteria *pCriteria );
    void getCriteria( HIDDeviceCriteria *pCriteria );

    private slots:
	void onTestClicked();

private:
    Logger m_Logger;
    HIDDevice *m_pDevice;
    QCheckBox *m_pchkPID;
    QCheckBox *m_pchkVID;
    QCheckBox *m_pchkManufacturer;
    QCheckBox *m_pchkProduct;
    QCheckBox *m_pchkSN;
    QCheckBox *m_pchkSystemId;
};

#endif // DEVICEWIDGET_H
