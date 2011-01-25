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

#include "stdafx.h"
#include "emcinterface.h"

// emc files
#include "emcglb.h"
#include "config.h"
#include "inifile.hh"
#include "timer.hh"
#include "nml_oi.hh"
#include <assert.h>

#include <QFileInfo>


EmcInterface::EmcInterface()
: m_Logger( QCoreApplication::applicationName(), "EmcInterface" )
, m_bEmcDebug( false )
, m_emcCommandBuffer( NULL )
, m_emcStatusBuffer( NULL )
, m_emcStatus( NULL )
, m_emcErrorBuffer( NULL )
{
}

EmcInterface::~EmcInterface(void)
{
    Close();
}

void EmcInterface::Close()
{
    if ( m_emcCommandBuffer != NULL )
	delete m_emcCommandBuffer;
    if ( m_emcStatusBuffer != NULL )
	delete m_emcStatusBuffer;
    if ( m_emcErrorBuffer != NULL )
	delete m_emcErrorBuffer;

    m_emcCommandBuffer = NULL;
    m_emcStatusBuffer = NULL;
    m_emcStatus = NULL;
    m_emcErrorBuffer = NULL;
}



bool EmcInterface::Initialise( int argc, char *argv[] )
{
    if ( emcGetArgs(argc, argv) != 0 ) 
    {
	LOG_MSG( m_Logger, LogTypes::Error, "error in argument list" );
        return false;
    }

    LOG_MSG( m_Logger, LogTypes::Debug, QString("Loading emc ini file '%1'").arg(EMC_INIFILE) );
    if ( !iniLoad(EMC_INIFILE) )
	return false;

    // init NML
    if ( !tryNml() ) 
    {
        LOG_MSG( m_Logger, LogTypes::Error, "can't connect to emc" );
        return false;
    }

    return true;
}



bool EmcInterface::iniLoad(const char *filename)
{
    IniFile inifile;
    const char *inistring;
    //char displayString[LINELEN] = "";
    //int t;
    //int i;

    // open it
    if (inifile.Open(filename) == false) 
    {
	LOG_MSG( m_Logger, LogTypes::Error, QString("Failed to open ini file - '%1'").arg(filename) );
	return false;
    }

    inistring = inifile.Find("DEBUG", "EMC");
    if ( inistring != NULL ) 
	m_bEmcDebug = QString( inistring ).toInt() != 0;


    inistring = inifile.Find("NML_FILE", "EMC");
    if ( inistring != NULL ) 
    {
	strcpy(EMC_NMLFILE, inistring);
	LOG_MSG( m_Logger, LogTypes::Debug, QString("Using nml file '%1'").arg(EMC_NMLFILE) );
    }

 //   for (t = 0; t < EMC_AXIS_MAX; t++) {
	//jogPol[t] = 1;          // set to default
	//sprintf(displayString, "AXIS_%d", t);
	//if (NULL != (inistring =
	//    inifile.Find("JOGGING_POLARITY", displayString)) &&
	//    1 == sscanf(inistring, "%d", &i) && i == 0) {
	//	// it read as 0, so override default
	//	jogPol[t] = 0;
	//}
 //   }

 //   if (NULL != (inistring = inifile.Find("LINEAR_UNITS", "DISPLAY"))) {
	//if (!strcmp(inistring, "AUTO")) {
	//    linearUnitConversion = LINEAR_UNITS_AUTO;
	//} else if (!strcmp(inistring, "INCH")) {
	//    linearUnitConversion = LINEAR_UNITS_INCH;
	//} else if (!strcmp(inistring, "MM")) {
	//    linearUnitConversion = LINEAR_UNITS_MM;
	//} else if (!strcmp(inistring, "CM")) {
	//    linearUnitConversion = LINEAR_UNITS_CM;
	//}
 //   } else {
	//// not found, leave default alone
 //   }

 //   if (NULL != (inistring = inifile.Find("ANGULAR_UNITS", "DISPLAY"))) {
	//if (!strcmp(inistring, "AUTO")) {
	//    angularUnitConversion = ANGULAR_UNITS_AUTO;
	//} else if (!strcmp(inistring, "DEG")) {
	//    angularUnitConversion = ANGULAR_UNITS_DEG;
	//} else if (!strcmp(inistring, "RAD")) {
	//    angularUnitConversion = ANGULAR_UNITS_RAD;
	//} else if (!strcmp(inistring, "GRAD")) {
	//    angularUnitConversion = ANGULAR_UNITS_GRAD;
	//}
 //   } else {
	//// not found, leave default alone
 //   }

    // close it
    inifile.Close();

    return true;
}



