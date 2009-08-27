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
#include "lcdconfigdlg.h"
#include "lcddisplaydata.h"
#include "usages.h"
#include "lcddataformatter.h"
#include "editfontdlg.h"

//#ifdef _WIN32
    #define ACTIVE_G_CODES 16
    #define ACTIVE_M_CODES 10
    #define ACTIVE_SETTINGS 3

    #define EMCMOT_MAX_AXIS 9
    #define EMC_AXIS_MAX EMCMOT_MAX_AXIS

    #define EMC_AUX_MAX_DOUT 4      // digital out bytes
    #define EMC_AUX_MAX_DIN  4      // digital in bytes
    #define EMC_AUX_MAX_AOUT 32     // analog out points
    #define EMC_AUX_MAX_AIN  32     // analog in points

//#endif

#define MAX_USER_TYPE       0xFFFF


struct LCDData
{
    ELCDDisplayData::ELCDDisplayData eData;      // The data that is to be displayed
    const char *sDisplayName;   // Default format String
    EDisplayDataType eType;     // Data type for testing
    int nIndexMin;              // For indexed items, min index value (min=max for non indexed items)
    int nIndexMax;              // max index value
    const char *sDefaultFormat; // Default format String
    const char *sToolTip;
};


static LCDData lcdData[] = {
    { ELCDDisplayData::OperatorError,            "Operator Error",               StringType, 0, 0, "%20s", "Operator Error Messages" },              // EMC_OPERATOR_ERROR_TYPE
    { ELCDDisplayData::OperatorText,             "Operator Text",                StringType, 0, 0, "%20s", "Operator Text Messages" },               // EMC_OPERATOR_TEXT_TYPE
    { ELCDDisplayData::OperatorDisplay,          "Operator Display",             StringType, 0, 0, "%20s", "Operator Display Messages" },       // EMC_OPERATOR_DISPLAY_TYPE 
    { ELCDDisplayData::OperatorMessage,          "Operator Message",             StringType, 0, 0, "%20s", "The last Operator Messages" },       // The last operator msg
    { ELCDDisplayData::TaskMode,                 "Mode",                         IntType,    0, 0, "%6:1,Manual:2,Auto:3,MDI:b", "EMC Task Mode - 1=Manual, 2=Auto, 3=MDI" },                   // EMC_TASK_STAT.mode (EMC_TASK_MODE_ENUM - manual, auto, mdi)
    { ELCDDisplayData::TaskState,                "State",                        IntType,    0, 0, "%12:1,E-Stop:2,E-Stop/Reset:3,Off:4,On:b", "EMC Task State - 1=E-Stop, 2=E-Stop/Reset, 3=Off, 4=On" },    // EMC_TASK_STATE.state (EMC_TASK_STATE_ENUM - estop, estop-reset, off, on)
    { ELCDDisplayData::TaskExecState,            "Execution State",              IntType,    0, 0, "%20:1,Error:2,Done:3,Waiting for Motion:4,Wait for Motion Queue:5,Waiting for IO:6,Waiting for Pause:7,Wait for Motion & IO:8,Waiting for Delay:9,Waiting for Sys Cmd:b", "EMC Executing state 1=Error, 2=Done, 3=Waiting for Motion, 4=Waiting for Motion Queue, 5=Waiting for IO, 6=Waiting for Pause, 7=Waiting for Motion and IO, 8=Waiting for Delay, 9=Waiting for System Cmd" }, // EMC_TASK_STATE.execState (EMC_TASK_EXEC_ENUM - error, done, wait for motion, ... )
    { ELCDDisplayData::TaskInterpState,          "Interpretter State",           IntType,    0, 0, "%7:1,Idle:2,Reading:3,Paused:4,Waiting:b", "EMC Interpretter state - 1=Idle, 2=Reading, 3=Paused, 4=Waiting" },    // EMC_TASK_STATE.interpState (EMC_TASK_INTERP_ENUM - idle, reading, paused, waiting )
    { ELCDDisplayData::TaskMotionLine,           "Motion Line",                  IntType,    0, 0, "%5d", "The line in the GCode file that corresponds to the current motion position" }, // // EMC_TASK_STATE.motionLine (int)
    { ELCDDisplayData::TaskCurrentLine,          "Current Line",                 IntType,    0, 0, "%5d", "The line in the GCode file that corresponds to the line the interpretter is looking at" }, // EMC_TASK_STATE.currentLine (int)
    { ELCDDisplayData::TaskOptionalStopState,    "Optional Stop State",          IntType,    0, 0, "%5:0,false:1,true:b", "Optional Stop State" }, // EMC_TASK_STATE.optional_stop_state (bool)
    { ELCDDisplayData::TaskBlockDeleteState,     "Block Delete State",           IntType,    0, 0, "%5:0,false:1,true:b", "Block Delete State" },   // EMC_TASK_STATE.block_delete_state (bool)
    { ELCDDisplayData::TaskFilePath,             "GCode File Path",              StringType, 0, 0, "%20s", "The full path of the current GCode program" }, // EMC_TASK_STATE.file str[LINELEN]
    { ELCDDisplayData::TaskFileName,             "GCode File Name",              StringType, 0, 0, "%20s", "The file name of the current GCode program" }, // EMC_TASK_STATE.file str[LINELEN]
    { ELCDDisplayData::TaskCommand,              "GCode Command",                StringType, 0, 0, "%20s", "The current GCode command being interpretted" }, // EMC_TASK_STATE.command str[LINELEN]
    { ELCDDisplayData::TaskActiveGCodes,         "Active GCodes",                IntType,    0, ACTIVE_G_CODES-1, "G%02d", "Active GCodes.  Index from 0 to 15" }, // EMC_TASK_STATE.activeGCodes[ACTIVE_G_CODES]
    { ELCDDisplayData::TaskActiveMCodes,         "Active MCodes",                IntType,    0, ACTIVE_M_CODES-1, "M%02d", "Active MCodes.  Index from 0 to 9" }, // EMC_TASK_STATE.activeMCodes[ACTIVE_M_CODES
    { ELCDDisplayData::TaskActiveSettings,       "Active Settings",              FloatType,  0, ACTIVE_SETTINGS-1, "%d", "Active settings.  Index from 0 to 2" }, // EMC_TASK_STATE.activeSettings[ACTIVE_SETTINGS
    { ELCDDisplayData::TaskPaused,               "Paused",                       IntType,    0, 0, "Paused=%5:0,false:1,true:b", "Task Paused" }, // EMC_TASK_STATE.task_paused (bool)

    { ELCDDisplayData::MotionTrajMode,           "Trajectory Mode",              IntType,    0, 0, "%6:1,Free:2,Coord:3,Teleop:b", "Trajectory Mode 1=Free, 2=Coord, 3=Teleop" },       // EMC_TRAJ_STAT.mode - EMC_TRAJ_MODE_ENUM - EMC_TRAJ_MODE_FREE, EMC_TRAJ_MODE_COORD, TELEOP
    { ELCDDisplayData::MotionTrajEnabled,        "Trajectory Enabled",           IntType,    0, 0, "%5:0,false:1,true:b", "Trajectory Enabled" },  // EMC_TRAJ_STAT.enabled;                // non-zero means enabled
    { ELCDDisplayData::MotionTrajInPos,          "In Position",                  IntType,    0, 0, "%5:0,false:1,true:b", "Motion in position" }, // EMC_TRAJ_STAT.inpos;                  // non-zero means in position
    { ELCDDisplayData::MotionTrajPaused,         "Paused",                       IntType,    0, 0, "%5:0,false:1,true:b", "Motion paused" }, // EMC_TRAJ_STAT.paused;                 // non-zero means motion paused
    { ELCDDisplayData::MotionTrajCommandPos,     "Commanded Position[axis]",     FloatType,  0, 8, "%06.3f", "Commanded Position for each axis.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_TRAJ_STAT.positionX;           // current commanded position
    { ELCDDisplayData::MotionTrajActCommandPos,  "Actual Position[axis]",        FloatType,  0, 8, "%06.3f", "Actual Position for each axis.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_TRAJ_STAT.actualPositionX;     // current actual position, from forward kins
    { ELCDDisplayData::MotionTrajVelocity,       "System Velocity",              FloatType,  0, 0, "%06.3f", "System velocity for subsequent moves" }, // EMC_TRAJ_STAT.velocity;            // system velocity, for subsequent motions
    { ELCDDisplayData::MotionTrajAcceleration,   "System Acceleration",          FloatType,  0, 0, "%06.3f", "System acceleration for subsequent moves" },  // EMC_TRAJ_STAT.acceleration;        // system acceleration, for subsequent
    { ELCDDisplayData::MotionTrajProbePos,       "Last Probe Trip Pos[axis]",    FloatType,  0, 8, "%06.3f", "Last probe trip position for each axis.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EmcPose EMC_TRAJ_STAT.probedPosition;     // last position where probe was tripped.
    { ELCDDisplayData::MotionTrajProbeTripped,   "Probe Tripped",                IntType,    0, 0, "%5:0,false:1,true:b", "Probe tripped since last clear" }, // int EMC_TRAJ_STAT.probe_tripped;          // Has the probe been tripped since the last    // clear.
    { ELCDDisplayData::MotionTrajProbing,        "Probing",                      IntType,    0, 0, "%5:0,false:1,true:b", "We are currently probing" }, // int EMC_TRAJ_STAT.probing;                // Are we currently looking for a probe    // signal.
    { ELCDDisplayData::MotionTrajProbeValue,     "Probe Value",                  IntType,    0, 0, "%5:0,false:1,true:b", "Current probe value" }, // int EMC_TRAJ_STAT.probeval;               // Current value of probe input.
    { ELCDDisplayData::MotionTrajDistanceToGo,   "Distance to Go",               FloatType,  0, 0, "%06.3f", "Distance to go in current move" },  // EMC_TRAJ_STAT.distance_to_go;         // in current move
    { ELCDDisplayData::MotionTrajDTG,            "Distance to Go[axis]",         FloatType,  0, 8, "%06.3f", "Distance to go for each individual axis.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_TRAJ_STAT.EmcPose dtg;
    { ELCDDisplayData::MotionTrajCurrentVel,     "Current Velocity",             FloatType,  0, 0, "%06.3f", "Velocity of current move" },  // EMC_TRAJ_STAT.current_vel;         // in current move
    { ELCDDisplayData::MotionTrajFeedOverride,   "Feedrate Override Enabled",    IntType,    0, 0, "%5:0,false:1,true:b", "Feedrate override enabled" }, // EMC_TRAJ_STAT.feed_override_enabled;
    { ELCDDisplayData::MotionTrajSpindleOverride, "Spindle Override Enabled",    IntType,    0, 0, "%5:0,false:1,true:b", "Spindle override enabled" }, // EMC_TRAJ_STAT.spindle_override_enabled;
    { ELCDDisplayData::MotionTrajAdaptiveFeedEnable, "Adaptive Feed Enabled",    IntType,    0, 0, "%5:0,false:1,true:b", "Adaptive feed enabled" }, // EMC_TRAJ_STAT.adaptive_feed_enabled;
    { ELCDDisplayData::MotionTrajFeedHoldEnabled,"Feed Hold Enabled",            IntType,    0, 0, "%5:0,false:1,true:b", "Feed Hold Enabled" }, // EMC_TRAJ_STAT.feed_hold_enabled;

    { ELCDDisplayData::MotionAxisFError,         "FError[axis]",                 FloatType,  0, EMC_AXIS_MAX-1, "%05.3f", "Current following error for this axis. Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.ferrorCurrent;       // current following error
    { ELCDDisplayData::MotionAxisOutput,         "Command Output Position[axis]",FloatType,0, EMC_AXIS_MAX-1, "%05.3f", "Current Command Position for this axis. Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.output;              // commanded output position
    { ELCDDisplayData::MotionAxisInput,          "Current Input Position[axis]", FloatType,0, EMC_AXIS_MAX-1, "%05.3f", "Current Input Position for this axis. Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" },    // EMC_AXIS_STAT.input;               // current input position
    { ELCDDisplayData::MotionAxisInPos,          "In Position[axis]",            IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis is currently at the requested position.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.inpos;        // non-zero means in position
    { ELCDDisplayData::MotionAxisHoming,         "Homing[axis]",                 IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis is currently homing.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.homing;       // non-zero means homing
    { ELCDDisplayData::MotionAxisHomed,          "Homed[axis]",                  IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis is homed.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.homed;        // non-zero means has been homed
    { ELCDDisplayData::MotionAxisFault,          "Fault[axis]",                  IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis has faulted.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.fault;        // non-zero means axis amp fault
    { ELCDDisplayData::MotionAxisEnabled,        "Enabled[axis]",                IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis is enabled.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.enabled;      // non-zero means enabled
    { ELCDDisplayData::MotionAxisMinSoftLimit,   "Min Soft Limit[axis]",         IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis has exceeded the min soft limit.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.minSoftLimit; // non-zero means min soft limit exceeded
    { ELCDDisplayData::MotionAxisMaxSoftLimit,   "Max Soft Limit[axis]",         IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis has exceeded the max soft limit.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.maxSoftLimit; // non-zero means max soft limit exceeded
    { ELCDDisplayData::MotionAxisMinHardLimit,   "Min Hard Limit[axis]",         IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis has exceeded the min hard limit.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.minHardLimit; // non-zero means min hard limit exceeded
    { ELCDDisplayData::MotionAxisMaxHardLimit,   "Max Hard Limit[axis]",         IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "The axis has exceeded the max hard limit.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.maxHardLimit; // non-zero means max hard limit exceeded
    { ELCDDisplayData::MotionAxisOverrideLimits, "Override Limits[axis]",        IntType,    0, EMC_AXIS_MAX-1, "%5:0,false:1,true:b", "Override the limits.  Axis numbers from 0-8 represent axis x,y,z,a,b,c,u,v,w" }, // EMC_AXIS_STAT.overrideLimits; // non-zero means limits are overridden

    { ELCDDisplayData::MotionSpindleSpeed,       "Spindle Speed",                FloatType,  0, 0, "%05.3f", "Spindle speed in RPM" },       // EMC_SPINDLE_STAT.speed
    { ELCDDisplayData::MotionSpindleDirection,   "Spindle Direction",            IntType,    0, 0, "%7:0,stopped:1,forward:-1,reverse:b", "Spindle direction. 0=stopped, 1=forward, -1=reverse" }, // EMC_SPINDLE_STAT.direction
    { ELCDDisplayData::MotionSpindleBrake,       "Spindle Brake",                IntType,    0, 0, "%8:0,released:1,engaged:b", "Spindle brake. 0=released, 1=engaged" },   // EMC_SPINDLE_STAT.brake
    { ELCDDisplayData::MotionSpindleIncreasing,  "Spindle Increasing",           IntType,    0, 0, "%10:1,increasing:-1,decreasing:0,steady:b", "Spindle Speed Increasing. 1=increasing, -1=decreasing, 0=steady" }, // EMC_SPINDLE_STAT.increasing
    { ELCDDisplayData::MotionSpindleEnabled,     "Spindle Enabled",              IntType,    0, 0, "%5:0,false:1,true:b", "Spindle enabled" }, // EMC_SPINDLE_STAT.enabled
                                                                
    //EMC_MOTION_STAT.synch_di[EMC_MAX_DIO];  // motion inputs queried by interp
    //EMC_MOTION_STAT.analog_input[EMC_MAX_AIO]; //motion analog inputs queried by interp

    { ELCDDisplayData::IOToolPrepped,            "Tool Prepped",                 IntType,    0, 0, "%d", "Tool ready for loading.  0 is no tool" }, // EMC_IO_STAT.EMC_TOOL_STAT.toolPrepped
    { ELCDDisplayData::IOToolInSpindle,          "Tool In Spindle",              IntType,    0, 0, "%d", "Tool in spindle.  0 is no tool"},   // EMC_IO_STAT.EMC_TOOL_STAT.toolInSpindle

    { ELCDDisplayData::IOCoolantMist,            "Coolant-Mist",                 IntType,    0, 0, "%3:0,off:1,on:b", "Mist coolant. 0 - off, 1 - on" },// EMC_IO_STAT.EMC_COOLANT_STAT.mist
    { ELCDDisplayData::IOCoolantFlood,           "Coolant-Flood",                IntType,    0, 0, "%3:0,off:1,on:b", "Flood coolant. 0 - off, 1 - on" },// EMC_IO_STAT.EMC_COOLANT_STAT.flood

    { ELCDDisplayData::IOAuxEStop,               "E-Stop",                       IntType,    0, 0, "%7:0,run:1,stopped:b", "E-Stop - 0 means run, 1 stopped" },// EMC_IO_STAT.EMC_AUX_STAT.estop
//    { IOAuxDOut                 "Digital Out[index]",        FloatType,  0, EMC_AUX_MAX_DOUT-1, "%f", "Digtal Output.  Index is 0 to  },   // EMC_IO_STAT.EMC_AUX_STAT.dout [EMC_AUX_MAX_DOUT]
//    { IOAuxDIn                  "Digital In[index]",         FloatType,  0, EMC_AUX_MAX_DIN-1,  = 602,   // EMC_IO_STAT.EMC_AUX_STAT.din [EMC_AUX_MAX_DIN]
//    { IOAuxAOut                 "Analog Out[index]",         FloatType,  0, EMC_AUX_MAX_AOUT-1, = 602,   // EMC_IO_STAT.EMC_AUX_STAT.aout [EMC_AUX_MAX_AOUT]
//    { IOAuxAIn                  "Analog In[index]",          FloatType,  0, EMC_AUX_MAX_AIN-1,= 603,   // EMC_IO_STAT.EMC_AUX_STAT.ain [EMC_AUX_MAX_AIN]

    { ELCDDisplayData::IOLubeOn,                 "Lube On",                      IntType,    0, 0, "%3:0,off:1,on:b", "Lube on - 0=off, 1=on" }, // EMC_IO_STAT.EMC_LUBE_STAT.on
    { ELCDDisplayData::IOLubeLevel,              "Lube Level",                   IntType,    0, 0, "%4:0,low:1,okay:b", "Lube level - 0=low, 1=okay" },   // EMC_IO_STAT.EMC_LUBE_STAT.level

    { ELCDDisplayData::UserS32,                  "User S32[index]",              IntType,    0, MAX_USER_TYPE-1, "%d", "Custom User S32 value on pin <device>.lcd-s32-<index>.  Any Index values can be used" },
    { ELCDDisplayData::UserU32,                  "User U32[index]",              IntType,    0, MAX_USER_TYPE-1, "%u", "Custom User U32 value on pin <device>.lcd-u32-<index>.  Any Index values can be used" },
    { ELCDDisplayData::UserFloat,                "User Float[index]",            FloatType,  0, MAX_USER_TYPE-1, "%f", "Custom User Float value on pin <device>.lcd-float-<index>.  Any Index values can be used" },
    { ELCDDisplayData::UserBit,                  "User Bit[index]",              IntType,    0, MAX_USER_TYPE-1, "%d", "Custom User Bit value on pin <device>.lcd-bit-<index>.  Any Index values can be used" },
};

static LCDData *FindData( ELCDDisplayData::ELCDDisplayData nId )
{
    static QMap<ELCDDisplayData::ELCDDisplayData,LCDData *> map;
    if ( map.count() == 0 )
    {
        for ( unsigned int i = 0; i < countof(lcdData); i++ )
            map[lcdData[i].eData] = lcdData + i;
    }

    QMap<ELCDDisplayData::ELCDDisplayData,LCDData *>::iterator it = map.find( nId );
    if ( it != map.end() )
        return it.value();
    else
        return NULL;
}

#define DATA_COL_COL        0
#define DATA_ROW_COL        1
#define DATA_VALUE_COL      2
#define DATA_FORMAT_COL     3
#define DATA_SCALE_COL      4
#define DATA_INDEX_COL      5
#define DATA_TEST_VALUE_COL 6
#define DATA_COL            DATA_COL_COL

#define PAGE_NO_COL         0
#define PAGE_NAME_COL       1
#define PAGE_COL            PAGE_NO_COL



LCDConfigDlg::LCDConfigDlg(HIDLCD *pLCDData, HIDDevice *pDevice, HID_CollectionPath_t *pCol, int nRows, int nColumns, bool bUserFonts, int nMinFontIndex, int nMaxFontIndex, QWidget *parent)
: QDialog(parent)
, m_Logger( QCoreApplication::applicationName().toAscii().constData(), "LCDConfigDlg" )
, m_pDevice( pDevice )
, m_pCol( pCol )
, m_nRows(nRows)
, m_nColumns(nColumns)
, m_bUpdating( false )
, m_pCurrentPage( NULL )
, m_bUserFonts(bUserFonts)
, m_nMinFontIndex(nMinFontIndex)
, m_nMaxFontIndex(nMaxFontIndex)
{
    ui.setupUi(this);

    ui.tableData->setColumnWidth(DATA_COL_COL, 40);
    ui.tableData->setColumnWidth(DATA_ROW_COL, 40);
    ui.tableData->setColumnWidth(DATA_SCALE_COL, 40);
    ui.tableData->setColumnWidth(DATA_INDEX_COL, 40);
    ui.tableData->setSelectionMode( QAbstractItemView::SingleSelection );

    ui.tablePages->setColumnWidth(PAGE_NO_COL, 50);
    ui.tablePages->setSelectionMode( QAbstractItemView::SingleSelection );
    ui.tablePages->setSelectionBehavior( QAbstractItemView::SelectRows );

    m_pSpinBoxColDelegate = new SpinBoxDelegate(0,m_nColumns-1);
    m_pSpinBoxRowDelegate = new SpinBoxDelegate(0,m_nRows-1);
    m_pSpinBoxIndexDelegate = new SpinBoxDelegate(0,0x7FFFFFFF);
    ui.tableData->setItemDelegateForColumn( DATA_COL_COL, m_pSpinBoxColDelegate );
    ui.tableData->setItemDelegateForColumn( DATA_ROW_COL, m_pSpinBoxRowDelegate );
    ui.tableData->setItemDelegateForColumn( DATA_INDEX_COL, m_pSpinBoxIndexDelegate );

    m_pDataTypeDelegate = new ComboBoxDelegate();
    for ( unsigned int i = 0; i < countof(lcdData); i++ )
        m_pDataTypeDelegate->addItem( lcdData[i].eData, lcdData[i].sDisplayName );
    ui.tableData->setItemDelegateForColumn( DATA_VALUE_COL, m_pDataTypeDelegate );
    ui.tableData->setColumnWidth( DATA_VALUE_COL, 185 );    // Should do this better - ask combo its max width

    if ( !bUserFonts )
	ui.btnUserFonts->setVisible( false );

    ui.lcd->setSize( nRows, nColumns );

    setConfig( pLCDData );
}

LCDConfigDlg::~LCDConfigDlg()
{

}

void LCDConfigDlg::NewPageRow( int nRow, LCDWorkingPage *pPage )
{
    ui.tablePages->insertRow( nRow );

    QTableWidgetItem *item = new QTableWidgetItem( QString::number(pPage->nNumber), QMetaType::UInt);
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
    ui.tablePages->setItem(nRow, PAGE_NO_COL, item );

    item = new QTableWidgetItem( pPage->sName, QMetaType::QString );
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled );
    ui.tablePages->setItem(nRow, PAGE_NAME_COL, item );

    SetPagePointer( nRow, pPage );
}

void LCDConfigDlg::onNewPage()
{
    m_bUpdating = true;

    int nRow = ui.tablePages->rowCount();
    int nPageNo = nRow;
    QString sPageName = QString("Page %1").arg(nRow);
    LCDWorkingPage *pPage = new LCDWorkingPage(nPageNo, sPageName);

    NewPageRow( nRow, pPage );

    m_bUpdating = false;

    m_pCurrentPage = pPage;
    ClearDataTable();
    ui.tablePages->selectRow( nRow );
}

void LCDConfigDlg::onDeletePage()
{
    if ( m_pCurrentPage == NULL )
        return;

    delete m_pCurrentPage;
    m_pCurrentPage = NULL;

    int nRow = ui.tablePages->currentRow();
    ui.tablePages->removeRow( nRow );

    // Shuffle the numbers.  Pages are always 0-n
    for ( int i = nRow; i < ui.tablePages->rowCount(); i++ )
    {
        ui.tablePages->item(i, PAGE_NO_COL)->setText( QString::number(i) );
        LCDWorkingPage *pPage = GetPagePointer( i );
        pPage->nNumber = i;
    }
}

void LCDConfigDlg::onPageUp()
{
    if ( m_pCurrentPage == NULL )
	return;

    int nRow = ui.tablePages->currentRow();
    if ( nRow == 0 )
	return;

    // Renumber rows, then sort
    LCDWorkingPage *pPage;
    pPage = GetPagePointer( nRow );
    pPage->nNumber = nRow - 1;
    ui.tablePages->item( nRow, PAGE_NO_COL )->setText( QString::number(pPage->nNumber) );
    pPage = GetPagePointer( nRow - 1 );
    pPage->nNumber = nRow;
    ui.tablePages->item( nRow - 1, PAGE_NO_COL )->setText( QString::number(pPage->nNumber) );

    ui.tablePages->sortByColumn( 0, Qt::AscendingOrder );
}

void LCDConfigDlg::onPageDown()
{
    if ( m_pCurrentPage == NULL )
	return;

    int nRow = ui.tablePages->currentRow();
    if ( nRow == ui.tablePages->rowCount() - 1 )
	return;

    // Renumber rows, then sort
    LCDWorkingPage *pPage;
    pPage = GetPagePointer( nRow );
    pPage->nNumber = nRow + 1;
    ui.tablePages->item( nRow, PAGE_NO_COL )->setText( QString::number(pPage->nNumber) );
    pPage = GetPagePointer( nRow + 1 );
    pPage->nNumber = nRow;
    ui.tablePages->item( nRow+1, PAGE_NO_COL )->setText( QString::number(pPage->nNumber) );

    ui.tablePages->sortByColumn( 0, Qt::AscendingOrder );
}

void LCDConfigDlg::onPageItemChanged(QTableWidgetItem *item)
{
    if ( m_bUpdating )
        return;

    if ( item->column() == PAGE_NAME_COL )
    {
        if ( m_pCurrentPage != NULL )
            m_pCurrentPage->sName = item->text();
    }
}

void LCDConfigDlg::onPageCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous )
{
    if ( current == NULL && previous == NULL )
        return;

    if ( current != NULL && previous != NULL && current->row() == previous->row() )
        return;

    m_pCurrentPage = GetPagePointer( current->row() );

    // clear data entries table and populate with this page
    ClearDataTable();
    for ( int i = 0; i < m_pCurrentPage->Entries.count(); i++ )
    {
        InsertNewDataRow( m_pCurrentPage->Entries[i] );
    }
    DrawCurrentPage();
}

LCDWorkingPage *LCDConfigDlg::GetPagePointer( int nRow )
{
    LCDWorkingPage *pData = NULL;

    QTableWidgetItem *item  = ui.tablePages->item( nRow, PAGE_COL );
    if ( item != NULL )
    {
        void *ptr = qvariant_cast<void *>( item->data(Qt::UserRole) );
        pData = reinterpret_cast<LCDWorkingPage *>( ptr );
    }
    return pData;
}

void LCDConfigDlg::SetPagePointer( int nRow, LCDWorkingPage *pData )
{
    QTableWidgetItem *item  = ui.tablePages->item( nRow, PAGE_COL );
    assert( item != NULL );
    if ( item != NULL )
        item->setData(Qt::UserRole, qVariantFromValue<void *>( pData ) );
}

void LCDConfigDlg::InsertNewDataRow( LCDWorkingData *pData )
{
    m_bUpdating = true;
    try
    {
        int nRow = ui.tableData->rowCount();
        ui.tableData->insertRow( nRow );
        ui.tableData->setItem(nRow, DATA_COL_COL, new QTableWidgetItem(QString::number(pData->entry.col()), QMetaType::UInt) );
        ui.tableData->setItem(nRow, DATA_ROW_COL, new QTableWidgetItem(QString::number(pData->entry.row()), QMetaType::UInt) );
        ui.tableData->setItem(nRow, DATA_FORMAT_COL, new QTableWidgetItem(pData->entry.format(),QMetaType::QString) );
        ui.tableData->setItem(nRow, DATA_SCALE_COL, new QTableWidgetItem(QString::number(pData->entry.scale()), QMetaType::Double) );
        ui.tableData->setItem(nRow, DATA_INDEX_COL, new QTableWidgetItem(QString::number(pData->entry.index()), QMetaType::UInt) );
        ui.tableData->setItem(nRow, DATA_TEST_VALUE_COL, new QTableWidgetItem(pData->entry.testData()) );
        
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData( Qt::UserRole, pData->entry.data() );         // data( Qt::UserRole ) contains the combo box data id
        ui.tableData->setItem(nRow, DATA_VALUE_COL, item );
        LCDData *pEntry = FindData(pData->entry.data());
        if ( pEntry != NULL )
            item->setData( Qt::EditRole, pEntry->sDisplayName );    // data( Qt::EditRole ) contains the display string
        SetDataPointer( nRow, pData );
        m_bUpdating = false;

        LOG_MSG( m_Logger, LogTypes::Debug, QString("colwidth=%d").arg(ui.tableData->columnWidth(DATA_VALUE_COL)) );
    }
    catch (...)
    {
    }
}

void LCDConfigDlg::onNewData()
{
    if ( m_pCurrentPage == NULL )
        return;
    LCDEntry temp( ELCDDisplayData::None, 0, 0, 0, "", 1.0, "0" );
    LCDWorkingData *pData = new LCDWorkingData( temp );
    m_pCurrentPage->Entries.append( pData );
    InsertNewDataRow( pData );
}


void LCDConfigDlg::onDeleteData()
{
    if ( m_pCurrentPage == NULL )
        return;
    int nRow = ui.tableData->currentRow();
    if ( nRow >= 0 )
    {
        // remove from table
        LCDWorkingData *pData = GetDataPointer( nRow );
        if ( pData != NULL )
        {
            ui.tableData->removeRow(nRow);
            // remove from data set
            for ( int i = 0; i < m_pCurrentPage->Entries.count(); i++ )
                if ( m_pCurrentPage->Entries[i] == pData )
                {
                    m_pCurrentPage->Entries.removeAt(i);
                    break;
                }
            delete pData;
        }
	DrawCurrentPage();
    }
}

LCDWorkingData *LCDConfigDlg::GetDataPointer( int nRow )
{
    LCDWorkingData *pData = NULL;

    QTableWidgetItem *item  = ui.tableData->item( nRow, DATA_COL );
    if ( item != NULL )
    {
        void *ptr = qvariant_cast<void *>( item->data(Qt::UserRole) );
        pData = reinterpret_cast<LCDWorkingData *>( ptr );
    }
    return pData;
}

void LCDConfigDlg::SetDataPointer( int nRow, LCDWorkingData *pData )
{
    QTableWidgetItem *item  = ui.tableData->item( nRow, DATA_COL );
    assert( item != NULL );
    if ( item != NULL )
        item->setData(Qt::UserRole, qVariantFromValue<void *>( pData ) );
}

void LCDConfigDlg::ClearDataTable()
{
    while ( ui.tableData->rowCount() > 0 )
        ui.tableData->removeRow( 0 );

    LCDClear();
}

void LCDConfigDlg::onOK()
{
    accept();
}


void LCDConfigDlg::onCancel()
{
    reject();
}


void LCDConfigDlg::onItemChanged(QTableWidgetItem *item)
{
    if ( m_bUpdating )
        return;

    if ( item->column() == DATA_VALUE_COL )
    {
        LCDWorkingData *pData = GetDataPointer( item->row() );
        ELCDDisplayData::ELCDDisplayData nOldId = pData->entry.data();

        ELCDDisplayData::ELCDDisplayData nId = (ELCDDisplayData::ELCDDisplayData)item->data(Qt::UserRole).toInt();
        LCDData *data = FindData( nId );
        LCDData *oldData = FindData( nOldId );
        if ( data != NULL )
        {
            item->setToolTip( data->sToolTip );

            QTableWidgetItem *formatItem = ui.tableData->item( item->row(), DATA_FORMAT_COL );
            if ( formatItem->text().isEmpty() || oldData == NULL || formatItem->text() == oldData->sDefaultFormat )
                formatItem->setText( data->sDefaultFormat );
        }
    }

    // Lazy - we just update all fields when 1 cell changes
    int nRow = item->row();
    LCDWorkingData *pData = GetDataPointer( nRow );
    pData->entry.setCol( ui.tableData->item(nRow, DATA_COL_COL)->text().toInt() );
    pData->entry.setRow( ui.tableData->item(nRow, DATA_ROW_COL)->text().toInt() );
    pData->entry.setFormat( ui.tableData->item(nRow, DATA_FORMAT_COL)->text() );
    pData->entry.setScale( ui.tableData->item(nRow, DATA_SCALE_COL)->text().toDouble() );
    pData->entry.setIndex( ui.tableData->item(nRow, DATA_INDEX_COL)->text().toInt() );
    pData->entry.setTestData( ui.tableData->item(nRow, DATA_TEST_VALUE_COL)->text() );
    pData->entry.setData( (ELCDDisplayData::ELCDDisplayData)ui.tableData->item(nRow,DATA_VALUE_COL)->data(Qt::UserRole).toInt() );

    DisplaySample( item->row() );
}

void LCDConfigDlg::onCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous )
{
    if ( current == NULL && previous == NULL )
        return;

    if ( current != NULL && previous != NULL && current->row() == previous->row() )
        return;

    if ( previous != NULL )
    {
        // repaint the item no highlighted.
        LCDWorkingData *pData = GetDataPointer( previous->row() );
        if ( pData->nLastRow >= 0 && pData->nLastCol >= 0 && pData->sLastText.length() > 0 )
            LCDWrite( pData->nLastRow, pData->nLastCol, pData->sLastText, false );
    }
    if ( current != NULL )
    {
        // repaint the item highlighted.
        LCDWorkingData *pData = GetDataPointer( current->row() );
        if ( pData->nLastRow >= 0 && pData->nLastCol >= 0 && pData->sLastText.length() > 0 )
            LCDWrite( pData->nLastRow, pData->nLastCol, pData->sLastText, true );
    }
}


// Update the display in the LCD of a data item as it is changed in the table.
void LCDConfigDlg::DisplaySample( int row )
{
    int nRow = ui.tableData->item( row, DATA_ROW_COL)->text().toInt();
    int nCol = ui.tableData->item( row, DATA_COL_COL)->text().toInt();
    ELCDDisplayData::ELCDDisplayData eData = (ELCDDisplayData::ELCDDisplayData )ui.tableData->item(row, DATA_VALUE_COL)->data(Qt::UserRole).toInt();
    QString sFormat = ui.tableData->item(row, DATA_FORMAT_COL)->text();
    double dScale = ui.tableData->item(row, DATA_SCALE_COL)->text().toDouble();
    //int nIndex = ui.tableData->item( row, DATA_INDEX_COL)->text().toInt();
    QVariant vData = ui.tableData->item(row, DATA_TEST_VALUE_COL)->data(Qt::EditRole);

    if ( ( nRow >= 0 && nRow <= m_nRows ) &&
         ( nCol >= 0 && nCol <= m_nColumns ) &&
         ( eData > 0 ) &&
         ( !sFormat.isEmpty() ) &&
         ( vData.isValid() ) )
    {
        QString sData = FormatData( eData, sFormat, dScale, vData );

        LCDWorkingData *pData = GetDataPointer( row );
        if ( pData->nLastCol != nCol || pData->nLastRow != nRow || sData != pData->sLastText )
        {
            // when we update an item, repaint everything else first, just in case this one clobbered or is unclobbering another entry.
            LCDClear();
            for ( int i = 0; i < ui.tableData->rowCount(); i++ )
                if ( i != row )
                {
                    LCDWorkingData *pOtherData = GetDataPointer( i );
                    if ( pOtherData != NULL && pOtherData->nLastRow >= 0 && pOtherData->nLastCol >= 0 && pOtherData->sLastText.length() > 0 )
                        LCDWrite( pOtherData->nLastRow, pOtherData->nLastCol, pOtherData->sLastText, false );
                }

            LCDWrite( nRow, nCol, sData, true );
            pData->nLastCol = nCol;
            pData->nLastRow = nRow;
            pData->sLastText = sData;
        }
    }
}

void LCDConfigDlg::DrawCurrentPage( )
{
    LCDClear();
    for ( int i = 0; i < ui.tableData->rowCount(); i++ )
    {
        LCDWorkingData *pData = GetDataPointer( i );
        pData->nLastCol = pData->entry.col();
        pData->nLastRow = pData->entry.row();
        pData->sLastText = FormatData( pData->entry.data(), pData->entry.format(), pData->entry.scale(), QVariant( pData->entry.testData() ) );
        if ( !pData->sLastText.isEmpty() )
            LCDWrite( pData->nLastRow, pData->nLastCol, pData->sLastText, false );
    }
}

QString LCDConfigDlg::FormatData( ELCDDisplayData::ELCDDisplayData entryType, const QString &sFormat, double dScale, const QVariant &value )
{
    LCDData *pData = FindData( entryType );
    EDisplayDataType dataType = UnknownType;
    if ( pData != NULL )
        dataType = pData->eType;

    switch ( dataType )
    {
        case StringType:
            if ( value.canConvert(QVariant::String) )
                return FormatData( &sFormat, value.toString().toAscii().constData() );
            break;
        case IntType: 
            if ( value.canConvert(QVariant::Int) )
                return FormatData( &sFormat, (int)(value.toInt() * dScale) );
            break;
        case FloatType:
            if ( value.canConvert(QVariant::Double) )
                return FormatData( &sFormat, value.toDouble() * dScale );
            break;
        default:
        case UnknownType:
            break;
    }
    return QString();
}


QString LCDConfigDlg::FormatData( const QString *sFormat, ... )
{
    va_list args;
    va_start(args, sFormat);

    const int nBufLen = m_nColumns*2+1;
    QByteArray buf;
    buf.resize(nBufLen);

    // check for enum list (regardless of data type)
    QByteArray newFormatString;
    QString sDefault;
    QMap<int,QString> lookup;
    LCDDataFormatter::ProcessFormatString( *sFormat, newFormatString, sDefault, lookup );

    if ( lookup.size() != 0 )
    {
	// enum
	QString s;
	QMap<int,QString>::iterator it = lookup.find( va_arg(args,int) );
	if ( it != lookup.end() )
	    s = it.value();
	else
	    s = sDefault;

	return LCDDataFormatter::snprintf( newFormatString.constData(), s.toAscii().constData() );
    }
    else
	return LCDDataFormatter::vsnprintf( newFormatString.constData(), args );
}


void LCDConfigDlg::setConfig( HIDLCD *lcdData )
{
    m_bUpdating = true;

    try
    {
        ui.editRefreshRate->setValue( lcdData->samplePeriod() );
        // load pages
        int nPages = lcdData->pages().count();
        for ( int i = 0; i < nPages; i++ )
        {
            LCDPage *pPage = lcdData->pages()[i];
            LCDWorkingPage *pWorkingPage = new LCDWorkingPage( pPage->number(), pPage->name() );
            for ( int j = 0; j < pPage->data().count(); j++ )
            {
                LCDWorkingData *pWorkingData = new LCDWorkingData( *(pPage->data()[j]) );
                pWorkingPage->Entries.append(pWorkingData);
            }

            NewPageRow( i, pWorkingPage );
        }   
	// copy fonts
	m_fonts.clear();
	for ( int i = 0; i < lcdData->fonts().count(); i++ )
	    m_fonts.push_back( new LCDFont(*(lcdData->fonts()[i])) );
    }
    catch (...)
    {
    }
    m_bUpdating = false;
    LCDSendUserFonts();
    ui.tablePages->selectRow( 0 );
}


void LCDConfigDlg::getConfig( HIDLCD *lcdData )
{
    lcdData->setSamplePeriod( ui.editRefreshRate->value() );
    lcdData->pages().clear();
    int nPages = ui.tablePages->rowCount();
    for ( int i = 0; i < nPages; i++ )
    {
        LCDWorkingPage *pPage = GetPagePointer(i);
        LCDPage *pNewPage = new LCDPage(pPage->nNumber,pPage->sName );
        lcdData->pages().append( pNewPage );

        for ( int j = 0; j < pPage->Entries.count(); j++ )
        {
            LCDEntry *pEntry = &(pPage->Entries[j]->entry);
            LCDEntry *pNewEntry = new LCDEntry( pEntry->data(), pEntry->index(), pEntry->row(), pEntry->col(), pEntry->format(), pEntry->scale(), pEntry->testData() );

            pNewPage->data().append( pNewEntry );
        }
    }
    lcdData->fonts().clear();
    foreach ( LCDFont *pFont, m_fonts )
	lcdData->fonts().append( new LCDFont(*pFont) );
}

void LCDConfigDlg::LCDClear()
{
    for ( int i = 0; i < m_nRows; i++ )
	LCDWrite( i, 0, QString(m_nColumns,QChar(' ')), false );
}

void LCDConfigDlg::LCDSendUserFonts()
{
    HID_ReportItem_t *pCharIndex = m_pDevice->ReportInfo().FindReportItem( m_pCol, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_FONT_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_DISPLAY_DATA );
    if ( pCharIndex == NULL )
    {
	LOG_MSG( m_Logger, LogTypes::Error, "Failed to find USAGE_DISPLAY_DATA in font report" );
    }
    HID_CollectionPath_t *pCollection = pCharIndex->CollectionPath;
    byte nReportId = pCharIndex->ReportID;

    // Find the index to the first data item
    unsigned int nIndex = 0;
    for ( ; nIndex < pCollection->ReportItems.size(); nIndex++ )
	if ( pCollection->ReportItems[nIndex]->Attributes.Usage == USAGE_FONT_DATA )
	    break;

    foreach (LCDFont *pFont, m_fonts )
    {
	ui.lcd->SetUserFont( pFont->index(), pFont->data() );

	pCharIndex->Value = pFont->index();

	for ( int i = 0; i < pFont->data().count() && nIndex + i < pCollection->ReportItems.size(); i++ )
	    pCollection->ReportItems[nIndex + i]->Value = pFont->data()[i];

	HID_ReportDetails_t pReportDetails = m_pDevice->ReportInfo().Reports[nReportId];
	int nBufLen = pReportDetails.OutReportLength;
	int nOffset = 0;
	if ( m_pDevice->ReportInfo().Reports.size() > 1 )
	    nOffset=1;

	byte *buf = new byte[nBufLen+nOffset];
	if ( nOffset )
	    *buf = nReportId;

	HIDParser parser;
	parser.MakeOutputReport( buf + nOffset, (byte)nBufLen, m_pDevice->ReportInfo().ReportItems, nReportId );

	// Send the report
	int nRet = m_pDevice->InterruptWrite( buf, nBufLen + nOffset, USB_TIMEOUT );
	LOG_MSG( m_Logger, LogTypes::Debug, QString("interrupt write returned %1\n").arg(nRet).toLatin1().constData() );

	delete buf;
    }
}


void LCDConfigDlg::LCDWrite( int nRow, int nCol, QString sText, bool bHighlight )
{
    ui.lcd->Write( nRow, nCol, sText, bHighlight );

    HID_ReportItem_t *pRowItem = m_pDevice->ReportInfo().FindReportItem( m_pCol, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_CHARACTER_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_ROW );
    HID_ReportItem_t *pColItem = m_pDevice->ReportInfo().FindReportItem( m_pCol, REPORT_ITEM_TYPE_Out, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_CHARACTER_REPORT, USAGEPAGE_ALPHANUMERIC_DISPLAY, USAGE_COLUMN );
    HID_CollectionPath_t *pCollection = pColItem->CollectionPath;

    // Find the index to the first data item
    unsigned int nIndex = 0;
    for ( ; nIndex < pCollection->ReportItems.size(); nIndex++ )
	if ( pCollection->ReportItems[nIndex]->Attributes.Usage == USAGE_DISPLAY_DATA )
	    break;

    pRowItem->Value = nRow;
    pColItem->Value = nCol;
    for ( int i = 0; i < sText.length() && i < m_nColumns && nIndex+i < pCollection->ReportItems.size(); i++ )
	pCollection->ReportItems[nIndex + i]->Value = sText[i].toAscii();
    if ( sText.length() < m_nColumns )
	pCollection->ReportItems[nIndex + sText.length()]->Value = 0;

    HID_ReportDetails_t pReportDetails = m_pDevice->ReportInfo().Reports[pRowItem->ReportID];
    int nBufLen = pReportDetails.OutReportLength;
    int nOffset = 0;
    if ( m_pDevice->ReportInfo().Reports.size() > 1 )
	nOffset=1;

    byte *buf = new byte[nBufLen+nOffset];
    if ( nOffset )
	*buf = pRowItem->ReportID;

    HIDParser parser;
    parser.MakeOutputReport( buf + nOffset, (byte)nBufLen, m_pDevice->ReportInfo().ReportItems, pRowItem->ReportID );

    // Send the report
    int nRet = m_pDevice->InterruptWrite( buf, nBufLen + nOffset, USB_TIMEOUT );
    LOG_MSG( m_Logger, LogTypes::Debug, QString("interrupt write returned %1\n").arg(nRet).toLatin1().constData() );

    delete buf;
}


void LCDConfigDlg::onEditUserFonts()
{
    EditFontDlg dlg(m_fonts, m_nMinFontIndex, m_nMaxFontIndex, this);
    if ( dlg.exec() == QDialog::Accepted )
    {
	foreach ( LCDFont *pFont, m_fonts )
	    delete pFont;
	m_fonts.clear();
	m_fonts = dlg.fontData();

	LCDSendUserFonts();
    }
}
