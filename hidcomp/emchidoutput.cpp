#include "emchidoutput.h"

#define OUT_VALUE     0

EMCHIDOutput::EMCHIDOutput(const QString  &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDItem", pCfgItem, pDeviceItem)
{
    m_nState = 0xFFFFFF;

    // Add an input bit
    QString sPin;
    sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
    sPin += ".";
    sPin += pCfgItem->sPinName;
    sPin += ".";
    QString  s = sPin;
    s += "ivalue";
    Pins.push_back( EMCPin(s, HAL_S32, HAL_IN) );
}

EMCHIDOutput::~EMCHIDOutput(void)
{
}

bool EMCHIDOutput::CheckOutputs()
{
    int nPinValue = **(hal_bit_t **)(Pins[OUT_VALUE].pData);

    if ( nPinValue != m_nState)
    {
        m_nState = nPinValue;
        m_pDeviceItem->Value = m_nState;
        return true;            // A change, we need to send an update
    }
    return false;
}
