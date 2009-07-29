#include "stdafx.h"
#include "hiduiled.h"
#include "usages.h"

HIDUILED::HIDUILED(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent) 
: QObject( parent )
, HIDUIBase( HIDItemType::LED, value, item, index )
{
    m_bOn = false;

    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // Current value
    m_pLight = new QCheckBox();
    layout->addWidget( m_pLight, nRow, 0 );
    connect(m_pLight, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
}


HIDUILED::~HIDUILED()
{
}


void HIDUILED::onStateChanged( int state )
{
    m_pItem->Value = (state == Qt::Checked);
    emit change( m_pItem );
}



QList<QString> HIDUILED::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "out" );

    return list;
}

