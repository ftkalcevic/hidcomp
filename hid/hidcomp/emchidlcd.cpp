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

#include "stdafx.h"
#include "emchidlcd.h"
#include "usages.h"

#define IN_PAGE_SELECT		    0
#define OUT_PAGES		    1
#define OUT_MAX_PAGE		    2

#define RGB(r,g,b) ( ((uint16_t)((b)>>3) & 0x1F) | ((((uint16_t)((g)>>3)) & 0x1f) << 5) | (((uint16_t)((r)>>3) & 0x1f) << 11) )


EMCHIDLCD::EMCHIDLCD(const QString &sPinPrefix, HIDItem *pCfgItem, HID_CollectionPath_t *pCollection )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDItem", pCfgItem, NULL)
, m_pLCDCollection( pCollection )
, m_pCharReportCollection( NULL )
, m_nPage( 0xFFFFFFFF )
, m_bInitialised( false )
, m_bFirst( true )
, m_pRowItem( NULL )
, m_pColItem( NULL )
, m_pPixelAddressItem( NULL )
, m_pClearScreenItem( NULL )
, m_pBacklightItem( NULL )
, m_pBackgroundColourItem( NULL )
, m_pForegroundColourItem( NULL )
, m_pFontItem( NULL )
, m_pRectXItem( NULL )
, m_pRectYItem( NULL )
, m_pRectWidthItem( NULL )
, m_pRectHeightItem( NULL )
, m_pRectFillItem( NULL )
, m_pLCDProc( NULL )
, m_bClearScreen( false )	// should read-back defaults from device
, m_nBackgroundColour( 0 )
, m_nForegroundColour( 0xFFFF )
, m_nFont( 0 )
, m_nIntensity( 50 )
, m_nOldBackgroundColour( 0 )
, m_nOldForegroundColour( 0xFFFF )
, m_nOldFont( 0 )
, m_nOldIntensity( 50 )
{
    HIDLCD *pItem = dynamic_cast<HIDLCD *>(pCfgItem);

    m_nRefreshPeriodMS = pItem->samplePeriod();
    m_nPort = pItem->LCDProcPort();

    QString sPin;
    sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
    sPin += ".";
    sPin += pCfgItem->sPinName;
    sPin += ".";

    // Add page select pin
    QString  s = sPin;
    s += "page-select";
    Pins.push_back( EMCPin(s, HAL_S32, HAL_IN) );

    // Add page count
    s = sPin;
    s += "pages";
    Pins.push_back( EMCPin(s, HAL_S32, HAL_OUT) );

    // Add page count
    s = sPin;
    s += "max-page";
    Pins.push_back( EMCPin(s, HAL_S32, HAL_OUT) );

    // Build working Page data - make user pins as we come across them
    QMap<QString,int> pin_list;
    for ( int nPage = 0; nPage < pItem->pages().count(); nPage++ )
    {
	LCDPage *pLCDPage = pItem->pages()[nPage];
	EMCLCDPage *pPage = new EMCLCDPage();
	
	for ( int nEntry = 0; nEntry < pLCDPage->data().count(); nEntry++ )
	{
	    LCDEntry *pLCDEntry = pLCDPage->data()[nEntry];
	    EMCLCDItem *pEntry = new EMCLCDItem();

	    pEntry->m_eData = pLCDEntry->data();
	    pEntry->m_nIndex = pLCDEntry->index();
	    pEntry->m_nRow = pLCDEntry->row();
	    pEntry->m_nCol = pLCDEntry->col();
	    pEntry->ProcessFormatString( pLCDEntry );
	    pEntry->m_dScale = pLCDEntry->scale();
	    pEntry->Init();

	    // build user pins - duplicates can appear but that means pins are shared
	    switch ( pEntry->m_eData )
	    {
		case ELCDDisplayData::UserBit:
		{
		    QString s = sPin + QString("in-bit-%1").arg(pEntry->m_nIndex);
		    if ( pin_list.contains( s ) )
			pEntry->m_nPinIndex = pin_list[s];
		    else
		    {
			pin_list.insert( s, (int)Pins.size() );
			pEntry->m_nPinIndex = (int)Pins.size();
			Pins.push_back( EMCPin(s, HAL_BIT, HAL_IN) );
		    }
		    break;
		}
		case ELCDDisplayData::UserFloat:
		{
		    QString s = sPin + QString("in-float-%1").arg(pEntry->m_nIndex);
		    if ( pin_list.contains( s ) )
			pEntry->m_nPinIndex = pin_list[s];
		    else
		    {
			pin_list.insert( s, (int)Pins.size() );
			pEntry->m_nPinIndex = (int)Pins.size();
			Pins.push_back( EMCPin(s, HAL_FLOAT, HAL_IN) );
		    }
		    break;
		}
		case ELCDDisplayData::UserS32:
		{
		    QString s = sPin + QString("in-s32-%1").arg(pEntry->m_nIndex);
		    if ( pin_list.contains( s ) )
			pEntry->m_nPinIndex = pin_list[s];
		    else
		    {
			pin_list.insert( s, (int)Pins.size() );
			pEntry->m_nPinIndex = (int)Pins.size();
			Pins.push_back( EMCPin(s, HAL_S32, HAL_IN) );
		    }
		    break;
		}
		case ELCDDisplayData::UserU32:
		{
		    QString s = sPin + QString("in-u32-%1").arg(pEntry->m_nIndex);
		    if ( pin_list.contains( s ) )
			pEntry->m_nPinIndex = pin_list[s];
		    else
		    {
			pin_list.insert( s, (int)Pins.size() );
			pEntry->m_nPinIndex = (int)Pins.size();
			Pins.push_back( EMCPin(s, HAL_U32, HAL_IN) );
		    }
		    break;
		}
		default:
		    break;
	    }
	    pPage->m_Items.append( pEntry );
	}
	m_Pages.append( pPage );
    }

    foreach (LCDFont *pFont, pItem->fonts() )
    	m_fonts.push_back( new LCDFont(*pFont) );
}

