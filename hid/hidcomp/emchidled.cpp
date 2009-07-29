#include "emchidled.h"

#define OUT_BIT     0

EMCHIDLED::EMCHIDLED(const QString  &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDItem", pCfgItem, pDeviceItem)
{
    m_bState = false;

    // Add an input bit
    QString sPin;
    sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
    sPin += ".";
    sPin += pCfgItem->sPinName;
    sPin += ".";
    QString  s = sPin;
    s += "out";
    Pins.push_back( EMCPin(s, HAL_BIT, HAL_IN) );
}

EMCHIDLED::~EMCHIDLED(void)
{
}

bool EMCHIDLED::CheckOutputs()
{
    bool bPinValue = **(hal_bit_t **)(Pins[OUT_BIT].pData) != 0;

    if ( (bPinValue && !m_bState) || (!bPinValue && m_bState) )
    {
        m_bState = bPinValue;
        m_pDeviceItem->Value = m_bState ? 1 : 0;
        return true;            // A change, we need to send an update
    }
    return false;
}
