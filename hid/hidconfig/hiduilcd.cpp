#include "stdafx.h"
#include "hiduilcd.h"
#include "usages.h"
#include "lcdconfigdlg.h"

HIDUILCD::HIDUILCD(HIDDevice *pDev, HID_CollectionPath_t *col, int nIndex, QGridLayout *layout, QWidget *parent) 
: QWidget( parent ),
  HIDUIBase( HIDItemType::LCD, m_temp, NULL, nIndex ),
  m_pCol( col ),
  m_lcdData(0,0,0,true,"pin"),
  m_pDevice( pDev ), 
  m_Logger( QCoreApplication::applicationName(), "HIDUILCD" )
{
    int nRow = layout->rowCount();
    CreateLCDUI( pDev, layout, nRow );

    // Current value
    QLabel *pLCD = new QLabel( "LCD" );
    layout->addWidget( pLCD, nRow, 0 );

    QPushButton *pButton = new QPushButton( tr("Config") );
    layout->addWidget( pButton, nRow, 5 );
    connect( pButton, SIGNAL(clicked(bool)), this, SLOT(onConfigClicked(bool)));

    m_lcdData.setSamplePeriod( 100 );
}


HIDUILCD::~HIDUILCD()
{
}

void HIDUILCD::CreateLCDUI( HIDDevice *, QGridLayout *layout, int nRow) 
{
    // UsagePage/Usage
    QString sUsagePage, sUsage;
    Usages::GetUsages( m_pCol->UsagePage, m_pCol->Usage, sUsagePage, sUsage );
    m_lblUsage = new QLabel( QString("%1:%2").arg(sUsagePage, sUsage) );
    layout->addWidget( m_lblUsage, nRow, 1 );

    // String
    QString sName;
    m_lblName = new QLabel( sName );
    layout->addWidget( m_lblName, nRow, 2 );

    // PIN Mapping
    m_chkHalPin = new QCheckBox( "HAL pin:" );
    layout->addWidget( m_chkHalPin, nRow, 3 );

    m_txtHalPin = new QLineEdit();
    layout->addWidget( m_txtHalPin, nRow, 4 );

    if ( !m_lblName->text().isEmpty() )
	m_txtHalPin->setText( Utility::MakeHALSafeName(m_lblName->text()) );
    else
	m_txtHalPin->setText( "LCD" );

}


void HIDUILCD::onConfigClicked(bool)
{
    // retreive the display attributes
    int nRows, nCols ;
    QueryDisplayParmeters( nRows, nCols );
    if ( nRows == 0 )
	nRows = 2;
    if ( nCols == 0 )
	nCols = 20;

    LCDConfigDlg dlg( &m_lcdData, m_pDevice, m_pCol, nRows, nCols, this );

    if ( dlg.exec() == QDialog::Accepted )
    {
        dlg.getConfig( &m_lcdData );
    }
}


void HIDUILCD::setConfig( HIDItem *pItem )
{
    HIDUIBase::setConfig( pItem );
    HIDLCD *lcdData = dynamic_cast<HIDLCD *>( pItem );
    m_lcdData = *lcdData;
}


void HIDUILCD::getConfig( HIDItem *pItem )
{
    HIDUIBase::getConfig( pItem );
    HIDLCD *lcdData = dynamic_cast<HIDLCD *>( pItem );
    *lcdData = m_lcdData;
}

void HIDUILCD::QueryDisplayParmeters( int &nRows, int &nCols )
{
    nRows = 0;
    nCols = 0;

    // find the feature report that contains the rows and columns count.  That's all we want.
    HID_ReportItem_t *pRowItem = m_pDevice->FindReportItem( REPORT_ITEM_TYPE_Feature, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_ROWS );
    HID_ReportItem_t *pColItem = m_pDevice->FindReportItem( REPORT_ITEM_TYPE_Feature, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_COLUMNS );

    HID_ReportDetails_t pReportDetails = m_pDevice->ReportInfo().Reports[pRowItem->ReportID];
    int nBufLen = pReportDetails.FeatureReportLength;
    int nOffset = 0;
    if ( m_pDevice->ReportInfo().Reports.size() > 1 )
	nOffset=1;

    byte *buf = new byte[nBufLen+nOffset];
    bool b = m_pDevice->GetReport( pRowItem->ReportID, REPORT_ITEM_TYPE_Feature, buf, nBufLen + nOffset );
    if ( !b ) 
    {
        LOG_MSG(m_Logger, LogTypes::Warning, QString("Failed to retreive feature report") );
    }
    else
    {
	HIDParser parser;
	parser.DecodeReport( buf+nOffset, (byte)nBufLen, m_pDevice->ReportInfo().ReportItems, pRowItem->ReportID, REPORT_ITEM_TYPE_Feature );
	nRows = pRowItem->Value;
	if ( pColItem->ReportID != pRowItem->ReportID )
	{
	    // attributes are in different reports.
	    pReportDetails = m_pDevice->ReportInfo().Reports[pColItem->ReportID];
	    nBufLen = pReportDetails.FeatureReportLength;

	    delete buf;
	    buf = new byte[nBufLen+nOffset];
	    b = m_pDevice->GetReport( pRowItem->ReportID, REPORT_ITEM_TYPE_Feature, buf, nBufLen );
	    if ( !b ) 
	    {
		LOG_MSG(m_Logger, LogTypes::Warning, QString("Failed to retreive feature report") );
		return;
	    }
	    parser.DecodeReport( buf+nOffset, (byte)nBufLen, m_pDevice->ReportInfo().ReportItems, pRowItem->ReportID, REPORT_ITEM_TYPE_Feature );
	}
	nCols = pColItem->Value;
	delete buf;
    }
    return;
}




QList<QString> HIDUILCD::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "page-select" );

    for ( int p = 0; p < m_lcdData.pages().count(); p++ )
	for ( int e = 0; e < m_lcdData.pages()[p]->data().count(); e++ )
	{
	    LCDEntry *pEntry = m_lcdData.pages()[p]->data()[e];
	    
	    QString s;
	    switch ( pEntry->data() )
	    {
		case ELCDDisplayData::UserBit:	    s = QString("in-bit-%1").arg(pEntry->index()); break;
		case ELCDDisplayData::UserFloat:    s = QString("in-float-%1").arg(pEntry->index()); break;
		case ELCDDisplayData::UserS32:	    s = QString("in-s32-%1").arg(pEntry->index()); break;
		case ELCDDisplayData::UserU32:	    s = QString("in-u32-%1").arg(pEntry->index()); break;
		default: break;
	    }
	    if ( !s.isEmpty() )
	    {
		s = sPrefix + "." + name() + "." + s;
		// We can get duplicates at this point (display fields shown in different places)
		if ( !list.contains( s ) )
		    list.push_back( s );
	    }
	}

    return list;
}

