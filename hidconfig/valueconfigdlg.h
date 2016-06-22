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

#ifndef VALUECONFIGDLG_H
#define VALUECONFIGDLG_H

#include "ui_valueconfigdlg.h"
#include "hid.h"
#include "hidparser.h"

class ValueConfigDlg : public QDialog
{
    Q_OBJECT

public:
    ValueConfigDlg(HID_ReportItem_t *item, QWidget *parent = 0);
    ValueConfigDlg(HID_ReportItem_t *item, QWidget *parent, bool bUseResponseCurve, std::vector<CPoint> &responseCurve, int nLogicalMinOverride, int nLogicalMaxOverride,
        ValueType::ValueType valueType, ValueOutputType::ValueOutputType valueOutputType, double dOutputMin, double dOutputMax, bool bReverseAxis );
    ~ValueConfigDlg();

public:
    void UpdateValue();

    bool useResponseCurve();
    std::vector<CPoint> responseCurve();
    int logicalMinOverride();
    int logicalMaxOverride();
    ValueType::ValueType type();
    ValueOutputType::ValueOutputType outputType();
    double outputMin();
    double outputMax();
    bool reverse();

public slots:
    void onOutputTypeChanged(int);
    void onCPointPositionChanged( int x, int y);
    void onSnapToGridClicked(bool);
    void onCPoint1();
    void onCPoint2();
    void onCPoint3();
    void onCPoint4();
    void onCPointCustom();
    void onLogicalMinEditFinished();
    void onLogicalMaxEditFinished();
    void onUseCPointsClicked(bool);
    void onReverseAxisClicked(bool);

private:
    Ui::ValueConfigDlg ui;
    HID_ReportItem_t *m_pHIDItem;
    std::vector<std::vector<CPoint> > m_Points;
    int m_nWhichPoints;

    void CreateUI();
    void UpdatePoints( int nNewPoints );
    void selectComboItem( QComboBox *pCombo, int nData );
    void EnableCPoints( bool bUse );
};

#endif // VALUECONFIGDLG_H
