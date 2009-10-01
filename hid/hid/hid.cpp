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

#include "hid.h"
#include "log.h"
#include "xmlutility.h"
#include "common.h"

#include <QFile>
#include <QCoreApplication>

/****************************************************
****************************************************/

HIDDeviceCriteria::HIDDeviceCriteria(void)
{
    bPID = false;
    nPID = 0 ;
    bVID = false;
    nVID = 0;
    bManufacturer = false;
    bProduct = false;
    bSerialNumber = false;
    bSystemId = false;
}

HIDDeviceCriteria::~HIDDeviceCriteria(void)
{
}

void HIDDeviceCriteria::ReadXML( QDomElement pNode )
{
    bPID = false;
    bVID = false;
    bManufacturer = false;
    bProduct = false;
    bSerialNumber = false;
    bSystemId = false;

    if ( XMLUtility::hasAttribute( pNode, "matchPID") )
    {
	bPID = true;
	nPID = (unsigned short)XMLUtility::getAttribute( pNode, "matchPID", 0 );
    }

    if ( XMLUtility::hasAttribute( pNode, "matchVID") )
    {
	bVID = true;
	nVID = (unsigned short)XMLUtility::getAttribute( pNode, "matchVID", 0 );
    }

    if ( XMLUtility::hasAttribute( pNode, "matchManufacturer") )
    {
	bManufacturer = true;
	sManufacturer = XMLUtility::getAttribute( pNode, "matchManufacturer", "" );
    }

    if ( XMLUtility::hasAttribute( pNode, "matchProduct") )
    {
	bProduct = true;
	sProduct = XMLUtility::getAttribute( pNode, "matchProduct", "" );
    }

    if ( XMLUtility::hasAttribute( pNode, "matchSerialNumber") )
    {
	bSerialNumber = true;
	sSerialNumber = XMLUtility::getAttribute( pNode, "matchSerialNumber", "" );
    }

    if ( XMLUtility::hasAttribute( pNode, "matchSystemId") )
    {
	bSystemId = true;
	sSystemId = XMLUtility::getAttribute( pNode, "matchSystemId", "" );
    }
}

QDomElement HIDDeviceCriteria::WriteXML( QDomElement pNode )
{
    QDomElement pDeviceElem = pNode.ownerDocument().createElement("Device");
    pNode.appendChild(pDeviceElem);

    if ( bPID )
	XMLUtility::setAttribute( pDeviceElem, "matchPID", nPID );
    if ( bVID )
	XMLUtility::setAttribute( pDeviceElem, "matchVID", nVID );
    if ( bManufacturer )
	XMLUtility::setAttribute( pDeviceElem, "matchManufacturer", sManufacturer );
    if ( bProduct )
	XMLUtility::setAttribute( pDeviceElem, "matchProduct", sProduct );
    if ( bSerialNumber )
	XMLUtility::setAttribute( pDeviceElem, "matchSerialNumber", sSerialNumber );
    if ( bSystemId )
	XMLUtility::setAttribute( pDeviceElem, "matchSystemId", sSystemId );
    return pDeviceElem;
}

/****************************************************
****************************************************/

HIDItem::HIDItem( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName, HIDItemType::HIDItemType type )
: nIndex(nIndex), 
nUsagePage(nUsagePage), 
nUsage(nUsage), 
bEnabled(bEnabled), 
sPinName(sPinName), 
type(type)
{
}

HIDItem::HIDItem( const HIDItem &other )
{
    nIndex = other.nIndex;
    nUsagePage = other.nUsagePage;
    nUsage = other.nUsage;
    bEnabled = other.bEnabled;
    sPinName = other.sPinName;
    type = other.type;
}

HIDItem::~HIDItem() 
{
}

HIDItem *HIDItem::CreateFromXML( QDomElement pNode )
{
    int nIndex = XMLUtility::getAttribute( pNode, "index", -1 );
    unsigned short nUsagePage = (unsigned short)XMLUtility::getAttribute( pNode, "usagePage", 0 );
    unsigned short nUsage = (unsigned short)XMLUtility::getAttribute( pNode, "usage", 0 );
    bool bEnabled = XMLUtility::getAttribute( pNode, "enabled", false );
    QString sPinName = XMLUtility::getAttribute( pNode, "pin", "" );
    HIDItemType::HIDItemType type = (HIDItemType::HIDItemType)XMLUtility::getAttribute( pNode, "type", 0 );

    HIDItem *pItem = CreateItem( type, nIndex, nUsagePage, nUsage, bEnabled, sPinName );
    pItem->ReadXML( pNode );

    return pItem;
}


