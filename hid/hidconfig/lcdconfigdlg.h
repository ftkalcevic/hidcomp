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
    LCDConfigDlg(HIDLCD *lcdData, HIDDevice *pDevice, HID_CollectionPath_t *pCol, int nRows, int nColumns, QWidget *parent = 0);
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

    void DisplaySample( int nRow );
    QString FormatData( ELCDDisplayData::ELCDDisplayData data, const QString &sFormat, const QVariant &value );
    QString FormatData( const QString *sFormat, ... );
    //QString FormatData( const QString &sFormat, const QString &s );
    //QString FormatData( const QString &sFormat, int n );
    //QString FormatData( const QString &sFormat, double d );
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
};

#endif // LCDCONFIGDLG_H

