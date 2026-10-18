#include "../stdafx.h"
#include <cstdio>
#include <string>
#include <cmath>
#include "../UtilityFunctions.h"

#include "Airframe.h"

/////////////////////////////////////////////////////////////////
//////////////////////// INTERIOR LIGHTS ////////////////////////
/////////////////////////////////////////////////////////////////

void Hawk::Systems::avIntLights::initCold()
{

}

void Hawk::Systems::avIntLights::initHot()
{

}

void Hawk::Systems::avIntLights::initHotInAir()
{

}

void Hawk::Systems::avIntLights::update(double frameTime)
{

}

/////////////////////////////////////////////////////////////////
//////////////////////// EXTERIOR LIGHTS ////////////////////////
/////////////////////////////////////////////////////////////////

void Hawk::Systems::avExtLights::initCold()
{

}

void Hawk::Systems::avExtLights::initHot()
{

}

void Hawk::Systems::avExtLights::initHotInAir()
{

}

void Hawk::Systems::avExtLights::setNavLt(double navLt_CMD)
{
	lightNav = navLt_CMD;
}

void Hawk::Systems::avExtLights::setTaxiLt(double taxiLt_CMD)
{
	lightTaxi = taxiLt_CMD;
}

void Hawk::Systems::avExtLights::setAntiColTop(double acTop_CMD)
{

}

void Hawk::Systems::avExtLights::setAntiColBtm(double acBtm_CMD)
{

}

double Hawk::Systems::avExtLights::getNavLt()
{
	return lightNav;
}

double Hawk::Systems::avExtLights::getTaxiLt()
{
	return lightTaxi;
}

double Hawk::Systems::avExtLights::getAntiColTop()
{
	return 0;
}

double Hawk::Systems::avExtLights::getAntiColBtm()
{
	return 0;
}

void Hawk::Systems::avExtLights::update(double frameTime)
{

}

/////////////////////////////////////////////////////////////////
//////////////////////// FLIGHT CONTROLS ////////////////////////
/////////////////////////////////////////////////////////////////

double Hawk::Airframe::setFlapL(double flap_CMD, double frameTime)
{
	if (!simInit)
	{
		L_flapPos_DEG = flap_CMD;
		return L_flapPos_DEG;
	}
	L_flapRate_DPS = 7.0 * (flap_CMD - L_flapPos_DEG);
	L_flapRate_DPS = limit(L_flapRate_DPS, -0.3125, 0.3125);
	L_flapPos_DEG += (L_flapRate_DPS * frameTime);
	L_flapPos_DEG = limit(L_flapPos_DEG, 0.0, 100.0);

	return L_flapPos_DEG;
}

double Hawk::Airframe::setFlapR(double flap_CMD, double frameTime)
{
	if (!simInit)
	{
		R_flapPos_DEG = flap_CMD;
		return R_flapPos_DEG;
	}
	R_flapRate_DPS = 7.0 * (flap_CMD - R_flapPos_DEG);
	R_flapRate_DPS = limit(R_flapRate_DPS, -0.3125, 0.3125);
	R_flapPos_DEG += (R_flapRate_DPS * frameTime);
	R_flapPos_DEG = limit(R_flapPos_DEG, 0.0, 100.0);

	return R_flapPos_DEG;
}

double Hawk::Airframe::setAirbrake(double airbrake_CMD, double frameTime)
{
	if (!simInit)
	{
		airbrake_state = airbrake_CMD;
		return airbrake_state;
	}
	airbrake_rate = 20.2 * (airbrake_CMD - airbrake_state);
	airbrake_rate = limit(airbrake_rate, -0.55, 0.55);
	airbrake_state += (airbrake_rate * frameTime);
	airbrake_state = limit(airbrake_state, 0.0, 1.0);

	return airbrake_state;
}

double Hawk::Airframe::setElevator(double pitch_CMD, double pitchTrim_CMD, double frameTime)
{
	if (!simInit)
	{
		elevPos_DEG = pitch_CMD;
		return elevPos_DEG;
	}
	if (pitchTrim_CMD >= 0)
	{
		elevRate_DPS = 80.0 * ((pitch_CMD / 1.8) - (pitchTrim_CMD / 1.8) - elevPos_DEG);
		elevRate_DPS = limit(elevRate_DPS, -2.5, 2.5);
	}
	else
	{
		elevRate_DPS = 80.0 * ((-pitchTrim_CMD / 1.8) + (pitch_CMD / 1.8) - elevPos_DEG);
		elevRate_DPS = limit(elevRate_DPS, -2.5, 2.5);
	}
	elevPos_DEG += (elevRate_DPS * frameTime);
	elevPos_DEG = limit(elevPos_DEG, -100, 100);

	return elevPos_DEG;
}

