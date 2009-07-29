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
