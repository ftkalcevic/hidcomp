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

#ifndef SELECTDEVICE_H
#define SELECTDEVICE_H

#include "ui_selectdevicedlg.h"
#include "hiddevices.h"

class SelectDevice : public QDialog
{
	Q_OBJECT

public:
	SelectDevice(HIDDevices &hidDevices, QWidget *parent = 0, Qt::WFlags flags = 0);
	~SelectDevice();

public:
	HIDDevice *m_pSelection;
private:
	//std::vector<HIDDevice *>  m_devices;

private slots:
	void accept();
	void onDeviceSelectionChanged(int);

private:
	Ui::SelectDeviceDlg ui;
	HIDDevices &m_hidDevices;
};

#endif // SelectDevice_H
