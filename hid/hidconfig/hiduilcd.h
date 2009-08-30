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

#ifndef _HIDUILCD_H_
#define _HIDUILCD_H_

#include "hiddevices.h"
#include "hiduibase.h"

class HIDUILCD: public QWidget, public HIDUIBase
{
    Q_OBJECT

public:
    HIDUILCD(HIDDevice *pDev, HID_CollectionPath_t *col, int nIndex, QGridLayout *layout, QWidget *parent = NULL);
    ~HIDUILCD();

public slots:
    void onConfigClicked(bool);

private:
    void CreateLCDUI( HIDDevice *pDev, QGridLayout *layout, int nRow);

    virtual void setConfig( HIDItem *pItem );
    virtual void getConfig( HIDItem *pItem );
    virtual void Update() {}
    virtual QList<QString> PinNames( const QString &sPrefix );
    int m_temp;
    HID_CollectionPath_t *m_pCol;
    HIDLCD m_lcdData;
    HIDDevice *m_pDevice;
    Logger m_Logger;
};

#endif 
