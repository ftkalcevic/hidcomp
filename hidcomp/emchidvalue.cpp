// hidcomp/hidconfig, HID device interface for emc
// Copyright (C) 2008, Frank Tkalcevic, www.franksworkshop.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "emchidvalue.h"
#include "log.h"

#define IN_RAW		    0

#define IN_IVALUE	    1
#define IN_FVALUE	    2

#define IN_ENCODER_COUNT    1
#define IN_ENCODER_POSITION 2
#define IN_ENCODER_RESET    3

EMCHIDValue::EMCHIDValue(const QString  &sPinPrefix, HIDItem *pCfgItem, HID_ReportItem_t *pDeviceItem )
: EMCHIDItem(QCoreApplication::applicationName(), "EMCHIDValue", pCfgItem, pDeviceItem)
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

    // Check the attributes of report item.  If it wraps, we assume an encoder.
    if ( pDeviceItem->ItemFlags & IOF_WRAP )
	m_bIsEncoder = true;
    else
	m_bIsEncoder = false;

    if ( m_bIsEncoder )
    {
	m_nEncoderCount = 0;

	s = sPin;
	s += "count";
	Pins.push_back( EMCPin(s, HAL_S32, HAL_OUT) );

	s = sPin;
	s += "position";
	Pins.push_back( EMCPin(s, HAL_FLOAT, HAL_OUT) );

	s = sPin;
	s += "reset";
	Pins.push_back( EMCPin(s, HAL_BIT, HAL_IO) );
    }
    else
    {
	s = sPin;
	s += "ivalue";
	Pins.push_back( EMCPin(s, HAL_U32, HAL_OUT) );

	s = sPin;
	s += "fvalue";
	Pins.push_back( EMCPin(s, HAL_FLOAT, HAL_OUT) );
    }
    m_bFirst = true;
}

EMCHIDValue::~EMCHIDValue(void)
{
}

static int SignExtend( int n, int nMax )
{
    n &= nMax;
    if ( n & ((nMax+1)>>1) )
    {
	n |= ~nMax;
    }
    return n;
}

static int Diff( int n, int old, int min, int max )
{
    int nDiff = n - old;
    return SignExtend(nDiff,max-min);
}

void EMCHIDValue::UpdatePin()
{
    HIDInputValue *pItem = (HIDInputValue *)m_pCfgItem;

    int n = m_pDeviceItem->Value;
    **(hal_s32_t **)(Pins[IN_RAW].pData) = n;

    if ( m_bIsEncoder )
    {
	if ( m_bFirst )
	    m_bFirst = false;
	else
	{
	    int nDelta = Diff( n, m_nLastDeviceCount, pItem->nLogicalMinOverride, pItem->nLogicalMaxOverride );
	    m_nEncoderCount += nDelta;

	    double pos = (double)(m_nEncoderCount - pItem->nLogicalMinOverride) * ( pItem->dOutputMax-pItem->dOutputMin) / (double)( pItem->nLogicalMaxOverride - pItem->nLogicalMinOverride + 1 );
	    **(hal_u32_t **)(Pins[IN_ENCODER_COUNT].pData) = m_nEncoderCount;
	    **(hal_float_t **)(Pins[IN_ENCODER_POSITION].pData) = pos;

	    LOG_MSG( m_Logger, LogTypes::Debug, QString("Encoder count %1, %2\n").arg(m_nEncoderCount).arg(pos) );
	}
	m_nLastDeviceCount = n;
    }
    else
    {
	// Scale n to 0-100%(x100)
	n = pItem->Scale(n);
	if ( pItem->bReverse )
	    n = POINTS_MAX_VAL - n;
	if ( pItem->bUseResponseCurve )
	    n = pItem->Interpolate(n);

	**(hal_u32_t **)(Pins[IN_IVALUE].pData) = n;

	hal_float_t d = pItem->DScale( n );
	**(hal_float_t **)(Pins[IN_FVALUE].pData) = d;
    }
}


bool EMCHIDValue::CheckOutputs()
{
    if ( m_bIsEncoder )
    {
        if ( **(hal_bit_t **)(Pins[IN_ENCODER_RESET].pData) )
        {
	    m_nEncoderCount = 0;
	    **(hal_bit_t **)(Pins[IN_ENCODER_RESET].pData) = 0;
        }
    }
    return false;
}