HIDItem *HIDItem::CreateItem( HIDItemType::HIDItemType type, int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName )
{
    HIDItem *pItem = NULL;
    switch ( type )
    {
    case HIDItemType::Button:
	pItem = new HIDButton(nIndex, nUsagePage, nUsage, bEnabled, sPinName );
	break;
    case HIDItemType::Value:
	pItem = new HIDInputValue(nIndex, nUsagePage, nUsage, bEnabled, sPinName );
	break;
    case HIDItemType::Hatswitch:
	pItem = new HIDHatswitch(nIndex, nUsagePage, nUsage, bEnabled, sPinName );
	break;
    case HIDItemType::LED:
	pItem = new HIDLED(nIndex, nUsagePage, nUsage, bEnabled, sPinName );
	break;
    case HIDItemType::LCD:
	pItem = new HIDLCD(nIndex, nUsagePage, nUsage, bEnabled, sPinName );
	break;
    case HIDItemType::OutputValue:
	pItem = new HIDOutputValue(nIndex, nUsagePage, nUsage, bEnabled, sPinName );
	break;
    }

    return pItem;
}

QDomElement HIDItem::WriteXML( QDomElement pNode )
{
    QDomElement pElem = pNode.ownerDocument().createElement("Item");
    pNode.appendChild(pElem);

    XMLUtility::setAttribute( pElem, "index", nIndex );
    XMLUtility::setAttribute( pElem, "usagePage", nUsagePage );
    XMLUtility::setAttribute( pElem, "usage", nUsage );
    XMLUtility::setAttribute( pElem, "enabled", bEnabled );
    XMLUtility::setAttribute( pElem, "pin", sPinName );
    XMLUtility::setAttribute( pElem, "type", type );

    return pElem;
}

/****************************************************
****************************************************/
HIDInputItem::HIDInputItem( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName, HIDItemType::HIDItemType type )
: HIDItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, type )
{
}

HIDInputItem::~HIDInputItem()
{
}

/****************************************************
****************************************************/
HIDOutputItem::HIDOutputItem( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName, HIDItemType::HIDItemType type )
: HIDItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, type )
{
}

HIDOutputItem::HIDOutputItem( const HIDOutputItem &other )
: HIDItem( other )
{
}

HIDOutputItem::~HIDOutputItem()
{
}

/****************************************************
****************************************************/
HIDButton::HIDButton( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName )
: HIDInputItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, HIDItemType::Button )
{
}

HIDButton::~HIDButton()
{
}

/****************************************************
****************************************************/
HIDInputValue::HIDInputValue( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName )
: HIDInputItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, HIDItemType::Value )
{
    valueType = ValueType::Default;
    outputType = ValueOutputType::FloatingPoint;
    nLogicalMinOverride = 0;
    nLogicalMaxOverride = 1023;
    dOutputMin = -1.0;
    dOutputMax = 1.0;
    bUseResponseCurve = false;
    bReverse = false;
}

HIDInputValue::~HIDInputValue()
{
}


