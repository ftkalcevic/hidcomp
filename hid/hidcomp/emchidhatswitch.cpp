#include "emchidhatswitch.h"
#include "log.h"


#define IN_RAW		    0
#define IN_DIRECTION	    1

EMCHIDHatSwitch::EMCHIDHatSwitch(const QString  &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDHatSwitch", pCfgItem, pDeviceItem)
{
    // Add a bit and an inverted bit.
    QString  sPin;
    sPin = sPinPrefix;		// comp.idx.[pCfgItem->sPinName]...
    sPin += ".";
    sPin += pCfgItem->sPinName;
    sPin += ".";

    QString  s = sPin;
    s += "raw";
    Pins.push_back( EMCPin(s, HAL_S32, HAL_OUT) );

    s = sPin;
    s += "direction";
    Pins.push_back( EMCPin(s, HAL_FLOAT, HAL_OUT) );

    double dPhysMax;
    if ( m_pDeviceItem->Attributes.PhysicalMinimum != m_pDeviceItem->Attributes.PhysicalMaximum )
    {
	m_dPhysMin = m_pDeviceItem->Attributes.PhysicalMinimum;
	dPhysMax = m_pDeviceItem->Attributes.PhysicalMaximum;
    }
    else
    {
	m_dPhysMin = m_pDeviceItem->Attributes.LogicalMinimum;
	dPhysMax = m_pDeviceItem->Attributes.LogicalMaximum;
    }
    m_dScale = (dPhysMax - m_dPhysMin) / ((double)(pDeviceItem->Attributes.LogicalMaximum - pDeviceItem->Attributes.LogicalMinimum));
}

EMCHIDHatSwitch::~EMCHIDHatSwitch(void)
{
}


void EMCHIDHatSwitch::UpdatePin()
{
    int nValue = m_pDeviceItem->Value;
    **(hal_s32_t **)(Pins[IN_RAW].pData) = nValue;

    // adjust the value to ...
    // -1, no selection
    // 0 - 7, 0 degrees to 315 degrees
    if ( nValue < m_pDeviceItem->Attributes.LogicalMinimum )
        nValue = -1;
    else if ( nValue > m_pDeviceItem->Attributes.LogicalMaximum )
        nValue = -1;

    if ( nValue == -1 )
	**(hal_float_t **)(Pins[IN_DIRECTION].pData) = -1.0;
    else
	**(hal_float_t **)(Pins[IN_DIRECTION].pData) = (double)(nValue - m_pDeviceItem->Attributes.LogicalMinimum) * m_dScale + m_dPhysMin;
}

