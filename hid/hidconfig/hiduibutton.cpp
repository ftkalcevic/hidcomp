#include "stdafx.h"
#include "hiduibutton.h"
#include "usages.h"

QPixmap *HIDUIButton::m_pOn = NULL;
QPixmap *HIDUIButton::m_pOff = NULL;


HIDUIButton::HIDUIButton(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *) 
: HIDUIBase( HIDItemType::Button, value, item, index )
{
    m_bOn = false;

    if ( m_pOn == NULL )
	m_pOn = new QPixmap( ":hidconfig/GreenLight" );
    if ( m_pOff == NULL )
	m_pOff = new QPixmap( ":hidconfig/RedLight" );

    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // default name will be button.n
    if ( m_lblName->text().isEmpty() )
	m_txtHalPin->setText( QString("Button.%1").arg(m_pItem->Attributes.Usage) );

    // Current value
    m_pLight = new QLabel();
    m_pLight->setPixmap( *m_pOff );
    layout->addWidget( m_pLight, nRow, 0 );
}


HIDUIButton::~HIDUIButton()
{
}


void HIDUIButton::Update()
{
    if ( m_nValue )
    {
	if ( !m_bOn )
	{
	    m_pLight->setPixmap( *m_pOn );
	    m_bOn = true;
	}
    }
    else
    {
	if ( m_bOn )
	{
	    m_pLight->setPixmap( *m_pOff );
	    m_bOn = false;
	}
    }
}


QList<QString> HIDUIButton::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "in" );
    list.push_back( sPrefix + "." + name() + "." + "in-not" );

    return list;
}

