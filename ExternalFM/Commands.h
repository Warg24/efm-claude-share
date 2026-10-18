#pragma once

enum inputControls
{
	JoystickPitch = 2001,
	JoystickRoll = 2002,
	JoystickYaw = 2003,		//2003,
	JoystickThrottle = 2004,
	JoystickLeftEngineThrottle = 2005,
	JoystickRightEngineThrottle = 2006,

	iCommandPlaneFlaps = 72,
	iCommandPlaneUpStart = 193,
	iCommandPlaneUpStop = 194,
	iCommandPlaneDownStart = 195,
	iCommandPlaneDownStop = 196,
	iCommandPlaneLeftStart = 197,
	iCommandPlaneLeftStop = 198,
	iCommandPlaneRightStart = 199,
	iCommandPlaneRightStop = 200,
	iCommandPlaneLeftRudderStart = 201,
	iCommandPlaneLeftRudderStop = 202,
	iCommandPlaneRightRudderStart = 203,
	iCommandPlaneRightRudderStop = 204,
	iCommandThrottleIncrease = 1032,
	iCommandThrottleDecrease = 1033,

	iCommandCockpitShowPilotOnOff = 835,

	iCommandPlaneLGPAntiSkid = 1394,

	Eject = 83,

	canopyToggle = 71,

	airbrakeTOGGLE = 73,
	airbrakeEXTEND = 147,
	airbrakeRETRACT = 148,

	LandingGearToggle = 68,
	LandingGearUp = 430,		// Gear up	(doesn't work)	
	LandingGearDown = 431,	// Gear down	(doesn't work)

	WheelBrakeOn = 74,
	WheelBrakeOff = 75,
	
	WheelBrake_L = 8001,
	WheelBrake_R = 8002,

	flapsDOWN = 145,
	flapsUP = 146,

	trimUp = 95,
	trimDown = 96,
	trimLeftRudder = 98,
	trimRightRudder = 99,
	trimLeft = 93,
	trimRight = 94,
	trimReset = 97,
	trimStop = 215,

	engineStart = 309,
	engineStop = 310,

	Battery1 = 5000,
	Battery2 = 5001,
	AC1Reset = 5002,
	AC2Reset = 5003,
	AC3Reset = 5004,

	EngN1_Throttle = 7000,
	EngN2_Throttle = 7001,
	EngN1_Start = 7002,
	EngN2_Start = 7003,
	EngN1_Stop = 7004,
	EngN2_Stop = 7005,
	ToggleNWS = 7006,
	ParkingBrake = 7007,
	DebugToggle = 7008,
	GTS_PUSH = 7009,
	Battery = 7011,
	L_GEN = 7012,
	R_GEN = 7013,
	NWS_ON = 7014,
	NWS_OFF = 7015,
	CANOPY_OPEN = 7016,
	CANOPY_CLOSE = 7017,
	
	WheelBrakeOn_L = 7018,
	WheelBrakeOff_L = 7019,
	WheelBrakeOn_R = 7020,
	WheelBrakeOff_R = 7021,

	ControlLock = 7030,
	
};