bool EmcInterface::tryNml()
{
    double end;
    bool bGood;
#define RETRY_TIME 10.0         // seconds to wait for subsystems to come up
#define RETRY_INTERVAL 1.0      // seconds between wait tries for a subsystem

    //if ((EMC_DEBUG & EMC_DEBUG_NML) == 0) {
    //    set_rcs_print_destination(RCS_PRINT_TO_NULL);   // inhibit diag
    //    // messages
    //}

    end = RETRY_TIME;
    bGood = false;
    do 
    {
        if ( emcTaskNmlGet() ) 
	{
            bGood = true;
            break;
        }
        esleep(RETRY_INTERVAL);
        end -= RETRY_INTERVAL;
    } 
    while (end > 0.0);

    //if ((EMC_DEBUG & EMC_DEBUG_NML) == 0) {
    //    set_rcs_print_destination(RCS_PRINT_TO_STDOUT); // inhibit diag
    //    // messages
    //}

    if ( !bGood ) 
        return false;

    //if ((EMC_DEBUG & EMC_DEBUG_NML) == 0) {
    //    set_rcs_print_destination(RCS_PRINT_TO_NULL);   // inhibit diag
    //    // messages
    //}

    end = RETRY_TIME;
    bGood = false;
    do 
    {
        if ( emcErrorNmlGet() ) 
	{
            bGood = true;
            break;
        }
        esleep(RETRY_INTERVAL);
        end -= RETRY_INTERVAL;
    } 
    while (end > 0.0);

    //if ((EMC_DEBUG & EMC_DEBUG_NML) == 0) {
    //    set_rcs_print_destination(RCS_PRINT_TO_STDOUT); // inhibit diag
    //    // messages
    //}

    return bGood;
}




bool EmcInterface::emcTaskNmlGet()
{
    bool bRetval = true;

    // try to connect to EMC cmd
    if (m_emcCommandBuffer == NULL) 
    {
        m_emcCommandBuffer = new RCS_CMD_CHANNEL(emcFormat, const_cast<char*>("emcCommand"), const_cast<char*>("hidcomp"), EMC_NMLFILE);

        if (!m_emcCommandBuffer->valid()) 
	{
            delete m_emcCommandBuffer;
            m_emcCommandBuffer = NULL;
            bRetval = false;
        }
    }

    // try to connect to EMC status
    if (m_emcStatusBuffer == NULL) 
    {
        m_emcStatusBuffer = new RCS_STAT_CHANNEL(emcFormat, const_cast<char*>("emcStatus"), const_cast<char*>("hidcomp"), EMC_NMLFILE);

        if (!m_emcStatusBuffer->valid() || EMC_STAT_TYPE != m_emcStatusBuffer->peek()) 
	{
            delete m_emcStatusBuffer;
            m_emcStatusBuffer = NULL;
            m_emcStatus = NULL;
            bRetval = false;
        } 
	else 
	{
            m_emcStatus = (EMC_STAT *) m_emcStatusBuffer->get_address();
        }
    }

    return bRetval;
}

bool EmcInterface::emcErrorNmlGet()
{
    bool bRetval = true;;

    if ( m_emcErrorBuffer == NULL) 
    {
        m_emcErrorBuffer = new NML(nmlErrorFormat, const_cast<char*>("emcError"), const_cast<char*>("hidcomp"), EMC_NMLFILE);

        if (!m_emcErrorBuffer->valid()) 
	{
            delete m_emcErrorBuffer;
            m_emcErrorBuffer = NULL;
            bRetval = false;
        }
    }

    return bRetval;
}