void HIDInputValue::ReadXML( QDomElement pNode )
{
    valueType =  (ValueType::ValueType)XMLUtility::getAttribute( pNode, "valueType", ValueType::Default );
    outputType =  (ValueOutputType::ValueOutputType)XMLUtility::getAttribute( pNode, "outputType", ValueOutputType::FloatingPoint );
    nLogicalMinOverride = XMLUtility::getAttribute( pNode, "logicalMinOverride", 0 );
    nLogicalMaxOverride = XMLUtility::getAttribute( pNode, "logicalMaxOverride", 0 );
    dOutputMin = XMLUtility::getAttribute( pNode, "outputMin", 0.0 );
    dOutputMax = XMLUtility::getAttribute( pNode, "outputMax", 0.0 );
    bUseResponseCurve = XMLUtility::getAttribute( pNode, "useResponseCurve", false );
    bReverse = XMLUtility::getAttribute( pNode, "reverse", false );

    responseCurve.clear();
    QDomElement cpoints = XMLUtility::firstChildElement(pNode, "controlPoints");
    QDomNodeList points = XMLUtility::elementsByTagName( cpoints, "point" );
    for ( uint p = 0; p < points.length(); p++ )
    {
	QDomElement point = points.item(p).toElement();

	int dwLog = XMLUtility::getAttribute( point, "dwLog", 0 );
	int lP = XMLUtility::getAttribute( point, "lP", 0 );

	responseCurve.push_back( CPoint(lP, dwLog ) );
    }

    // make sure we have a valid curve
    if ( responseCurve.size() < 2 )
    {
	// Corrupt curve.  Put in a straight line.
	responseCurve.clear();
	responseCurve.push_back( CPoint(0,0) );
	responseCurve.push_back( CPoint(POINTS_MAX,POINTS_MAX) );
    }
}
QDomElement HIDInputValue::WriteXML( QDomElement pNode )
{
    QDomElement pElem = HIDInputItem::WriteXML( pNode );

    XMLUtility::setAttribute( pElem, "valueType", valueType );
    XMLUtility::setAttribute( pElem, "outputType", outputType );
    XMLUtility::setAttribute( pElem, "logicalMinOverride", nLogicalMinOverride );
    XMLUtility::setAttribute( pElem, "logicalMaxOverride", nLogicalMaxOverride );
    XMLUtility::setAttribute( pElem, "outputMin", dOutputMin );
    XMLUtility::setAttribute( pElem, "outputMax", dOutputMax );
    XMLUtility::setAttribute( pElem, "reverse", bReverse );
    XMLUtility::setAttribute( pElem, "useResponseCurve", bUseResponseCurve );

    if ( responseCurve.size() > 0 )
    {
	QDomElement pPoints = pNode.ownerDocument().createElement("controlPoints");
	pElem.appendChild(pPoints);

	for ( uint p = 0; p < responseCurve.size(); p++ )
	{
	    QDomElement pPoint = pNode.ownerDocument().createElement("point");
	    pPoints.appendChild(pPoint);

	    XMLUtility::setAttribute( pPoint, "dwLog", responseCurve[p].dwLog );
	    XMLUtility::setAttribute( pPoint, "lP", responseCurve[p].lP );
	}
    }

    return pElem;
}

// interpolate an input value according to the control points on the response curve.
int HIDInputValue::Interpolate( int lP )
{
    // interpolate the logical value from the raw value in screen coordinates.
    int x;
    for ( int i = 0; i < (int)responseCurve.size(); i++ )
    {
	x = responseCurve[i].lP;

	if ( lP < x )
	{
	    if ( i == 0 )
		return responseCurve[0].dwLog;
	    else
	    {
		int x0 = responseCurve[i-1].lP;
		int y0 = responseCurve[i-1].dwLog;
		int y = responseCurve[i].dwLog;

		if ( x == x0 )
		    return y;

		int ret = (lP - x0) * ( y - y0 ) / ( x - x0) + y0;

		return ret;
	    }
	}
    }

    return responseCurve.rbegin()->dwLog;
}

// Linear scale the value to 0 to POINTS_MAX_VAL
int HIDInputValue::Scale( int lP )
{
    int x0 = nLogicalMinOverride;
    int x = nLogicalMaxOverride;
    int y0 = 0;
    int y = POINTS_MAX_VAL;

    int ret = (lP - x0) * ( y - y0 ) / ( x - x0) + y0;

    if ( ret < 0 )
	ret = 0;
    else if ( ret > POINTS_MAX_VAL )
	ret = POINTS_MAX_VAL;
    return ret;
}

// Linear scale of the % value to double
double HIDInputValue::DScale( int lP )
{
    int x0 = 0;
    int x = POINTS_MAX_VAL;
    double y0 = dOutputMin;
    double y = dOutputMax;

    double ret = ((double)(lP - x0)) * ( y - y0 ) / ((double)( x - x0 )) + y0;

    if ( ret < dOutputMin )
	ret = dOutputMin;
    else if ( ret > dOutputMax )
	ret = dOutputMax;
    return ret;
}

/****************************************************
****************************************************/
HIDHatswitch::HIDHatswitch( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName )
: HIDInputItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, HIDItemType::Hatswitch )
{
}

HIDHatswitch::~HIDHatswitch()
{
}


