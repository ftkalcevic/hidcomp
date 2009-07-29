#include "emchiditem.h"

EMCHIDItem::EMCHIDItem( const QString &sModule, const QString &sComponent, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: m_Logger(sModule, sComponent),
  m_pCfgItem(pCfgItem), 
  m_pDeviceItem(pDeviceItem)
{
}

EMCHIDItem::~EMCHIDItem(void)
{
}
