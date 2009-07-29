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
    void QueryDisplayParmeters( int &nRows, int &nCols );

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