/****************************************************
****************************************************/
HIDLED::HIDLED( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName )
: HIDOutputItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, HIDItemType::LED )
{
}

HIDLED::~HIDLED()
{
}

/****************************************************
****************************************************/
HIDOutputValue::HIDOutputValue( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName )
: HIDOutputItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, HIDItemType::OutputValue )
{
}

HIDOutputValue::~HIDOutputValue()
{
}

/****************************************************
****************************************************/
LCDEntry::LCDEntry( ELCDDisplayData::ELCDDisplayData eData, int nIndex, int nRow, int nCol, const QString &sFormat, double dScale, const QString &sTestData )
: m_eData( eData )
, m_nIndex( nIndex )
, m_nRow( nRow )
, m_nCol( nCol )
, m_sFormat( sFormat )
, m_dScale( dScale )
, m_sTestData( sTestData )
{
}

LCDEntry::LCDEntry( const LCDEntry &other )
: m_eData( other.m_eData )
, m_nIndex( other.m_nIndex )
, m_nRow( other.m_nRow )
, m_nCol( other.m_nCol )
, m_sFormat( other.m_sFormat )
, m_dScale( other.m_dScale )
, m_sTestData( other.m_sTestData )
{
}

LCDEntry::~LCDEntry()
{
}

void LCDEntry::ReadXML( QDomElement pNode )
{
    m_eData = (ELCDDisplayData::ELCDDisplayData)XMLUtility::getAttribute( pNode, "DisplayData", ELCDDisplayData::None );
    m_nIndex = XMLUtility::getAttribute( pNode, "Index", 0 );
    m_nRow = XMLUtility::getAttribute( pNode, "Row", 0 );
    m_nCol = XMLUtility::getAttribute( pNode, "Col", 0 );
    m_sFormat = XMLUtility::getAttribute( pNode, "Format", "" );
    m_dScale = XMLUtility::getAttribute( pNode, "Scale", (double)(1.0) );
    m_sTestData = XMLUtility::getAttribute( pNode, "TestData", "" );
}

QDomElement LCDEntry::WriteXML( QDomElement pNode )
{
    XMLUtility::setAttribute( pNode, "DisplayData", m_eData );
    XMLUtility::setAttribute( pNode, "Index", m_nIndex );
    XMLUtility::setAttribute( pNode, "Row", m_nRow );
    XMLUtility::setAttribute( pNode, "Col", m_nCol );
    XMLUtility::setAttribute( pNode, "Format", m_sFormat );
    XMLUtility::setAttribute( pNode, "Scale", m_dScale );
    XMLUtility::setAttribute( pNode, "TestData", m_sTestData );
    return pNode;
}

/****************************************************
****************************************************/
LCDPage::LCDPage( int nNumber, const QString &sName )
: m_nNumber( nNumber )
, m_sName( sName )
{
}

LCDPage::LCDPage( const LCDPage &other )
{
    m_nNumber = other.m_nNumber;
    m_sName = other.m_sName;
    for ( int i = 0; i < other.m_data.count(); i++ )
        m_data.append( new LCDEntry( *other.m_data[i]) );
}

LCDPage::~LCDPage()
{
    for ( int i = 0; i < m_data.count(); i++ )
        delete m_data[i];
    m_data.clear();
}

void LCDPage::ReadXML( QDomElement pNode )
{
    m_nNumber =  XMLUtility::getAttribute( pNode, "Number", 0 );
    m_sName =  XMLUtility::getAttribute( pNode, "Name", "0" );

    m_data.clear();
    QDomNodeList entries = XMLUtility::elementsByTagName( pNode, "Data" );
    for ( uint p = 0; p < entries.length(); p++ )
    {
	QDomElement entryElement = entries.item(p).toElement();
        LCDEntry *entry = new LCDEntry(ELCDDisplayData::None,0,0,0,QString(),1.0,QString());
        entry->ReadXML( entryElement );
        m_data.append( entry );
    }
}


QDomElement LCDPage::WriteXML( QDomElement pNode )
{
    XMLUtility::setAttribute( pNode, "Number", m_nNumber );
    XMLUtility::setAttribute( pNode, "Name", m_sName );

    if ( m_data.count() > 0 )
    {
        for ( int i = 0; i < m_data.count(); i++ )
        {
	    QDomElement pDataNode = pNode.ownerDocument().createElement("Data");
	    pNode.appendChild(pDataNode);

            m_data[i]->WriteXML( pDataNode );
        }
    }

    return pNode;
}


