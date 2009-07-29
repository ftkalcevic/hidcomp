#ifndef _HIDCONFIG_H
#define _HIDCONFIG_H

#include "ui_hidconfig.h"
#include "hiddevices.h"
#include "hiddatathread.h"
#include "hiduivalue.h"
#include "devicewidget.h"
#include "hid.h"

class hidconfig : public QMainWindow
{
    Q_OBJECT

public:
    hidconfig(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~hidconfig();

    void ProcessCommandline();

private slots:
    void onSelectionChangedHIDDevice();
    void onExit();
    void onOpenFile();
    void onSaveFile();
    void onNew();
    void onDebug();
    void onNewData( QVector<byte> data );
    void outputChanged( HID_ReportItem_t *);
    void onAbout();

private:
    virtual void closeEvent(QCloseEvent *event);
    void openFile( QString sFile );
    void updateWindowTitle();

private:
    std::vector<HIDUIBase *> m_HIDDisplayItems;
    Ui::hidconfigClass ui;
    HIDDataThread *m_pThread;
    HIDDevice *m_device;
    void DisplayDevice();
    DeviceWidget *m_pDeviceCriteria;
    QString m_sLastFile;
    HID *m_pHidCfg;
    Logger m_Logger;
    HIDDevices m_hidDevices;
};

#endif // _HIDCONFIG_H