bool EmcInterface::updateStatus()
{
    if ( m_emcStatus == NULL || m_emcStatusBuffer == NULL || !m_emcStatusBuffer->valid() ) 
    {
        return false;
    }

    NMLTYPE type = m_emcStatusBuffer->peek();
    switch ( type ) 
    {
	case -1:	    // error on CMS channel
	    return false;

	case 0:		    // no new data
	case EMC_STAT_TYPE: // new data
	    break;

	default:
	    return false;
    }

    return true;
}

static QString GetString( const char *s, int nMaxLen )
{
    QString sRet;
    for ( int i = 0; i < nMaxLen; i++, s++ )
	if ( *s )
	    sRet += QChar( *s );
	else
	    break;
    
    return sRet.trimmed();;
}


/*
  updateError() updates "errors," which are true errors and also
  operator display and text messages.
*/
bool EmcInterface::updateError()
{
    if ( m_emcErrorBuffer == NULL || !m_emcErrorBuffer->valid() ) 
    {
        return false;
    }

    for (;;)
    {
	NMLTYPE type = m_emcErrorBuffer->peek();
	if ( type != 0 )
	{
	    LOG_MSG( m_Logger, LogTypes::Debug, QString("Received message type: %1").arg(type) );
	}

	switch ( type ) 
	{
	    case -1:			    // error reading channel
		return false;

	    case 0:
		// nothing new
		return true;

	    case EMC_OPERATOR_ERROR_TYPE:
		m_sLastMsg = m_sErrorString = GetString( ((EMC_OPERATOR_ERROR *)(m_emcErrorBuffer->get_address()))->error, LINELEN - 1);
		break;

	    case EMC_OPERATOR_TEXT_TYPE:
		m_sLastMsg = m_sOperatorTextString = GetString( ((EMC_OPERATOR_TEXT *) (m_emcErrorBuffer->get_address()))->text, LINELEN - 1);
		break;

	    case EMC_OPERATOR_DISPLAY_TYPE:
		m_sLastMsg = m_sOperatorDisplayString = GetString( ((EMC_OPERATOR_DISPLAY *) (m_emcErrorBuffer->get_address()))->display, LINELEN - 1);
		break;

	    case NML_ERROR_TYPE:
		m_sLastMsg = m_sErrorString = GetString( ((NML_ERROR *) (m_emcErrorBuffer->get_address()))->error, NML_ERROR_LEN - 1);
		break;

	    case NML_TEXT_TYPE:
		m_sLastMsg = m_sOperatorTextString = GetString( ((NML_TEXT *) (m_emcErrorBuffer->get_address()))->text, NML_TEXT_LEN - 1);
		break;

	    case NML_DISPLAY_TYPE:
		m_sLastMsg = m_sOperatorDisplayString = ((EMC_OPERATOR_DISPLAY *) ((NML_DISPLAY *) (m_emcErrorBuffer->get_address()))->display, NML_DISPLAY_LEN - 1);
		break;

	    default:
		// if not recognized, set the error string
		m_sLastMsg = m_sErrorString = QString( "unrecognized error %1").arg(type);
		return false;
	}
    }

    return true;
}


bool EmcInterface::Update()
{
    updateStatus();
    updateError();
    return true;
}




QString EmcInterface::GetStringData( ELCDDisplayData::ELCDDisplayData eData, int /*index*/ )
{
    switch ( eData )
    {
	case ELCDDisplayData::OperatorError: 
	    return m_sErrorString;

	case ELCDDisplayData::OperatorText: 
	    return m_sOperatorTextString;

	case ELCDDisplayData::OperatorDisplay: 
	    return m_sOperatorDisplayString;

	case ELCDDisplayData::OperatorMessage: 
	    return m_sLastMsg;

	case ELCDDisplayData::TaskFilePath:
	    return GetString( m_emcStatus->task.file, LINELEN );

	case ELCDDisplayData::TaskFileName:
	{
	    QFileInfo fi( GetString( m_emcStatus->task.file, LINELEN ) );
	    return fi.fileName();
	}

	case ELCDDisplayData::TaskCommand:
	    return GetString( m_emcStatus->task.command, LINELEN );

	default:
	    assert( false );
	    break;
    }

    return QString();
}


