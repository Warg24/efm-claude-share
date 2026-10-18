#include "ElectricSystem.h"
#include "../Engine/Adour.h"
//#include <Cockpit/cockpit_api_declare.h>
#include "../Commands.h"
#include "../UtilityFunctions.h"

using namespace Hawk;

//EDPARAM electricAPI;
Adour EngineAPI;

//void* BATT_1 = electricAPI.getParamHandle("BATT_1");
//void* BATT_2 = electricAPI.getParamHandle("BATT_2");
//void* AC_1 = electricAPI.getParamHandle("AC_1");
//void* AC_2 = electricAPI.getParamHandle("AC_2");
//void* AC_3 = electricAPI.getParamHandle("AC_3");
//void* VOLTAGE = electricAPI.getParamHandle("VOLTS");

// For now these are not necessary as they will be defined in LUA
void HawkElectricSystem::initCold()
{
	batt1 = false;
	batt2 = false;
}

void HawkElectricSystem::initHot()
{
	batt1 = true;
	batt2 = true;
}

void HawkElectricSystem::SetCommand(const int command, const float value)
{
	switch (command)
	{
	case Battery1:
		if (value == 1)
		{
			batt1 = true;
		}
		else
		{
			batt1 = false;
		}
		break;
		
	case Battery2:
		if (value == 1)
		{
			batt2 = true;
		}
		else
		{
			batt2 = false;
		}
		break;

	case AC1Reset:
		if (value == 1)
		{
			if (ac1 == false)
			{
				ac1 = true;
			}
		}
		break;

	case AC2Reset:
		if (value == 1)
		{
			if (ac2 == false)
			{
				ac2 = true;
			}
		}
		break;

	case AC3Reset:
		if (value == 1)
		{
			if (ac3 == false)
			{
				ac3 = true;
			}
		}
		break;
	}
}

void HawkElectricSystem::updateVolts(double dt)
{
	if (EngineAPI.getRPM() < 0.45)
	{
		if (batt1 == true)
		{
			volts = volts + 0.03 * dt;
			volts = limit(volts, 0, 0.3);
		}

		if (batt2 == true)
		{
			volts = volts + 0.03 * dt;
			volts = limit(volts, 0, 0.3);
		}

		if (batt1 == true && batt2 == true)
		{
			volts = volts + 0.01 * dt;
			volts = limit(volts, 0, 0.3);
		}
	}
	else if (EngineAPI.getRPM() > 0.45)
	{
		if (batt1 == true && batt2 == false)
		{
			volts = volts + 0.01 * dt;
			volts = limit(volts, 0, 0.55);
		}
		else if (batt1 == false && batt2 == true)
		{
			volts = volts + 0.01 * dt;
			volts = limit(volts, 0, 0.55);
		}
		else if (batt1 == true && batt2 == true)
		{
			volts = volts + 0.01 * dt;
			volts = limit(volts, 0, 0.55);
		}
	}

	if (volts > 0)
	{
		if (batt1 == false)
		{
			volts = volts - 0.01 * dt;
		}

		if (batt2 == false)
		{
			volts = volts - 0.01 * dt;
		}
	}
	volts = limit(volts, 0, 1);
}

void HawkElectricSystem::updateAC1(double dt)
{
	if (EngineAPI.getRPM() > 45)
	{
		if (getVolts() > 25)
		{
			ac1 = true;
		}
		else
		{
			ac1 = false;
		}
	}
	else
	{
		ac1 = false;
	}
}

void HawkElectricSystem::updateAC2(double dt)
{
	if (EngineAPI.getRPM() > 45)
	{
		if (getVolts() > 25)
		{
			ac2 = true;
		}
		else
		{
			ac2 = false;
		}
	}
	else
	{
		ac2 = false;
	}
}

void HawkElectricSystem::updateAC3(double dt)
{
	if (EngineAPI.getRPM() > 45)
	{
		if (getVolts() > 25)
		{
			ac3 = true;
		}
		else
		{
			ac3 = false;
		}
	}
	else
	{
		ac3 = false;
	}
}

void HawkElectricSystem::simulate(double dt)
{
	// Updates battery voltage
	updateVolts(dt);

	// Updates the AC buses
	updateAC1(dt);
	updateAC2(dt);
	updateAC3(dt);

	// Setting values to the exported parameters
	//electricAPI.setParamNumber(BATT_1, getBatt1());
	//electricAPI.setParamNumber(BATT_2, getBatt2());
	//electricAPI.setParamNumber(AC_1, getAC1());
	//electricAPI.setParamNumber(AC_2, getAC2());
	//electricAPI.setParamNumber(AC_3, getAC3());
	//electricAPI.setParamNumber(VOLTAGE, getVolts());
}