EMCHIDLCD::~EMCHIDLCD(void)
{
    for ( int i = 0; i < m_Pages.count(); i++ )
	delete m_Pages[i];
    m_Pages.clear();
}


void EMCHIDLCD::Initialise(HIDDevice *pDevice)
{
    // Query the device for rows and columns
    HIDQueryDisplayParmeters( pDevice );

    if ( m_fonts.count() > 0 )
        LCDSendUserFonts(pDevice, m_fonts);

    // Force timer start to one minute ago
    m_timer = m_timer.addSecs(-60);

    for ( unsigned int i = 0; i < m_nRows; i++ )
    {
        m_Display.append( QString( m_nColumns, QChar(' ') ) );
	m_Changes.append( LineChanges() );
    }

    // Find what features the LCD has.
    // clear screen
    // bg/fg
    // font
    // backlight
    // rectangle

    // currently we only support lcds that have row/col/data (optionally pixel address) in the Character Report
    m_pRowItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_CHARACTER_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_ROW );
    m_pColItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_CHARACTER_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_COLUMN );
    m_pPixelAddressItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_CHARACTER_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_CURSOR_PIXEL_POSITIONING );
    if ( m_pRowItem == NULL || m_pColItem == NULL )
    {
	LOG_MSG( m_Logger, LogTypes::Error, "Failed to locate Row and Column entries in LCD report" );
	m_pRowItem = NULL;
	m_pColItem = NULL;
	return;
    }
    m_pCharReportCollection = m_pColItem->CollectionPath;
    m_nCharReportID = m_pColItem->ReportID;

    m_pClearScreenItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_CONTROL_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_CLEAR_DISPLAY );
    m_pBacklightItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_CONTROL_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_BRIGHTNESS );
    m_pBackgroundColourItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_CONTROL_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_BACKGROUND_COLOUR );
    m_pForegroundColourItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_CONTROL_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_FOREGROUND_COLOUR );
    m_pFontItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_CONTROL_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_SELECT_FONT );
    
    m_pRectXItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DRAW_RECT_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_RECT_X );
    m_pRectYItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DRAW_RECT_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_RECT_Y );
    m_pRectWidthItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DRAW_RECT_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_RECT_WIDTH );
    m_pRectHeightItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DRAW_RECT_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_RECT_HEIGHT );
    m_pRectFillItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DRAW_RECT_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_RECT_FILLED );

    // Find the index to the first data item
    m_nFirsDataIndex = 0;
    for ( ; m_nFirsDataIndex < m_pCharReportCollection->ReportItems.size(); m_nFirsDataIndex++ )
	if ( m_pCharReportCollection->ReportItems[m_nFirsDataIndex]->Attributes.Usage == USAGE_DISPLAY_DATA )
	    break;

    if ( m_nFirsDataIndex >= m_pCharReportCollection->ReportItems.size() )
    {
	LOG_MSG( m_Logger, LogTypes::Error, "Failed to locate Display Data entry in LCD report" );
	m_pRowItem = NULL;
	m_pColItem = NULL;
	return;
    }

    // Create the report buffer
    HID_ReportDetails_t pReportDetails = pDevice->ReportInfo().Reports[m_nCharReportID];
    int nBufLen = pReportDetails.OutReportLength;
    m_nReportIdSpace = 0;
    if ( pDevice->ReportInfo().Reports.size() > 1 )
	m_nReportIdSpace=1;

    m_Report.resize( nBufLen + m_nReportIdSpace );
    if ( m_nReportIdSpace )
	m_Report[0] = m_nCharReportID;

    HIDLCD *pItem = dynamic_cast<HIDLCD *>(m_pCfgItem);
    **(hal_s32_t **)(Pins[OUT_PAGES].pData) = pItem->pages().count();
    **(hal_s32_t **)(Pins[OUT_MAX_PAGE].pData) = pItem->pages().count()-1;

    if ( m_nPort > 0 )
    {
	m_pLCDProc = new LCDProc( m_nPort );
	if ( !m_pLCDProc->Initialise() )
	{
	    LOG_MSG( m_Logger, LogTypes::Error, "Failed to initialise LCD Listen Port" );
	    delete m_pLCDProc;
	    m_pLCDProc = NULL;
	    return;
	}
    }

    m_bInitialised = true;
}