/****************************************************
****************************************************/
LCDFont::LCDFont( byte nIndex, const QVector<byte> &data )
: m_nIndex( nIndex )
{
    setData( data );
}

LCDFont::LCDFont( const LCDFont &other )
{
    m_nIndex = other.m_nIndex;
    setData( other.m_data );
}

LCDFont::~LCDFont()
{
    m_data.clear();
}

void LCDFont::setData( const QVector<byte> &data )
{ 
    m_data.resize( data.size() );
    for ( int i = 0; i < data.size(); i++ )
	m_data[i] = data[i];
}

bool LCDFont::GetFontBit( int cols, int r, int c, const byte *data )
{
    int nBit = r * cols + (cols-c-1);
    int nByte = nBit / 8;
    nBit &= 0x07;
    return (data[nByte] & (1 << nBit)) != 0;
}

void LCDFont::SetFontBit( int cols, int r, int c, byte *data, bool bSet )
{
    int nBit = r * cols + (cols-c-1);
    int nByte = nBit / 8;
    nBit &= 0x07;
    if ( bSet )
	data[nByte] |= 1 << nBit;
    else
	data[nByte] &= ~(1 << nBit);
}

void LCDFont::ReadXML( QDomElement pNode )
{
    m_nIndex =  (byte)XMLUtility::getAttribute( pNode, "Index", 0 );

    m_data.clear();
    QDomNodeList entries = XMLUtility::elementsByTagName( pNode, "Data" );
    for ( uint p = 0; p < entries.length(); p++ )
    {
	QDomElement entryElement = entries.item(p).toElement();
	int n = XMLUtility::getAttribute( entryElement, "bits", 0 );
        m_data.append( (byte)n );
    }
}


QDomElement LCDFont::WriteXML( QDomElement pNode )
{
    XMLUtility::setAttribute( pNode, "Index", m_nIndex );

    if ( m_data.count() > 0 )
    {
        for ( int i = 0; i < m_data.count(); i++ )
        {
	    QDomElement pDataElem = pNode.ownerDocument().createElement("Data");
	    pNode.appendChild(pDataElem);
	    XMLUtility::setAttribute( pDataElem, "bits", m_data[i] );
        }
    }

    return pNode;
}



/****************************************************
****************************************************/
HIDLCD::HIDLCD( int nIndex, unsigned short nUsagePage, unsigned short nUsage, bool bEnabled, const QString &sPinName )
: HIDOutputItem( nIndex, nUsagePage, nUsage, bEnabled, sPinName, HIDItemType::LCD )
, m_nSamplePeriod( 100 )
, m_nLCDProcPort( 0 )
{
}

HIDLCD::HIDLCD( const HIDLCD &other )
: HIDOutputItem( other )
{
    m_nSamplePeriod = other.m_nSamplePeriod;
    m_nLCDProcPort = other.m_nLCDProcPort;
    for ( int i = 0; i < other.m_pages.count(); i++ )
        m_pages.append( new LCDPage(*other.m_pages[i]) );
    for ( int i = 0; i < other.m_fonts.count(); i++ )
        m_fonts.append( new LCDFont(*other.m_fonts[i]) );
}

HIDLCD & HIDLCD::operator= ( const HIDLCD & other )
{
    m_nSamplePeriod = other.m_nSamplePeriod;
    m_nLCDProcPort = other.m_nLCDProcPort;
    clear();
    for ( int i = 0; i < other.m_pages.count(); i++ )
        m_pages.append( new LCDPage(*other.m_pages[i]) );
    for ( int i = 0; i < other.m_fonts.count(); i++ )
        m_fonts.append( new LCDFont(*other.m_fonts[i]) );
    return *this;
}

HIDLCD::~HIDLCD()
{
    clear();
}

void HIDLCD::clear()
{
    for ( int i = 0; i < m_pages.count(); i++ )
	delete m_pages[i];
    m_pages.clear();
    for ( int i = 0; i < m_fonts.count(); i++ )
	delete m_fonts[i];
    m_fonts.clear();
}

