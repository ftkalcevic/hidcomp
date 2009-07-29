#include "emchidbutton.h"

#define IN_PIN		0
#define IN_NOT_PIN	1


EMCHIDButton::EMCHIDButton(const QString  &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDButton", pCfgItem, pDeviceItem)
{
    // Add a bit and an inverted bit.
    QString sPin;
    sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
    sPin += ".";
    sPin += pCfgItem->sPinName;
    sPin += ".";
    QString  s = sPin;
    s += "in";
    Pins.push_back( EMCPin(s, HAL_BIT, HAL_OUT) );
    s = sPin;
    s += "in-not";
    Pins.push_back( EMCPin(s, HAL_BIT, HAL_OUT) );
}

EMCHIDButton::~EMCHIDButton(void)
{
}


void EMCHIDButton::UpdatePin()
{
    hal_bit_t n;
    if ( m_pDeviceItem->Value )
	n = 1;
    else
	n = 0;

    **(hal_bit_t **)(Pins[IN_PIN].pData) = n;
    **(hal_bit_t **)(Pins[IN_NOT_PIN].pData) = !n;
}