bool EMCHIDLCD::CheckOutputs()
{
    return false;
}


void EMCHIDLCD::Refresh( HIDDevice *pDevice )
{
    if ( !m_bInitialised )
	return;

    bool bRedrawAll = false;
    bool bUpdate = false;
    HIDLCD *pItem = dynamic_cast<HIDLCD *>(m_pCfgItem);

    if ( m_bFirst )
	bRedrawAll = true;

    if ( m_timer.elapsed() < m_nRefreshPeriodMS && !m_bFirst )
	return;

    m_timer.restart();

    for ( unsigned int i = 0; i < m_nRows; i++ )
	m_Changes[i].Reset();

    // if the page changes, re load all.
    int nPage = **(hal_s32_t **)(Pins[IN_PAGE_SELECT].pData);
    if ( nPage != m_nPage )
    {
	LOG_MSG( m_Logger, LogTypes::Debug, QString("Page change from %1 to %2").arg(m_nPage).arg(nPage) );

	m_nPage = nPage;
	if ( nPage < 0 || m_nPage >= pItem->pages().count() )
	    m_nPage = 0;
	bRedrawAll = true;
	ClearLCDBuffer();
    }

    // Update EMC statuses - iterate through items on the current page to see if any thing has changed.
    if ( m_Pages.count() > 0 )
    {
	EMCLCDPage *pPage = m_Pages[m_nPage];
	int nEntries = pPage->m_Items.count();
	for ( int i = 0; i < nEntries; i++ )
	{
	    EMCLCDItem *pEntry = pPage->m_Items[i];
	    if ( pEntry->CheckDataChange(Pins,bRedrawAll) )	// Changed Data
	    {
		// Format it for display.  
		// Write it to the local buffer.  If it is different, flag it.
		if ( WriteLCDBuffer( pEntry->m_nRow, pEntry->m_nCol, pEntry->m_sLastFormattedData ) )
		    bUpdate = true;
	    }
	}
    }

    // anthing changes?
    if ( bRedrawAll )
    {
	for ( unsigned int i = 0; i < m_nRows; i++ )
	    OutputHIDLCD( pDevice, i, 0, m_Display[i], 0, m_nColumns );
    } 
    else if ( bUpdate )
    {
	for ( unsigned int i = 0; i < m_nRows; i++ )
	{
	    LineChanges &changes = m_Changes[i];
	    if ( changes.Changed() )
		OutputHIDLCD( pDevice, i, changes.Low(), m_Display[i], changes.Low(), changes.High() - changes.Low() + 1);
	}
    }

    // check the socket
    if ( m_pLCDProc != NULL )
    {
	while ( m_pLCDProc->queueLength() > 0 )
	{
	    LCDCmd *pCmd = m_pLCDProc->getQueueMessage();
	    if ( pCmd != NULL )
	    {
		ProcessCommand( pDevice, pCmd );
		delete pCmd;
	    }
	}
    }
    m_bFirst = false;
}