double Hawk::Airframe::setAileronL(double roll_CMD, double rollTrim_CMD, double frameTime)
{
	if (!simInit)
	{
		L_ailPos_DEG = roll_CMD;
		return L_ailPos_DEG;
	}
	L_ailRate_DPS = 80.0 * ((-rollTrim_CMD / 1.8) + (roll_CMD / 1.8) - L_ailPos_DEG);
	L_ailRate_DPS = limit(L_ailRate_DPS, -2.5, 2.5);
	L_ailPos_DEG += (L_ailRate_DPS * frameTime);
	L_ailPos_DEG = limit(L_ailPos_DEG, -100.0, 100.0);

	return L_ailPos_DEG;
}

double Hawk::Airframe::setAileronR(double roll_CMD, double rollTrim_CMD, double frameTime)
{
	if (!simInit)
	{
		R_ailPos_DEG = roll_CMD;
		return R_ailPos_DEG;
	}
	R_ailRate_DPS = 80.0 * ((-rollTrim_CMD / 1.8) + (roll_CMD / 1.8) - R_ailPos_DEG);
	R_ailRate_DPS = limit(R_ailRate_DPS, -2.5, 2.5);
	R_ailPos_DEG += (R_ailRate_DPS * frameTime);
	R_ailPos_DEG = limit(R_ailPos_DEG, -100.0, 100.0);

	return R_ailPos_DEG;
}

double Hawk::Airframe::setRudder(double yaw_CMD, double yawTrim_CMD, double frameTime)
{
	if (!simInit)
	{
		rudPos_DEG = yaw_CMD;
		return rudPos_DEG;
	}
	rudRate_DPS = 80.0 * ((-yawTrim_CMD / 2.8) + (yaw_CMD / 1.8) - rudPos_DEG);
	rudRate_DPS = limit(rudRate_DPS, -2.5, 2.5);
	rudPos_DEG += (rudRate_DPS * frameTime);
	rudPos_DEG = limit(rudPos_DEG, -100.0, 100.0);

	return rudPos_DEG;
}

double Hawk::Airframe::setThrottleInput(double throttle_CMD, double frameTime)
{
	if (!simInit)
	{
		throttlePos = throttle_CMD;
		return throttlePos;
	}

	double timeConstant = 0.7;
	double rateOfChange = (throttle_CMD - throttlePos) / timeConstant;

	throttlePos += rateOfChange * frameTime;
	throttlePos = limit(throttlePos, 0, 100.0);

	return throttlePos;
}

double Hawk::Airframe::setWheelBrakeL(double command, double dt)
{
	if (!simInit)
	{
		wheelBrakeL = command;
		return wheelBrakeL;
	}

	double constant = 0.8;
	double rate = (command - wheelBrakeL) / constant;

	wheelBrakeL += rate * dt;
	wheelBrakeL = limit(wheelBrakeL, 0, 1);

	return wheelBrakeL;
}

double Hawk::Airframe::setWheelBrakeR(double command, double dt)
{
	if (!simInit)
	{
		wheelBrakeR = command;
		return wheelBrakeR;
	}

	double constant = 0.8;
	double rate = (command - wheelBrakeR) / constant;

	wheelBrakeR += rate * dt;
	wheelBrakeR = limit(wheelBrakeR, 0, 1);

	return wheelBrakeR;
}

double Hawk::Airframe::getWheelBrakeL()
{
	return wheelBrakeL;
}

double Hawk::Airframe::getWheelBrakeR()
{
	return wheelBrakeR;
}

double Hawk::Airframe::getFlaps()
{
	return L_flapPos_DEG;
	return R_flapPos_DEG;
}

double Hawk::Airframe::getAirbrake()
{
	return airbrake_state;
}

double Hawk::Airframe::getElevator()
{
	return elevPos_DEG;
}

double Hawk::Airframe::getAilerons()
{
	return L_ailPos_DEG;
	return R_ailPos_DEG;
}

double Hawk::Airframe::getRudder()
{
	return rudPos_DEG;
}

double Hawk::Airframe::getThrottleInput()
{
	return throttlePos;
}

double Hawk::Airframe::setDRMState(double DRMState)
{
	if (DRMState == true)
	{
		PlaneDRM = 0;
	}
	else if (DRMState == false)
	{
		PlaneDRM = 1;
	}
	return PlaneDRM;
}

double Hawk::Airframe::getDRMState()
{
	return PlaneDRM;
}