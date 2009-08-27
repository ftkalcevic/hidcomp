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

#ifndef LCDCONFIGDLG_H
#define LCDCONFIGDLG_H

#include <QWidget>
#include "ui_lcdconfigdlg.h"
#include "spinboxdelegate.h"
#include "comboboxdelegate.h"
#include "hiddevices.h"
#include "hid.h"


enum EDisplayDataType
{
    UnknownType = -1, 
    StringType = 1,
    IntType = 2,
    FloatType = 3
};


class LCDWorkingData
{
public:
    LCDWorkingData( LCDEntry &temp )
        : entry( temp )
    {
        nLastRow = -1;
        nLastCol = -1;
    }

    LCDEntry entry;
    QString sLastText;
    int nLastRow;
    int nLastCol;
};

class LCDWorkingPage
{
public:
    LCDWorkingPage( int n, const QString &s )
    {
        nNumber = n;
        sName = s;
    }
    ~LCDWorkingPage()
    {
        for ( int i = 0; i < Entries.count(); i++ )
            delete Entries[i];
        Entries.clear();
    }

    QString sName;
    int nNumber;
    QList< LCDWorkingData *> Entries;
};



class LCDConfigDlg : public QDialog
{
    Q_OBJECT

public:
    LCDConfigDlg(HIDLCD *lcdData, HIDDevice *pDevice, HID_CollectionPath_t *pCol, int nRows, int nColumns, bool bUserFonts, int nMinFontIndex, int nMaxFontIndex, QWidget *parent = 0);
    ~LCDConfigDlg();

    void setConfig( HIDLCD *lcdData );
    void getConfig( HIDLCD *lcdData );

public slots:
    void onNewPage();
    void onDeletePage();
    void onNewData();
    void onDeleteData();
    void onOK();
    void onCancel();
    void onItemChanged(QTableWidgetItem*);
    void onCurrentItemChanged(QTableWidgetItem*,QTableWidgetItem*);
    void onPageItemChanged(QTableWidgetItem *item);
    void onPageCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous );
    void onPageUp();
    void onPageDown();
    void onEditUserFonts();

private:
    Logger m_Logger;
    HIDDevice *m_pDevice;
    HID_CollectionPath_t *m_pCol;
    Ui::LCDConfigDlgClass ui;
    SpinBoxDelegate *m_pSpinBoxColDelegate;
    SpinBoxDelegate *m_pSpinBoxRowDelegate;
    SpinBoxDelegate *m_pSpinBoxIndexDelegate;
    ComboBoxDelegate *m_pDataTypeDelegate;

    int m_nRows;
    int m_nColumns;
    bool m_bUpdating;
    int m_nSampleRate; 
    LCDWorkingPage *m_pCurrentPage;
    QList<LCDFont *> m_fonts;
    bool m_bUserFonts;
    int m_nMinFontIndex;
    int m_nMaxFontIndex;

    void DisplaySample( int nRow );
    QString FormatData( ELCDDisplayData::ELCDDisplayData data, const QString &sFormat, double dScale, const QVariant &value );
    QString FormatData( const QString *sFormat, ... );
    LCDWorkingData *GetDataPointer( int nRow );
    void SetDataPointer( int nRow, LCDWorkingData *pData );
    void ClearDataTable();
    LCDWorkingPage *GetPagePointer( int nRow );
    void SetPagePointer( int nRow, LCDWorkingPage *pData );
    void InsertNewDataRow( LCDWorkingData *pData );
    void DrawCurrentPage( );
    void NewPageRow( int nRow, LCDWorkingPage *pPage );

    void LCDClear();
    void LCDWrite( int nRow, int nCol, QString sText, bool bHighlight );
    void LCDSendUserFonts();
};

#endif // LCDCONFIGDLG_H