bool EMCHIDLCD::WriteLCDBuffer( unsigned int nRow, int unsigned nCol, const QString &s )
{
    if ( nRow >= m_nRows )
	return false;
    if ( nCol >= m_nColumns )
	return false;

    unsigned int nLen = s.length();
    if ( nCol + nLen > m_nColumns )
	nLen = m_nColumns - nCol;

    QString &sLine = m_Display[nRow];
    LineChanges &changes = m_Changes[nRow];

    bool bChange = false;
    for ( unsigned int i = 0; i < nLen; i++, nCol++ )
    {
	QChar cNew = s[i];
	QChar cOld = sLine[nCol];

	if ( cNew != cOld )
	{
	    bChange = true;
	    sLine[nCol] = cNew;
	    changes.Change(nCol);
	}
    }

    return bChange;
}


void EMCHIDLCD::ClearLCDBuffer()
{
    for ( unsigned int i = 0; i < m_nRows; i++ )
	m_Display[i] = QString( m_nColumns, QChar(' ') );
}


void EMCHIDLCD::OutputHIDLCD( HIDDevice *pDevice, unsigned int nRow, unsigned int nCol, const QString &sText, unsigned int nPos, unsigned int nLen )
{
    if ( m_pRowItem == NULL || m_pColItem == NULL )
	return;

    // Set report attributes
    m_pRowItem->Value = nRow;
    m_pColItem->Value = nCol;

    // copy the changed text to the report
    for ( unsigned int i = 0; i < nLen && m_nFirsDataIndex + i < m_pCharReportCollection->ReportItems.size(); i++ )
	m_pCharReportCollection->ReportItems[m_nFirsDataIndex + i]->Value = sText[nPos + i].toAscii();

    // null terminate the string if the buffer isn't full
    if ( nPos + nLen < m_nColumns )
	m_pCharReportCollection->ReportItems[m_nFirsDataIndex + nLen]->Value = 0;

    HIDParser parser;
    parser.MakeOutputReport( m_Report.data() + m_nReportIdSpace, (byte)(m_Report.count() - m_nReportIdSpace), m_pCharReportCollection->ReportItems, m_nCharReportID );

    // Send the report
    int nRet = pDevice->AsyncInterruptWrite( m_Report.data(), m_Report.count() );
    LOG_MSG( m_Logger, LogTypes::Debug, QString("interrupt write returned %1\n").arg(nRet).toLatin1().constData() );
}



