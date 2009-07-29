#ifndef _LCDDISPLAY_DATA_
#define _LCDDISPLAY_DATA_

namespace ELCDDisplayData
{
    enum ELCDDisplayData
    {
	None                        = -1,

	OperatorError               = 1,    // EMC_OPERATOR_ERROR_TYPE
	OperatorText                = 2,    // EMC_OPERATOR_TEXT_TYPE
	OperatorDisplay             = 3,    // EMC_OPERATOR_DISPLAY_TYPE
	OperatorMessage             = 4,    // The last operator message
	TaskMode                    = 5,    // EMC_TASK_STAT.mode (EMC_TASK_MODE_ENUM - manual, auto, mdi)
	TaskState                   = 6,    // EMC_TASK_STATE.state (EMC_TASK_STATE_ENUM - estop, estop-reset, off, on)
	TaskExecState               = 7,    // EMC_TASK_STATE.execState (EMC_TASK_EXEC_ENUM - error, done, wait for motion, ... )
	TaskInterpState             = 8,    // EMC_TASK_STATE.interpState (EMC_TASK_INTERP_ENUM - idle, reading, paused, waiting )
	TaskMotionLine              = 9,    // EMC_TASK_STATE.motionLine (int)
	TaskCurrentLine             = 10,   // EMC_TASK_STATE.currentLine (int)
	TaskOptionalStopState       = 11,   // EMC_TASK_STATE.optional_stop_state (bool)
	TaskBlockDeleteState        = 12,   // EMC_TASK_STATE.block_delete_state (bool)
	TaskFilePath                = 13,   // EMC_TASK_STATE.file str[LINELEN]
	TaskCommand                 = 14,   // EMC_TASK_STATE.command str[LINELEN]
	TaskActiveGCodes            = 15,   // EMC_TASK_STATE.activeGCodes[ACTIVE_G_CODES]
	TaskActiveMCodes            = 16,   // EMC_TASK_STATE.activeMCodes[ACTIVE_M_CODES
	TaskActiveSettings          = 17,   // EMC_TASK_STATE.activeSettings[ACTIVE_SETTINGS
	TaskPaused                  = 18,   // EMC_TASK_STATE.task_paused (bool)
	TaskFileName                = 19,   // Filename part of EMC_TASK_STATE.file str[LINELEN]

	MotionTrajMode              = 100,  // EMC_TRAJ_STAT.mode - EMC_TRAJ_MODE_ENUM - EMC_TRAJ_MODE_FREE, EMC_TRAJ_MODE_COORD, TELEOP
	MotionTrajEnabled           = 101,  // EMC_TRAJ_STAT.enabled;                // non-zero means enabled
	MotionTrajInPos             = 102,  // EMC_TRAJ_STAT.inpos;                  // non-zero means in position
	MotionTrajPaused            = 103,  // EMC_TRAJ_STAT.paused;                 // non-zero means motion paused
	MotionTrajCommandPos        = 104,  // EMC_TRAJ_STAT.positionX;           // current commanded position
	MotionTrajActCommandPos     = 105,  // EMC_TRAJ_STAT.actualPositionX;     // current actual position, from forward kins
	MotionTrajVelocity          = 106,  // EMC_TRAJ_STAT.velocity;            // system velocity, for subsequent motions
	MotionTrajAcceleration      = 107,  // EMC_TRAJ_STAT.acceleration;        // system acceleration, for subsequent
	MotionTrajProbePos          = 108,  // EmcPose EMC_TRAJ_STAT.probedPosition;     // last position where probe was tripped.
	MotionTrajProbeTripped      = 109,  // int EMC_TRAJ_STAT.probe_tripped;          // Has the probe been tripped since the last    // clear.
	MotionTrajProbing           = 110,  // int EMC_TRAJ_STAT.probing;                // Are we currently looking for a probe    // signal.
	MotionTrajProbeValue        = 111,  // int EMC_TRAJ_STAT.probeval;               // Current value of probe input.
	MotionTrajDistanceToGo      = 112,  // EMC_TRAJ_STAT.distance_to_go;         // in current move
	MotionTrajDTG               = 113,  // EMC_TRAJ_STAT.EmcPose dtg;
	MotionTrajCurrentVel        = 114,  // EMC_TRAJ_STAT.current_vel;         // in current move
	MotionTrajFeedOverride      = 115,  // EMC_TRAJ_STAT.feed_override_enabled;
	MotionTrajSpindleOverride   = 116,  // EMC_TRAJ_STAT.spindle_override_enabled;
	MotionTrajAdaptiveFeedEnable= 117,  // EMC_TRAJ_STAT.adaptive_feed_enabled;
	MotionTrajFeedHoldEnabled   = 118,  // EMC_TRAJ_STAT.feed_hold_enabled;

