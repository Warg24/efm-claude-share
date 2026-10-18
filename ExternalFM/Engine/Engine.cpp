#include "Engine.h"
#include "EngineData.h"
#include "../ED_FM_Utility.h"
#include "../UtilityFunctions.h"
#include <iostream>
//#include <../include/Cockpit/cockpit_api_declare.h>

//EDPARAM adourAPI;

//void* ccRPM = adourAPI.getParamHandle("NG_RPM");
//void* ccTGT = adourAPI.getParamHandle("NG_TGT");

void Engine::initCold()
{
	ENGINE_STATE = 0;
}

void Engine::initHot()
{
	ENGINE_STATE = 1;
}

void Engine::initHotInAir()
{
	ENGINE_STATE = 1;
}

void Engine::simulate(double mach, double alt, double dt)
{
	double pi = 4.0 * atan(1.0);
	double thrustN = 0.0;
	double tas = 0.0;
	double altFt = 0.0;
	double thrustEff = 0.0;
	double gasTemp = 0.0;
	double flow = 0.0;

	double row = -1;

	// Thrust table init
	for (int i = 0; i < thrustData.size() - 1; i++)
	{
		if (thrustData[i].eMach <= 0)
		{
			row = i;
		}
	}
	// EGT table init
	for (int i = 0; i < egtData.size() - 1; i++)
	{
		if (egtData[i].eRPM <= 0)
		{
			row = i;
		}
	}
	// FF table init
	for (int i = 0; i < ffData.size() - 1; i++)
	{
		if (ffData[i].eThrottle <= 0)
		{
			row = i;
		}
	}
	// Alt Scalar table init
	for (int i = 0; i < scalarDueToAlt.size() - 1; i++)
	{
		if (scalarDueToAlt[i].eAlt <= 0)
		{
			row = i;
		}
	}
	
	// Thrust table rows
	double thrustCurrentRow = thrustData[row].eMach;
	double thrustNextRow = thrustData[row + 1].eMach;
	// EGT table rows
	double egtCurrentRow = egtData[row].eRPM;
	double egtNextRow = egtData[row + 1].eRPM;
	// FF table rows
	double ffCurrentRow = ffData[row].eThrottle;
	double ffNextRow = ffData[row + 1].eThrottle;
	// Scalar table rows
	double altCurrentRow = scalarDueToAlt[row].eAlt;
	double altNextRow = scalarDueToAlt[row + 1].eAlt;

	// Calculate thrust
	thrustN = linearInterpolation(thrustData[row].eThrust, thrustData[row + 1].eThrust, mach, thrustCurrentRow, thrustNextRow);
	// Calculate TGT
	gasTemp = linearInterpolation(egtData[row].eEGT, egtData[row + 1].eEGT, (getRPM() * 100), egtCurrentRow, egtNextRow);
	// Calculate FF
	flow = linearInterpolation(ffData[row].eFuelFlow, ffData[row + 1].eFuelFlow, getThrottle(), ffCurrentRow, ffNextRow);
	// Calculate thrust efficiency
	thrustEff = (getRPM() * getFuelFlow()) * linearInterpolation(scalarDueToAlt[row].eThrustEff, scalarDueToAlt[row + 1].eThrustEff, alt, altCurrentRow, altNextRow);

	thrust_N = thrustN * thrustEff;
	EGT = (getRPM() * 100) * gasTemp;
	fuelFlow = getThrottle() * flow;

	double rpmTime{};
	double rateOfChange = (RPM - prevRPM) / rpmTime;

	switch (ENGINE_STATE)
	{
	case 0: // ENGINE OFF
		engineRunning = false;
		break;
	case 1: // ENGINE RUNNING
	{
		engineRunning = true;
		double growthRate = 0.04 * 40;
		growthRate = limit(growthRate, -0.04, 0.04);
		double ffRate;
		ffRate = 80 * ((getThrottle()) - throttle);
		ffRate = limit(ffRate, -fuelFlow, fuelFlow);
		double targetRPM= (45 * getThrottle()) + 55;
		double diffRPM = targetRPM - RPM;
		// RPM += growthRate * diffRPM * dt;
		RPM += growthRate * dt;
		RPM = limit(RPM, 0, 100);
		break;
	}
	case 2: // ENGINE STARTING
		// RPM += dt / 8.6 * 30;
		rpmTime = 8.6;
		RPM += rateOfChange * dt;
		RPM = limit(RPM, 0, 30);
		if (getHPCock() == true)
		{
			ENGINE_STATE = 3;
		}
		break;
	case 3: // ENGINE SPOOL UP
		// RPM += dt / 26 * 100;
		rpmTime = 26;
		RPM += rateOfChange * dt;
		RPM = limit(RPM, 0, 55);
		if (RPM >= 55)
		{
			ENGINE_STATE = 1;
		}
		break;
	case 4: // ENGINE SHUTDOWN
		RPM -= dt / 46 * 100;
		RPM = limit(RPM, 0, 100);
		if (RPM <= 0.05)
		{
			ENGINE_STATE = 0;
		}
		break;
	}

	/*adourAPI.setParamNumber(ccRPM, getRPM());
	adourAPI.setParamNumber(ccTGT, getEGT());*/

	std::cout << getRPM() << std::endl;
	std::cout << getEGT() << std::endl;
	std::cout << getFuelFlow() << std::endl;
	std::cout << "---------------------" << std::endl;
}