#include "stdafx.h"
#include "hiduioutputvalue.h"
#include "usages.h"

HIDUIOutputValue::HIDUIOutputValue(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *parent) 
: QObject(parent), 
  HIDUIBase( HIDItemType::OutputValue, value, item, index )
{
    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // Current value
    m_pValue = new QSlider(Qt::Horizontal);
    m_pValue->setRange( item->Attributes.LogicalMinimum, item->Attributes.LogicalMaximum );
    layout->addWidget( m_pValue, nRow, 0 );

    connect(m_pValue, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}


HIDUIOutputValue::~HIDUIOutputValue(void)
{
}



void HIDUIOutputValue::onValueChanged( int value )
{
    m_pItem->Value = value;
    emit change( m_pItem );
}


QList<QString> HIDUIOutputValue::PinNames( const QString & )
{
    QList<QString> list;
    return list;
}