void HIDLCD::ReadXML( QDomElement pNode )
{
    m_nSamplePeriod =  XMLUtility::getAttribute( pNode, "refreshPeriod", 100 );
    m_nLCDProcPort =  XMLUtility::getAttribute( pNode, "LCDProcPort", 0 );

    m_pages.clear();
    QDomNodeList pages = XMLUtility::elementsByTagName( pNode, "Page" );
    for ( uint p = 0; p < pages.length(); p++ )
    {
	QDomElement pageElement = pages.item(p).toElement();
        LCDPage *page = new LCDPage(0,QString());
        page->ReadXML( pageElement );
        m_pages.append( page );
    }
    m_fonts.clear();
    QDomNodeList fonts = XMLUtility::elementsByTagName( pNode, "Font" );
    for ( uint f = 0; f < fonts.length(); f++ )
    {
	QDomElement fontElement = fonts.item(f).toElement();
        LCDFont *font = new LCDFont();
        font->ReadXML( fontElement );
        m_fonts.append( font );
    }
}

QDomElement HIDLCD::WriteXML( QDomElement pNode )
{
    QDomElement pElem = HIDOutputItem::WriteXML( pNode );

    XMLUtility::setAttribute( pElem, "refreshPeriod", m_nSamplePeriod );
    XMLUtility::setAttribute( pElem, "LCDProcPort", m_nLCDProcPort );

    if ( m_pages.count() > 0 )
    {
        for ( int p = 0; p < m_pages.count(); p++ )
        {
	    QDomElement pPageNode = pNode.ownerDocument().createElement("Page");
	    pElem.appendChild(pPageNode);

            m_pages[p]->WriteXML( pPageNode );
        }
    }

    if ( m_fonts.count() > 0 )
    {
        for ( int f = 0; f < m_fonts.count(); f++ )
        {
	    QDomElement pFontNode = pNode.ownerDocument().createElement("Font");
	    pElem.appendChild(pFontNode);

            m_fonts[f]->WriteXML( pFontNode );
        }
    }

    return pElem;
}


/****************************************************
****************************************************/

HID::HID()
: criteria( NULL )
{
}

HID::~HID()
{
    if ( criteria != NULL )
	delete criteria;

    for ( unsigned int i = 0; i < items.size(); i++ )
        delete items[i];
    items.clear();
}


HID *HID::CreateFromXML( const QString &fileName )
{
    Logger logger( QCoreApplication::applicationName(), "HID" );

    HID *hid = NULL;

    QFile file( fileName );
    if ( !file.open(QIODevice::ReadOnly) )
	return NULL;
    QDomDocument doc("HID");
    doc.setContent( &file );
    file.close();

    QDomElement pRootElement = doc.firstChildElement( "HID" );
    if ( pRootElement.isNull() )
    {
	LOG_MSG( logger, LogTypes::Error, "Root node is not 'HID'" );
	return NULL;
    }

    QDomElement pDevice = XMLUtility::firstChildElement( pRootElement, "Device" );
    if ( pDevice.isNull() )
    {
	LOG_MSG( logger, LogTypes::Error, "Can't find 'Device' node" );
	return NULL;
    }

    hid = new HID();
    hid->configFile = fileName;
    hid->criteria = new HIDDeviceCriteria();
    hid->criteria->ReadXML( pDevice );

    QDomElement pItemsNode = XMLUtility::firstChildElement( pRootElement, "Items" );
    if ( pItemsNode.isNull() )
    {
	LOG_MSG( logger, LogTypes::Warning, "Can't find 'Items' node." );
	return NULL;
    }

    QDomNodeList pItems = XMLUtility::elementsByTagName( pItemsNode, "Item" );
    for ( uint i = 0; i < pItems.length(); i++ )
    {
	QDomElement item = pItems.item(i).toElement();
	HIDItem *pHIDItem = HIDItem::CreateFromXML( item );
	hid->items.push_back( pHIDItem );
    }

    return hid;
}


QDomDocument HID::MakeXML()
{
    QDomDocument doc("HID");

    QDomElement rootElem = doc.createElement("HID");
    doc.appendChild( rootElem );

    criteria->WriteXML( rootElem );

    QDomElement  itemsElem = doc.createElement("Items");
    rootElem.appendChild(itemsElem);

    for ( unsigned int i = 0; i < items.size(); i++ )
	items[i]->WriteXML( itemsElem );

    return doc;
}

