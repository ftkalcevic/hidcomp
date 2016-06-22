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

#ifndef _HIDUIBASE_H_
#define _HIDUIBASE_H_

#include "hid.h"

class HIDUIBase
{
public:
    HIDUIBase( HIDItemType::HIDItemType type, int & value, HID_ReportItem_t *item, int index ) : m_type(type), m_nValue( value ), m_pItem(item), m_nIndex( index ), m_bHALEnabled( false ) {}
    ~HIDUIBase(void) {}

    void CreateCommonUI( HIDDevice *pDev, QGridLayout *layout, int nRow );

public:
    virtual void setEnabled(bool );
    virtual void setName(QString );
    virtual bool enabled();
    virtual QString name();
    HID_ReportItem_t *ReportItem() { return m_pItem; }
    HIDItemType::HIDItemType type() { return m_type; }
    virtual int Index() { return m_nIndex; }
    virtual void setConfig( HIDItem *pItem );
    virtual void getConfig( HIDItem *pItem );
    virtual void Update() {}
    virtual QList<QString> PinNames( const QString &sPrefix ) = 0;

protected:
    QLabel *m_lblUsage;
    QLabel *m_lblName;
    QLineEdit *m_txtHalPin;
    QCheckBox *m_chkHalPin;

    QString m_sPinName;
    HIDItemType::HIDItemType m_type;
    int &m_nValue;
    HID_ReportItem_t *m_pItem;
    int m_nIndex;
    bool m_bHALEnabled;
};


#endif