int EmcInterface::GetIntData( ELCDDisplayData::ELCDDisplayData eData, int nIndex )
{
    switch ( eData )
    {
	case ELCDDisplayData::TaskMode:
	    return m_emcStatus->task.mode & 0xFF;

	case ELCDDisplayData::TaskState:
	    return m_emcStatus->task.state & 0xFF;

	case ELCDDisplayData::TaskExecState:
	    return m_emcStatus->task.execState & 0xFF;

	case ELCDDisplayData::TaskInterpState:
	    return m_emcStatus->task.interpState & 0xFF;

	case ELCDDisplayData::TaskMotionLine:
	    return m_emcStatus->task.motionLine;

	case ELCDDisplayData::TaskCurrentLine:
	    return m_emcStatus->task.currentLine;

	case ELCDDisplayData::TaskOptionalStopState:
	    return m_emcStatus->task.optional_stop_state & 0xFF;

	case ELCDDisplayData::TaskBlockDeleteState:
	    return m_emcStatus->task.block_delete_state & 0xFF;

	case ELCDDisplayData::TaskActiveGCodes:
	    if ( nIndex < 0 || nIndex >= ACTIVE_G_CODES )
		nIndex = 0;
	    return m_emcStatus->task.activeGCodes[nIndex];

	case ELCDDisplayData::TaskActiveMCodes:
	    if ( nIndex < 0 || nIndex >= ACTIVE_M_CODES )
		nIndex = 0;
	    return m_emcStatus->task.activeMCodes[nIndex];

	case ELCDDisplayData::TaskPaused:
	    return m_emcStatus->task.task_paused & 0xFF;

	case ELCDDisplayData::MotionTrajMode: 
	    return m_emcStatus->motion.traj.mode & 0xFF; 

	case ELCDDisplayData::MotionTrajEnabled:  
	    return m_emcStatus->motion.traj.enabled;

	case ELCDDisplayData::MotionTrajInPos: 
	    return m_emcStatus->motion.traj.inpos;

	case ELCDDisplayData::MotionTrajPaused:  
	    return m_emcStatus->motion.traj.paused;

	case ELCDDisplayData::MotionTrajProbeTripped: 
	    return m_emcStatus->motion.traj.probe_tripped;

	case ELCDDisplayData::MotionTrajProbing: 
	    return m_emcStatus->motion.traj.probing;

	case ELCDDisplayData::MotionTrajProbeValue: 
	    return m_emcStatus->motion.traj.probeval;

	case ELCDDisplayData::MotionTrajFeedOverride: 
	    return m_emcStatus->motion.traj.feed_override_enabled;

	case ELCDDisplayData::MotionTrajSpindleOverride:  
	    return m_emcStatus->motion.traj.spindle_override_enabled;

	case ELCDDisplayData::MotionTrajAdaptiveFeedEnable: 
	    return m_emcStatus->motion.traj.adaptive_feed_enabled;

	case ELCDDisplayData::MotionTrajFeedHoldEnabled: 
	    return m_emcStatus->motion.traj.feed_hold_enabled;

	case ELCDDisplayData::MotionAxisInPos:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].inpos;

	case ELCDDisplayData::MotionAxisHoming:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].homing;

	case ELCDDisplayData::MotionAxisHomed:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].homed;

	case ELCDDisplayData::MotionAxisFault:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].fault;

	case ELCDDisplayData::MotionAxisEnabled:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].enabled;

	case ELCDDisplayData::MotionAxisMinSoftLimit:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].minSoftLimit;

	case ELCDDisplayData::MotionAxisMaxSoftLimit:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].maxSoftLimit;

	case ELCDDisplayData::MotionAxisMinHardLimit: 
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].minHardLimit;

	case ELCDDisplayData::MotionAxisMaxHardLimit:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].maxHardLimit;

	case ELCDDisplayData::MotionAxisOverrideLimits:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].overrideLimits;

	case ELCDDisplayData::MotionSpindleDirection:
	    return m_emcStatus->motion.spindle.direction;

	case ELCDDisplayData::MotionSpindleBrake:
	    return m_emcStatus->motion.spindle.brake;

	case ELCDDisplayData::MotionSpindleIncreasing:
	    return m_emcStatus->motion.spindle.increasing;

	case ELCDDisplayData::MotionSpindleEnabled:
	    return m_emcStatus->motion.spindle.enabled;

	case ELCDDisplayData::IOToolPrepped:
	    return m_emcStatus->io.tool.pocketPrepped;

	case ELCDDisplayData::IOToolInSpindle:
	    return m_emcStatus->io.tool.toolInSpindle;

	case ELCDDisplayData::IOCoolantMist:
	    return m_emcStatus->io.coolant.mist;

	case ELCDDisplayData::IOCoolantFlood:
	    return m_emcStatus->io.coolant.flood;

	case ELCDDisplayData::IOAuxEStop:
	    return m_emcStatus->io.aux.estop;

	case ELCDDisplayData::IOLubeOn:
	    return m_emcStatus->io.lube.on;

	case ELCDDisplayData::IOLubeLevel:
	    return m_emcStatus->io.lube.level;

	default:
	    assert( false );
	    break;
    }

    return 0;
}