void EMCHIDLCD::HIDQueryDisplayParmeters( HIDDevice *pDevice )
{
    // default
    m_nRows = 16;
    m_nColumns = 2;

    // find the feature report that contains the rows and columns count.  That's all we want.
    HID_ReportItem_t *pRowItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Feature, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_ATTRIBUTES_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_ROWS );
    HID_ReportItem_t *pColItem = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Feature, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_ATTRIBUTES_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_COLUMNS );

    if ( pRowItem == NULL || pColItem == NULL )
    {
	LOG_MSG(m_Logger, LogTypes::Error, "Failed to find Rows or Columns fields in the display attributes report." );
	return;
    }

    HID_ReportDetails_t pReportDetails = pDevice->ReportInfo().Reports[pRowItem->ReportID];
    int nBufLen = pReportDetails.FeatureReportLength;
    int nOffset = 0;
    if ( pDevice->ReportInfo().Reports.size() > 1 )
	nOffset=1;

    QVarLengthArray<byte> buf;
    buf.resize(nBufLen+nOffset);

    bool b = pDevice->GetReport( pRowItem->ReportID, REPORT_ITEM_TYPE_Feature, buf.data(), buf.size() );
    if ( !b ) 
    {
        LOG_MSG(m_Logger, LogTypes::Warning, QString("Failed to retreive feature LCD Attributes Feature Report.  Can't determine LCD rows and columns from device.") );
    }
    else
    {
	HIDParser parser;
	parser.DecodeReport( buf.data()+nOffset, (byte)buf.size(), pDevice->ReportInfo().ReportItems, pRowItem->ReportID, REPORT_ITEM_TYPE_Feature );
	m_nRows = pRowItem->Value;
	m_nColumns = pColItem->Value;
    }
}


void EMCHIDLCD::LCDSendUserFonts(HIDDevice *pDevice, QList<LCDFont*> &fonts)
{
    HID_ReportItem_t *pCharIndex = pDevice->ReportInfo().FindReportItem( m_pLCDCollection, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_FONT_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_DATA );
    if ( pCharIndex == NULL )
    {
	LOG_MSG( m_Logger, LogTypes::Error, "Failed to find USAGE_DISPLAY_DATA in font report" );
	return;
    }
    HID_CollectionPath_t *pCollection = pCharIndex->CollectionPath;
    byte nReportId = pCharIndex->ReportID;

    // Find the index to the first data item
    unsigned int nIndex = 0;
    for ( ; nIndex < pCollection->ReportItems.size(); nIndex++ )
	if ( pCollection->ReportItems[nIndex]->Attributes.Usage == USAGE_FONT_DATA )
	    break;

    foreach (LCDFont *pFont, fonts )
    {
	pCharIndex->Value = pFont->index();

	for ( int i = 0; i < pFont->data().count() && nIndex + i < pCollection->ReportItems.size(); i++ )
	    pCollection->ReportItems[nIndex + i]->Value = pFont->data()[i];

	HID_ReportDetails_t pReportDetails = pDevice->ReportInfo().Reports[nReportId];
	int nBufLen = pReportDetails.OutReportLength;
	int nOffset = 0;
	if ( pDevice->ReportInfo().Reports.size() > 1 )
	    nOffset=1;

	QVarLengthArray<byte> buf;
	buf.resize(nBufLen+nOffset);
	if ( nOffset )
	    buf[0] = nReportId;

	HIDParser parser;
	parser.MakeOutputReport( buf.data() + nOffset, (byte)nBufLen, pDevice->ReportInfo().ReportItems, nReportId );

	// Send the report
	int nRet = pDevice->InterruptWrite( buf.data(), nBufLen + nOffset, USB_TIMEOUT );
	LOG_MSG( m_Logger, LogTypes::Debug, QString("interrupt write returned %1\n").arg(nRet).toLatin1().constData() );
    }
}


void EMCHIDLCD::ProcessCommand( HIDDevice *pDevice, LCDCmd *pCmd )
{
    switch ( pCmd->nCmd )
    {
	case ELCDCmd::ClearScreen:	DoClearScreen(pDevice); break;
	case ELCDCmd::SetBackground:	DoSetBackground( pDevice, (LCDSetBackground *)pCmd ); break;
	case ELCDCmd::SetForeground:	DoSetForeground( pDevice, (LCDSetForeground *)pCmd ); break;
	case ELCDCmd::Text:		DoText( pDevice, (LCDText *)pCmd ); break;
	case ELCDCmd::SetFont:		DoSetFont( pDevice, (LCDSetFont *)pCmd ); break;
	case ELCDCmd::SetBacklight:	DoSetBacklight( pDevice, (LCDSetBacklight *)pCmd ); break;
	case ELCDCmd::Rectangle:	DoRectangle( pDevice, (LCDRectangle *)pCmd ); break;
    }
}


