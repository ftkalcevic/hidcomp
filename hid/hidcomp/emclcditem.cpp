#include "stdafx.h"
#include "emclcditem.h"
#include "emcinterface.h"
#include "lcddataformatter.h"
#include <assert.h>

    
#define	MAX_LINE_WIDTH	    128	    // some arbitrary number.  LCDs usually aren't more than 40 wide.


void EMCLCDItem::Init()
{
    m_bLastBool = false;
    m_dLastFloat = 0;
    m_nLastU32 = 0;
    m_nLastS32 = 0;
    m_sLastString.clear();
}

void EMCLCDItem::ProcessFormatString( LCDEntry *pLCDEntry )
{
    LCDDataFormatter::ProcessFormatString( pLCDEntry->format(), m_sFormat, m_sLookupDefault, m_Lookup );
}

bool EMCLCDItem::CheckDataChange( std::vector<EMCPin> &Pins, bool bForceUpdate )
{
    QString s;

    switch ( m_eData )
    {
	case ELCDDisplayData::OperatorError: 
	case ELCDDisplayData::OperatorText: 
	case ELCDDisplayData::OperatorDisplay: 
	case ELCDDisplayData::OperatorMessage: 
	case ELCDDisplayData::TaskFileName:
	case ELCDDisplayData::TaskFilePath:
	case ELCDDisplayData::TaskCommand:
	{
	    QString sValue = emcIFace.GetStringData( m_eData, m_nIndex );
	    if ( bForceUpdate || sValue != m_sLastString )
	    {
		m_sLastString = sValue;
		s = FormatData( sValue );
	    }
	    break;
	}

	case ELCDDisplayData::TaskMode:
	case ELCDDisplayData::TaskState:
	case ELCDDisplayData::TaskExecState:
	case ELCDDisplayData::TaskInterpState:
	case ELCDDisplayData::TaskMotionLine:
	case ELCDDisplayData::TaskCurrentLine:
	case ELCDDisplayData::TaskOptionalStopState:
	case ELCDDisplayData::TaskBlockDeleteState:
	case ELCDDisplayData::TaskActiveGCodes:
	case ELCDDisplayData::TaskActiveMCodes:
	case ELCDDisplayData::TaskPaused:
	case ELCDDisplayData::MotionTrajMode:
	case ELCDDisplayData::MotionTrajEnabled:
	case ELCDDisplayData::MotionTrajInPos:
	case ELCDDisplayData::MotionTrajPaused:
	case ELCDDisplayData::MotionTrajProbeTripped:
	case ELCDDisplayData::MotionTrajProbing:
	case ELCDDisplayData::MotionTrajProbeValue:
	case ELCDDisplayData::MotionTrajFeedOverride:
	case ELCDDisplayData::MotionTrajSpindleOverride:
	case ELCDDisplayData::MotionTrajAdaptiveFeedEnable:
	case ELCDDisplayData::MotionTrajFeedHoldEnabled:
	case ELCDDisplayData::MotionAxisInPos:
	case ELCDDisplayData::MotionAxisHoming:
	case ELCDDisplayData::MotionAxisHomed:
	case ELCDDisplayData::MotionAxisFault:
	case ELCDDisplayData::MotionAxisEnabled:
	case ELCDDisplayData::MotionAxisMinSoftLimit:
	case ELCDDisplayData::MotionAxisMaxSoftLimit:
	case ELCDDisplayData::MotionAxisMinHardLimit:
	case ELCDDisplayData::MotionAxisMaxHardLimit:
	case ELCDDisplayData::MotionAxisOverrideLimits:
	case ELCDDisplayData::MotionSpindleDirection:
	case ELCDDisplayData::MotionSpindleBrake:
	case ELCDDisplayData::MotionSpindleIncreasing:
	case ELCDDisplayData::MotionSpindleEnabled:
	case ELCDDisplayData::IOToolPrepped:
	case ELCDDisplayData::IOToolInSpindle:
	case ELCDDisplayData::IOCoolantMist:
	case ELCDDisplayData::IOCoolantFlood:
	case ELCDDisplayData::IOAuxEStop:
	case ELCDDisplayData::IOLubeOn:
	case ELCDDisplayData::IOLubeLevel:
	{
	    int n = emcIFace.GetIntData( m_eData, m_nIndex );
	    if ( bForceUpdate || n != m_nLastS32 )
	    {
		m_nLastS32 = n;
		s = FormatData( n );
	    }
	    break;
	}

	case ELCDDisplayData::TaskActiveSettings:
	case ELCDDisplayData::MotionTrajCommandPos:
	case ELCDDisplayData::MotionTrajActCommandPos:
	case ELCDDisplayData::MotionTrajVelocity:
	case ELCDDisplayData::MotionTrajAcceleration:
	case ELCDDisplayData::MotionTrajProbePos:
	case ELCDDisplayData::MotionTrajDistanceToGo:
	case ELCDDisplayData::MotionTrajDTG:
	case ELCDDisplayData::MotionTrajCurrentVel:
	case ELCDDisplayData::MotionAxisFError:
	case ELCDDisplayData::MotionAxisOutput:
	case ELCDDisplayData::MotionAxisInput:
	case ELCDDisplayData::MotionSpindleSpeed:
	{
	    double d = emcIFace.GetFloatData( m_eData, m_nIndex );
	    if ( bForceUpdate || d != m_dLastFloat )
	    {
		m_dLastFloat = d;
		s = FormatData( d );
	    }
	    break;
	}

	case ELCDDisplayData::UserS32: 
	{
	    int n = **(hal_s32_t **)(Pins[m_nPinIndex].pData);
	    if ( bForceUpdate || n != m_nLastS32 )
	    {
		m_nLastS32 = n;
		s = FormatData( n );
	    }
	    break;
	}
	case ELCDDisplayData::UserU32: 
	{
	    unsigned int n = **(hal_u32_t **)(Pins[m_nPinIndex].pData);
	    if ( bForceUpdate || n != m_nLastU32 )
	    {
		m_nLastU32 = n;
		s = FormatData( n );
	    }
	    break;
	}
	case ELCDDisplayData::UserFloat: 
	{
	    double n = **(hal_float_t **)(Pins[m_nPinIndex].pData);
	    if ( bForceUpdate || n != m_dLastFloat )
	    {
		m_dLastFloat = n;
		s = FormatData( n );
	    }
	    break;
	}
	case ELCDDisplayData::UserBit:
	{
	    bool b = (**(hal_bit_t **)(Pins[m_nPinIndex].pData)) != 0;
	    if ( bForceUpdate || b != m_bLastBool )
	    {
		m_bLastBool = b;
		s = FormatData( b );
	    }
	    break;
	}
	default:
	    assert( m_eData || false );
	    break;
    }

    if ( s != m_sLastFormattedData )
    {
	m_sLastFormattedData = s;
	return true;
    }
    else
	return false;
}


