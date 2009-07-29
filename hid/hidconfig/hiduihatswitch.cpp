#include "stdafx.h"
#include "hiduihatswitch.h"
#include "usages.h"
#include "hatswitchwidget.h"


HIDUIHatswitch::HIDUIHatswitch(HIDDevice *pDev, HID_ReportItem_t *item, int index, QGridLayout *layout, int &value, QWidget *) 
: HIDUIBase( HIDItemType::Hatswitch, value, item, index )
{
    int nRow = layout->rowCount();
    CreateCommonUI( pDev, layout, nRow );

    // Current value
    m_pHatSwitch = new HatSwitchWidget();
    m_pHatSwitch->setValue( 0 );
    layout->addWidget( m_pHatSwitch, nRow, 0 );
}


HIDUIHatswitch::~HIDUIHatswitch(void)
{
}

void HIDUIHatswitch::Update()
{
    // adjust the value to ...
    // -1, no selection
    // 0 - 7, 0 degrees to 315 degrees
    if ( m_nValue < m_pItem->Attributes.LogicalMinimum )
        m_nValue = -1;
    else if ( m_nValue > m_pItem->Attributes.LogicalMaximum )
        m_nValue = -1;
    else 
        m_nValue -= m_pItem->Attributes.LogicalMinimum;
    m_pHatSwitch->setValue( m_nValue );
}


QList<QString> HIDUIHatswitch::PinNames( const QString &sPrefix )
{
    QList<QString> list;
    list.push_back( sPrefix + "." + name() + "." + "raw" );
    list.push_back( sPrefix + "." + name() + "." + "direction" );

    return list;
}

