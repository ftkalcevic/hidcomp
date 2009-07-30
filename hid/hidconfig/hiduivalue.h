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

#ifndef _HIDUIVALUE_H_
#define _HIDUIVALUE_H_

#include "hiddevices.h"
#include "hiduibase.h"
#include "valueconfigdlg.h"

class HIDUIValue: public QWidget, public HIDUIBase
{
    Q_OBJECT

public:
    HIDUIValue(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent=NULL);
    ~HIDUIValue(void);

    virtual void Update();
    virtual void setConfig( HIDItem *pItem );
    virtual void getConfig( HIDItem *pItem );
    virtual QList<QString> PinNames( const QString &sPrefix );

public slots:
    void onConfigClicked(bool);

signals:
    void configValue( HIDUIValue *);

private:
    QProgressBar *m_pValue;

    ValueConfigDlg *m_pValueConfigDlg;
    bool m_bUseResponseCurve;
    std::vector<CPoint> m_responseCurve;
    int m_nLogicalMinOverride;
    int m_nLogicalMaxOverride;
    ValueType::ValueType m_ValueType;
    ValueOutputType::ValueOutputType m_ValueOutputType;
    double m_dOutputMin;
    double m_dOutputMax;
    bool m_bReverseAxis;

    void CreateUI( HIDDevice *pDev, QGridLayout *layout, int nRow );
};


#endif