QString EMCLCDItem::FormatData( int n )
{
    if ( m_Lookup.count() > 0 )
	return FormatEnum( n );
    return LCDDataFormatter::snprintf( m_sFormat.constData(), n );
}

QString EMCLCDItem::FormatData( unsigned int n )
{
    if ( m_Lookup.count() > 0 )
	return FormatEnum( (int)n );
    return LCDDataFormatter::snprintf( m_sFormat.constData(), n );
}

QString EMCLCDItem::FormatData( double d )
{
    if ( m_Lookup.count() > 0 )
	return FormatEnum( (int)d );
    return LCDDataFormatter::snprintf( m_sFormat.constData(), d );
}

QString EMCLCDItem::FormatData( bool b )
{
    if ( m_Lookup.count() > 0 )
	return FormatEnum( (int)b );
    return LCDDataFormatter::snprintf( m_sFormat.constData(), b );
}

QString EMCLCDItem::FormatData( const QString &s )
{
    return LCDDataFormatter::snprintf( m_sFormat.constData(), s.toAscii().constData() );
}


QString EMCLCDItem::FormatEnum( int n )
{
    QMap<int,QString>::iterator it = m_Lookup.find( n );
    QString sValue;
    if ( it != m_Lookup.end() )
	sValue = it.value();
    else
	sValue = m_sLookupDefault;

    return FormatData( sValue );
}
