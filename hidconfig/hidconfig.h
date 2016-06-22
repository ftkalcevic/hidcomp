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

#ifndef _HIDCONFIG_H
#define _HIDCONFIG_H

#include "ui_hidconfig.h"
#include "hiddevices.h"
#include "hiddatathread.h"
#include "hiduivalue.h"
#include "devicewidget.h"
#include "hid.h"
#include "mru.h"

class hidconfig : public QMainWindow
{
    Q_OBJECT

public:
    hidconfig(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~hidconfig();

    void ProcessCommandline();

private slots:
    void onSelectionChangedHIDDevice();
    void onOpenFile();
    void onSaveFile();
    void onNew();
    void onDebug();
    void onNewData( QVector<byte> data );
    void outputChanged( HID_ReportItem_t *);
    void onAbout();
    void onExit();
    void onMRUSelected(const QString &sFile);

private:
    virtual void closeEvent(QCloseEvent *event);
    void openFile( QString sFile );
    void updateWindowTitle();
    bool SaveChanges();
    bool DoSave();
    void writeSettings();
    void readSettings();
    void SetLoaded();

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
    MRU m_MRU;
    QSettings m_Settings;
};

#endif // _HIDCONFIG_H
