#pragma once

#ifndef __VARS__
#define __VARS__

#define NOMINMAX

namespace Imp
{
	static double ThrottleInput;

	static int cockpit_command;
	static int cockpit_command_value;

	static bool joystick_transmit_trigger;

	static bool cold_start;
	static bool hot_start;
	static bool hot_start_in_air;

	static bool cockpit_command_trigger;

	void* ParamHandle_Server_Port;
}

#endif