static double GetPose( EmcPose &pos, int nIndex )
{
    switch (nIndex)
    {
	case 0:	return pos.tran.x;
	case 1: return pos.tran.y;
	case 2: return pos.tran.z;
	case 3: return pos.a;
	case 4: return pos.b;
	case 5: return pos.c;
	case 6: return pos.u;
	case 7: return pos.v;
	case 8: return pos.w;
	default:
	    assert( false );
	    return 0;
    }
}


double EmcInterface::GetFloatData( ELCDDisplayData::ELCDDisplayData eData, int nIndex )
{
    switch ( eData )
    {
	case ELCDDisplayData::TaskActiveSettings:
	    if ( nIndex < 0 || nIndex >= ACTIVE_SETTINGS )
		nIndex = 0;
	    return m_emcStatus->task.activeSettings[nIndex];

	case ELCDDisplayData::MotionTrajCommandPos:
	    return GetPose( m_emcStatus->motion.traj.position, nIndex );

	case ELCDDisplayData::MotionTrajActCommandPos: 
	    return GetPose( m_emcStatus->motion.traj.actualPosition, nIndex );

	case ELCDDisplayData::MotionTrajVelocity: 
	    return m_emcStatus->motion.traj.velocity;

	case ELCDDisplayData::MotionTrajAcceleration: 
	    return m_emcStatus->motion.traj.acceleration;

	case ELCDDisplayData::MotionTrajProbePos:
	    return GetPose( m_emcStatus->motion.traj.probedPosition, nIndex );

	case ELCDDisplayData::MotionTrajDistanceToGo:
	    return m_emcStatus->motion.traj.distance_to_go;

	case ELCDDisplayData::MotionTrajDTG:
	    return GetPose( m_emcStatus->motion.traj.dtg, nIndex );

	case ELCDDisplayData::MotionTrajCurrentVel:
	    return m_emcStatus->motion.traj.current_vel;

	case ELCDDisplayData::MotionAxisFError:
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].ferrorCurrent;

	case ELCDDisplayData::MotionAxisOutput: 
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].output;

	case ELCDDisplayData::MotionAxisInput: 
	    if ( nIndex < 0 || nIndex >= EMC_AXIS_MAX )
		nIndex = 0;
	    return m_emcStatus->motion.axis[nIndex].input;

	case ELCDDisplayData::MotionSpindleSpeed: 
	    return m_emcStatus->motion.spindle.speed;

	default:
	    assert( false );
	    return 0;
    }
}

