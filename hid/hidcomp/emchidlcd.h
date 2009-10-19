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

#ifndef __EMCHIDLCD_H__
#define __EMCHIDLCD_H__

#include "emclcditem.h"
#include "lcdproc.h"

class LineChanges
{
    unsigned int m_nChangeLow;
    unsigned int m_nChangeHigh;
    bool m_bChanges;

public:
    LineChanges()
    {
	m_bChanges = false;
    }
    void Change(unsigned int i)
    {
	if ( !m_bChanges )
	{
	    m_bChanges = true;
	    m_nChangeLow = i;
	    m_nChangeHigh = i;
	}
	else if ( i < m_nChangeLow )
	    m_nChangeLow = i;
	else if ( i > m_nChangeHigh )
	    m_nChangeHigh = i;
    }
    bool Changed() const { return m_bChanges; }
    unsigned int Low() const { return m_nChangeLow; }
    unsigned int High() const { return m_nChangeHigh; }
    void Reset() { m_bChanges = false; }
};

class EMCHIDLCD : public EMCHIDItem
{
public:
    EMCHIDLCD(const QString &sPinPrefix, HIDItem *pCfgItem, HID_CollectionPath_t *pCollection );
    virtual ~EMCHIDLCD(void);

    virtual bool CheckOutputs();
    void Refresh( HIDDevice *pDevice );
    void Initialise(HIDDevice *pDevice);

private:
    HID_CollectionPath_t *m_pLCDCollection;
    HID_CollectionPath_t *m_pCharReportCollection;
    byte m_nCharReportID;
    unsigned int m_nFirsDataIndex;
    QList<QString> m_Display;
    QList<LineChanges> m_Changes;
    int m_nPage;
    bool m_bInitialised;
    bool m_bFirst;
    unsigned int m_nRows;
    unsigned int m_nColumns;
    QList<EMCLCDPage *> m_Pages;
    HID_ReportItem_t *m_pRowItem;
    HID_ReportItem_t *m_pColItem;
    HID_ReportItem_t *m_pPixelAddressItem;
    HID_ReportItem_t *m_pClearScreenItem;
    HID_ReportItem_t *m_pBacklightItem;
    HID_ReportItem_t *m_pBackgroundColourItem;
    HID_ReportItem_t *m_pForegroundColourItem;
    HID_ReportItem_t *m_pFontItem;
    HID_ReportItem_t *m_pRectXItem;
    HID_ReportItem_t *m_pRectYItem;
    HID_ReportItem_t *m_pRectWidthItem;
    HID_ReportItem_t *m_pRectHeightItem;
    HID_ReportItem_t *m_pRectFillItem;
    QVarLengthArray<byte> m_Report;
    int m_nReportIdSpace;
    QTime m_timer;
    int m_nRefreshPeriodMS;
    int m_nPort;
    QList<LCDFont *> m_fonts;
    LCDProc *m_pLCDProc;
    bool m_bClearScreen;
    int m_nBackgroundColour;
    int m_nForegroundColour;
    int m_nFont;
    int m_nIntensity;
    int m_nOldBackgroundColour;
    int m_nOldForegroundColour;
    int m_nOldFont;
    int m_nOldIntensity;

    int SendReport( HIDDevice *pDevice, byte nReportId );
    void SendDisplayControlReport(HIDDevice *pDevice);
    void ProcessCommand( HIDDevice *pDevice, LCDCmd *pCmd );
    bool WriteLCDBuffer( unsigned int nRow, int unsigned nCol, const QString &s );
    void ClearLCDBuffer();
    void OutputHIDLCD( HIDDevice *pDevice, unsigned int nRow, unsigned int nCol, const QString &s, unsigned int nPos, unsigned int nLen );
    void HIDQueryDisplayParmeters( HIDDevice *pDevice );
    void LCDSendUserFonts(HIDDevice *pDevice, QList<LCDFont*> &fonts);

    void DoClearScreen(HIDDevice *pDevice);
    void DoSetBackground( HIDDevice *pDevice, LCDSetBackground *pCmd );
    void DoSetForeground( HIDDevice *pDevice, LCDSetForeground *pCmd );
    void DoText( HIDDevice *pDevice, LCDText *pCmd );
    void DoSetFont( HIDDevice *pDevice, LCDSetFont *pCmd );
    void DoSetBacklight( HIDDevice *pDevice, LCDSetBacklight *pCmd );
    void DoRectangle( HIDDevice *pDevice, LCDRectangle *pCmd );
    void UpdateDisplayControl(HIDDevice *pDevice, bool bClearScreen = false);
};


#endif
