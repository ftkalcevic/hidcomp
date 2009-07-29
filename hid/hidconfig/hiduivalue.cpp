#include "stdafx.h"
#include "hiduivalue.h"
#include "usages.h"

HIDUIValue::HIDUIValue(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent)
: QWidget(parent),
HIDUIBase( HIDItemType::Value, value, item, index ),
m_pValueConfigDlg(NULL)
{
    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );
    CreateUI( pDev, layout, nRow );

    // Current value
    m_pValue->setRange( m_pItem->Attributes.LogicalMinimum, m_pItem->Attributes.LogicalMaximum );

    m_responseCurve.clear();
    m_responseCurve.push_back( CPoint(0,0) );
    m_responseCurve.push_back( CPoint(POINTS_MAX,POINTS_MAX) );
    m_bUseResponseCurve = false;
    m_bReverseAxis = false;
    m_nLogicalMinOverride = m_pItem->Attributes.LogicalMinimum;
    m_nLogicalMaxOverride = m_pItem->Attributes.LogicalMaximum;
    m_ValueType = ValueType::Default;
    m_ValueOutputType = ValueOutputType::FloatingPoint;
    m_dOutputMin = -1.0;
    m_dOutputMax = 1.0;

    setFixedSize(0,0);
}


// create empty common ui bits.
void HIDUIValue::CreateUI( HIDDevice *, QGridLayout *layout, int nRow )
{
    // Current value
    m_pValue = new QProgressBar();
    m_pValue->setFormat( "%v" );
    layout->addWidget( m_pValue, nRow, 0 );

    QPushButton *pButton = new QPushButton( tr("Config") );
    layout->addWidget( pButton, nRow, 5 );
    connect( pButton, SIGNAL(clicked(bool)), this, SLOT(onConfigClicked(bool)));
}

HIDUIValue::~HIDUIValue(void)
{
}

// The HID Device has changed.  Repaint the display.
void HIDUIValue::Update()
{
    m_pValue->setValue( m_nValue );

    if ( m_pValueConfigDlg != NULL )
	m_pValueConfigDlg->UpdateValue();
}


void HIDUIValue::onConfigClicked(bool)
{
    m_pValueConfigDlg = new ValueConfigDlg( m_pItem, this, m_bUseResponseCurve, m_responseCurve, m_nLogicalMinOverride, m_nLogicalMaxOverride,
	m_ValueType, m_ValueOutputType, m_dOutputMin, m_dOutputMax, m_bReverseAxis );

    if ( m_pValueConfigDlg->exec() == QDialog::Accepted )
    {
	m_bUseResponseCurve = m_pValueConfigDlg->useResponseCurve();
	m_responseCurve = m_pValueConfigDlg->responseCurve();
	m_nLogicalMinOverride = m_pValueConfigDlg->logicalMinOverride();
	m_nLogicalMaxOverride = m_pValueConfigDlg->logicalMaxOverride();
	m_ValueType = m_pValueConfigDlg->type();
	m_ValueOutputType = m_pValueConfigDlg->outputType();
	m_dOutputMin = m_pValueConfigDlg->outputMin();
	m_dOutputMax = m_pValueConfigDlg->outputMax();
	m_bReverseAxis = m_pValueConfigDlg->reverse();

        m_pValue->setRange( m_nLogicalMinOverride, m_nLogicalMaxOverride );
    }

    delete m_pValueConfigDlg;
    m_pValueConfigDlg = NULL;
}



void HIDUIValue::setConfig( HIDItem *pItem )
{
    HIDUIBase::setConfig( pItem );

    HIDInputValue *item = (HIDInputValue *)pItem;

    m_bUseResponseCurve = item->bUseResponseCurve;
    m_responseCurve = item->responseCurve;
    m_nLogicalMinOverride = item->nLogicalMinOverride;
    m_nLogicalMaxOverride = item->nLogicalMaxOverride;
    m_ValueType = item->valueType;
    m_ValueOutputType = item->outputType;
    m_dOutputMin = item->dOutputMin;
    m_dOutputMax = item->dOutputMax;
    m_bReverseAxis = item->bReverse;

    m_pValue->setRange( m_nLogicalMinOverride, m_nLogicalMaxOverride );
}

void HIDUIValue::getConfig( HIDItem *pItem )
{
    HIDUIBase::getConfig( pItem );

    HIDInputValue *item = (HIDInputValue *)pItem;

    item->bUseResponseCurve = m_bUseResponseCurve;
    item->responseCurve = m_responseCurve;
    item->nLogicalMinOverride = m_nLogicalMinOverride;
    item->nLogicalMaxOverride = m_nLogicalMaxOverride;
    item->valueType = m_ValueType;
    item->outputType = m_ValueOutputType;
    item->dOutputMin = m_dOutputMin;
    item->dOutputMax = m_dOutputMax;
    item->bReverse = m_bReverseAxis;
}


QList<QString> HIDUIValue::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "raw" );

    // Check the attributes of report item.  If it wraps, we assume an encoder.
    if ( m_pItem->ItemFlags & IOF_WRAP )
    {
	list.push_back( sPrefix + "." + name() + "." + "count" );
	list.push_back( sPrefix + "." + name() + "." + "position" );
	list.push_back( sPrefix + "." + name() + "." + "reset" );
    }
    else
    {
	list.push_back( sPrefix + "." + name() + "." + "ivalue" );
	list.push_back( sPrefix + "." + name() + "." + "fvalue" );
    }

    return list;
}

