#include "APU.h"
//#include <Cockpit/cockpit_api_declare.h>
// #include <FMEngines/APU.h>
#include "../UtilityFunctions.h"
#include "Adour.h"

//EDPARAM cockpitAPI;
EngineAPU eAPU;
Hawk::Adour RRAdour;

//void* APURPM = cockpitAPI.getParamHandle("APU_RPM");
//void* APUSTATE = cockpitAPI.getParamHandle("APU_STATE");

void EngineAPU::initCold()
{
	APU_RPM = 0;
	thrust_N = 0;
	starterPressed = 0;
	flag = 0;
	APURunning = false;
	hotStart = false;
	APUfire = false;
}

void EngineAPU::initHot()
{
	APU_RPM = 0;
	thrust_N = 0;
	starterPressed = 0;
	flag = 0;
	APURunning = false;
	hotStart = false;
	APUfire = false;
}

double EngineAPU::getCoreRelatedRPM()
{
	return APU_RPM;
	printf("EngineAPU::getCoreRelatedRPM = %f\n", APU_RPM);
}

double EngineAPU::getRPM()
{
	return APU_RPM;
}

double EngineAPU::getFlagON()
{
	if (APU_STATE == 1)
	{
		flag = 1;
	}
	else
	{
		flag = 0;
	}
	return flag;
}

void EngineAPU::spoolUp(double frameTime)
{
	APU_RPM += frameTime / 12; // should be 12 seconds on start up
	APU_RPM = limit(APU_RPM, 0, 1);
	if (APU_RPM == 1) { APU_STATE = 1; }
}

void EngineAPU::spoolDown(double frameTime)
{
	APU_RPM -= frameTime / 17; // should be 17 seconds on shut down
	APU_RPM = limit(APU_RPM, 0, 1);
	if (APU_RPM == 0) { APU_STATE = 0; }
}

double EngineAPU::getThrust()
{
	return APU_RPM;
}

void EngineAPU::update(const double frameTime, const int engine_state)
{
	switch (APU_STATE)
	{
		//apu running
	case 1:
		//increment apu duration timer
		flag = 1;
		APU_DURATION += 1;
		//start apu shutdown on main rotor achieving 45% rpm
		if (engine_state == 4)
		{
			APU_STATE = 3;
			// printf("APU turning off now engine running.\n");
			break;
		}
		//if the apu is running and it has been running for a long time, turn it off
		if (APU_DURATION > 50000) //frame time to turn off apu if engine is not started about five minutes at 50,000
		{
			APU_DURATION = 0;
			APU_STATE = 3;
			// printf("APU turning off owing to lack of use\n");
			spoolDown(frameTime);

			// printf("APU_SHUTDOWN = 1.0");
			break;
		}
		break;
		//turn on apu on button push
	case  2:
		flag = 0;
		//this will gradually spin it up and then set APU_STATE to 1 (RUNNING) when done
		spoolUp(frameTime);
		// printf("APU turning spooling up.\n");
		break;
	case 3:
		flag = 0;
		spoolDown(frameTime);
		// printf("APU spooling down.\n");
		APU_DURATION = 0;
		break;
	}

	// cockpitAPI.setParamNumber(APURPM, APU_RPM);
	//cockpitAPI.setParamNumber(APUSTATE, APU_STATE);
}