#include "../stdafx.h"
#include <stdio.h>
#include <string>
#include <math.h>
#include "../UtilityFunctions.h"

#include "Actuators.h"

double Actuators::gear_actuator(double gear_CMD, double frameTime)
{
	if (!simInit)
	{
		gear_state = gear_CMD;
		return gear_state;
	}
	gear_rate = 80.0 * (gear_CMD - gear_state);
	gear_rate = limit(gear_rate, -0.2222, 0.15625);
	gear_state += (gear_rate * frameTime);
	gear_state = limit(gear_state, 0.0, 100.0);

	return gear_state;
}

double Actuators::airbrake_actuator(double airbrake_CMD, double frameTime)
{
	if (!simInit)
	{
		airbrake_state = airbrake_CMD;
		return airbrake_state;
	}
	airbrake_rate = 20.2 * (airbrake_CMD - airbrake_state);
	airbrake_rate = limit(airbrake_rate, -0.6, 0.6);
	airbrake_state += (airbrake_rate * frameTime);
	airbrake_state = limit(airbrake_state, 0.0, 1.0);

	return airbrake_state;
}

void Actuators::setCanopy(double dt)
{
	if (canopyCMD == 1 && canopyPos < 1)
	{
		canopyPos += dt / 1.75;
		canopyPos = limit(canopyPos, 0, 1);
	}
	else if (canopyCMD == 0 && canopyPos > 0)
	{
		canopyPos -= dt / 2;
		canopyPos = limit(canopyPos, 0, 1);
	}
}

void Actuators::simulate(double dt)
{
	setCanopy(dt);
}