void EMCHIDLCD::DoClearScreen(HIDDevice *pDevice) 
{
    LOG_DEBUG( m_Logger, "DoClearScreen" );
    if ( m_pClearScreenItem != NULL )
    {
	m_bClearScreen = true;
	UpdateDisplayControl( pDevice, true );
    }
}

void EMCHIDLCD::DoSetBackground( HIDDevice *, LCDSetBackground *pCmd )
{
    LOG_DEBUG( m_Logger, QString("DoSetBackground %1 %2 %3").arg(pCmd->r).arg(pCmd->g).arg(pCmd->b) );
    if ( m_pBackgroundColourItem != NULL )
    {
	m_nBackgroundColour = RGB( pCmd->r, pCmd->g, pCmd->b );
    }
}

void EMCHIDLCD::DoSetForeground( HIDDevice *, LCDSetForeground *pCmd )
{
    LOG_DEBUG( m_Logger, QString("DoSetForeground %1 %2 %3").arg(pCmd->r).arg(pCmd->g).arg(pCmd->b) );
    if ( m_pForegroundColourItem != NULL )
    {
	m_nForegroundColour = RGB( pCmd->r, pCmd->g, pCmd->b );
    }
}

void EMCHIDLCD::UpdateDisplayControl( HIDDevice *pDevice, bool bClearScreen )
{
    if ( bClearScreen ||
         m_nOldBackgroundColour == m_nBackgroundColour ||
         m_nOldForegroundColour == m_nForegroundColour ||
         m_nOldFont == m_nFont ||
         m_nOldIntensity == m_nIntensity )
    {
	SendDisplayControlReport( pDevice );
    }
    
     m_nOldBackgroundColour = m_nBackgroundColour;
     m_nOldForegroundColour = m_nForegroundColour;
     m_nOldFont = m_nFont;
     m_nOldIntensity = m_nIntensity;
}


// This function is used by the external socket connection methods.  There is no
// optimisaton or checking of coordinate ranges.
void EMCHIDLCD::DoText( HIDDevice *pDevice, LCDText *pCmd ) 
{
    LOG_DEBUG( m_Logger, QString("DoText %1 %2 %3 %4").arg(pCmd->x).arg(pCmd->y).arg(pCmd->pixel_coord).arg(pCmd->s) );

    UpdateDisplayControl(pDevice);

    if ( m_pRowItem == NULL || m_pColItem == NULL )
	return;

    // Set report attributes
    m_pRowItem->Value = pCmd->y;
    m_pColItem->Value = pCmd->x;
    if ( m_pPixelAddressItem != NULL )
	m_pPixelAddressItem->Value = pCmd->pixel_coord;

    // copy the changed text 
    unsigned nLen = pCmd->s.length();
    for ( unsigned int i = 0; i < nLen && m_nFirsDataIndex + i < m_pCharReportCollection->ReportItems.size(); i++ )
	m_pCharReportCollection->ReportItems[m_nFirsDataIndex + i]->Value = pCmd->s[i].toAscii();

    // null terminate the string if the buffer isn't full
    if ( nLen < m_nColumns )
	m_pCharReportCollection->ReportItems[m_nFirsDataIndex + nLen]->Value = 0;

    HIDParser parser;
    parser.MakeOutputReport( m_Report.data() + m_nReportIdSpace, (byte)(m_Report.count() - m_nReportIdSpace), m_pCharReportCollection->ReportItems, m_nCharReportID );

    // Send the report
    int nRet = pDevice->AsyncInterruptWrite( m_Report.data(), m_Report.count() );
    LOG_MSG( m_Logger, LogTypes::Debug, QString("interrupt write returned %1\n").arg(nRet).toLatin1().constData() );
}

void EMCHIDLCD::DoSetFont( HIDDevice *, LCDSetFont *pCmd )
{
    LOG_DEBUG( m_Logger, QString("DoFont %1").arg(pCmd->font) );
    if ( m_pFontItem != NULL )
    {
	m_nFont = pCmd->font;
    }
}

void EMCHIDLCD::DoSetBacklight( HIDDevice *, LCDSetBacklight *pCmd )
{
    LOG_DEBUG( m_Logger, QString("DoSetBacklight %1").arg(pCmd->intensity) );
    if ( m_pBacklightItem != NULL )
    {
	m_nIntensity = pCmd->intensity;
    }
}

