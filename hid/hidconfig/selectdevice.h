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