	MotionAxisFError            = 200,   // EMC_AXIS_STAT.ferrorCurrent;       // current following error
	MotionAxisOutput            = 201,   // EMC_AXIS_STAT.output;              // commanded output position
	MotionAxisInput             = 202,   // EMC_AXIS_STAT.input;               // current input position
	MotionAxisInPos             = 203,   // EMC_AXIS_STAT.inpos;        // non-zero means in position
	MotionAxisHoming            = 204,   // EMC_AXIS_STAT.homing;       // non-zero means homing
	MotionAxisHomed             = 205,   // EMC_AXIS_STAT.homed;        // non-zero means has been homed
	MotionAxisFault             = 206,   // EMC_AXIS_STAT.fault;        // non-zero means axis amp fault
	MotionAxisEnabled           = 207,   // EMC_AXIS_STAT.enabled;      // non-zero means enabled
	MotionAxisMinSoftLimit      = 208,   // EMC_AXIS_STAT.minSoftLimit; // non-zero means min soft limit exceeded
	MotionAxisMaxSoftLimit      = 209,   // EMC_AXIS_STAT.maxSoftLimit; // non-zero means max soft limit exceeded
	MotionAxisMinHardLimit      = 210,   // EMC_AXIS_STAT.minHardLimit; // non-zero means min hard limit exceeded
	MotionAxisMaxHardLimit      = 211,   // EMC_AXIS_STAT.maxHardLimit; // non-zero means max hard limit exceeded
	MotionAxisOverrideLimits    = 212,   // EMC_AXIS_STAT.overrideLimits; // non-zero means limits are overridden

	MotionSpindleSpeed          = 300,   // EMC_SPINDLE_STAT.speed
	MotionSpindleDirection      = 301,   // EMC_SPINDLE_STAT.direction
	MotionSpindleBrake          = 302,   // EMC_SPINDLE_STAT.brake
	MotionSpindleIncreasing     = 303,   // EMC_SPINDLE_STAT.increasing
	MotionSpindleEnabled        = 304,   // EMC_SPINDLE_STAT.enabled

	//EMC_MOTION_STAT.synch_di[EMC_MAX_DIO];  // motion inputs queried by interp
	//EMC_MOTION_STAT.analog_input[EMC_MAX_AIO]; //motion analog inputs queried by interp

	IOToolPrepped               = 400,   // EMC_IO_STAT.EMC_TOOL_STAT.toolPrepped
	IOToolInSpindle             = 401,   // EMC_IO_STAT.EMC_TOOL_STAT.toolInSpindle

	IOCoolantMist               = 500,   // EMC_IO_STAT.EMC_COOLANT_STAT.mist
	IOCoolantFlood              = 501,   // EMC_IO_STAT.EMC_COOLANT_STAT.flood

	IOAuxEStop                  = 600,   // EMC_IO_STAT.EMC_AUX_STAT.estop
	IOAuxDOut                   = 601,   // EMC_IO_STAT.EMC_AUX_STAT.dout [EMC_AUX_MAX_DOUT]
	IOAuxDIn                    = 602,   // EMC_IO_STAT.EMC_AUX_STAT.din [EMC_AUX_MAX_DIN]
	IOAuxAOut                   = 602,   // EMC_IO_STAT.EMC_AUX_STAT.aout [EMC_AUX_MAX_AOUT]
	IOAuxAIn                    = 603,   // EMC_IO_STAT.EMC_AUX_STAT.ain [EMC_AUX_MAX_AIN]

	IOLubeOn                    = 700,   // EMC_IO_STAT.EMC_LUBE_STAT.on
	IOLubeLevel                 = 701,   // EMC_IO_STAT.EMC_LUBE_STAT.level


	UserS32                     = 800,
	UserU32                     = 801,
	UserFloat                   = 802,
	UserBit                     = 803,
    };
}

#endif