void EMCHIDLCD::DoRectangle( HIDDevice *pDevice, LCDRectangle *pCmd ) 
{
    UpdateDisplayControl(pDevice);

    if ( m_pRectXItem == NULL &&
	 m_pRectYItem == NULL &&
	 m_pRectWidthItem == NULL &&
	 m_pRectHeightItem == NULL &&
	 m_pRectFillItem == NULL )
    {
	return;
    }

    byte nReportId=0;
    if ( m_pRectXItem != NULL )
    {
	nReportId = m_pRectXItem->ReportID;
	m_pRectXItem->Value = pCmd->x;
    }
    if ( m_pRectYItem != NULL )
    {
	nReportId = m_pRectYItem->ReportID;
	m_pRectYItem->Value = pCmd->y;
    }
    if ( m_pRectWidthItem != NULL )
    {
	nReportId = m_pRectWidthItem->ReportID;
	m_pRectWidthItem->Value = pCmd->width;
    }
    if ( m_pRectHeightItem != NULL )
    {
	nReportId = m_pRectHeightItem->ReportID;
	m_pRectHeightItem->Value = pCmd->height;
    }
    if ( m_pRectFillItem != NULL )
    {
	nReportId = m_pRectFillItem->ReportID;
	m_pRectFillItem->Value = pCmd->fill;
    }

    SendReport( pDevice, nReportId );
}

void EMCHIDLCD::SendDisplayControlReport(HIDDevice *pDevice)
{
    LOG_DEBUG( m_Logger, "SendDisplayControlReport" );

    if ( m_pClearScreenItem == NULL &&
         m_pBacklightItem == NULL && 
	 m_pBackgroundColourItem == NULL &&
         m_pForegroundColourItem == NULL &&
         m_pFontItem == NULL )
    {
        LOG_DEBUG( m_Logger, "no hid items found in the display control report" );
	return;
    }

    byte nReportId=0;
    if ( m_pClearScreenItem != NULL )
    {
	nReportId = m_pClearScreenItem->ReportID;
	m_pClearScreenItem->Value = m_bClearScreen ? 1 : 0;
    }
    if ( m_pBackgroundColourItem != NULL )
    {
	nReportId = m_pBackgroundColourItem->ReportID;
	m_pBackgroundColourItem->Value = m_nBackgroundColour;
    }
    if ( m_pForegroundColourItem != NULL )
    {
	nReportId = m_pForegroundColourItem->ReportID;
	m_pForegroundColourItem->Value = m_nForegroundColour;
    }
    if ( m_pFontItem != NULL )
    {
	nReportId = m_pFontItem->ReportID;
	m_pFontItem->Value = m_nFont;
    }
    if ( m_pBacklightItem != NULL )
    {
	nReportId = m_pBacklightItem->ReportID;
	m_pBacklightItem->Value = m_nIntensity;
    }

    SendReport( pDevice, nReportId );

    m_bClearScreen = false;
}



int EMCHIDLCD::SendReport( HIDDevice *pDevice, byte nReportId )
{
    LOG_DEBUG( m_Logger, QString("Sending report %1").arg(nReportId) );

    HID_ReportDetails_t pReportDetails = pDevice->ReportInfo().Reports[nReportId];
    int nBufLen = pReportDetails.OutReportLength;
    int nOffset = 0;
    if ( pDevice->ReportInfo().Reports.size() > 1 )
	nOffset=1;

    QVarLengthArray<byte> buf;
    buf.resize(nBufLen+nOffset);
    if ( nOffset )
	buf[0] = nReportId;

    HIDParser parser;
    parser.MakeOutputReport( buf.data() + nOffset, (byte)nBufLen, pDevice->ReportInfo().ReportItems, nReportId );

    // Send the report
    int nRet = pDevice->AsyncInterruptWrite( buf.data(), nBufLen + nOffset );
    LOG_MSG( m_Logger, LogTypes::Debug, QString("interrupt write returned %1\n").arg(nRet).toLatin1().constData() );
    return nRet;
}



