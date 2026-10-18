// #include <ed/SounderAPI.h>
#include "Adour.h"
//#include <Cockpit/cockpit_api_declare.h>
#include "../UtilityFunctions.h"
#include "EngineData.h"
#include "../ED_FM_Utility.h"
#include <iostream>

//EDPARAM cockpitAPI;

//void* ccNGRPM = cockpitAPI.getParamHandle("NG_RPM");
//void* ccTGT = cockpitAPI.getParamHandle("NG_TGT");
//void* ccSTATE = cockpitAPI.getParamHandle("NG_STATE");

// TEST PARAM STUFF

//void* test_param = cockpitAPI.getParamHandle("TEST_PARAM");
//
//void listen_param(const char* param)
//{
//	cockpitAPI.getParamHandle(param);
//}
//
//double param_handle_test()
//{
//	listen_param("TEST_PARAM");
//}

void Hawk::Adour::initCold()
{
	ENGINE_STATE = 0;
	NG_RPM = 0;
	TGT = 15;
	rotationFlag = 0;
}

void Hawk::Adour::initHot()
{
	ENGINE_STATE = 1;
	NG_RPM = 55;
	TGT = 15;
	rotationFlag = 0;
}

void Hawk::Adour::setCycledRPM(double frameTime)
{
	NG_RPM_100 = NG_RPM * 10;
	NG_RPM_100 = (0.4 * 40) * frameTime;
}

double Hawk::Adour::getCycledRPM()
{
	return NG_RPM_100;
}

double Hawk::Adour::getCoreRelatedRPM()
{
	return NG_RPM / 100;
}

double Hawk::Adour::getRPM()
{
	return NG_RPM / 100;
}

double Hawk::Adour::getPropellerRPM()
{
	return (NG_RPM / 100) * 1217.5;
}

void Hawk::Adour::spoolUp(double frameTime)
{
	static int frame_counter{ 0 };
	frame_counter++;
	double spoolTime = 8.5;
	double spoolRate = (55 - prevRPM) / spoolTime; // (Target Value - Previous RPM Value) / time to spool
	NG_RPM += frameTime / 71 * 55;  //3.4 gives 16.7s run-up, target is 71s
	// NG_RPM += spoolRate * frameTime;
	NG_RPM = limit(NG_RPM, 0, 55);
	if (NG_RPM >= 55)
	{
		ENGINE_STATE = 1;
		printf("Engine start-up complete!\n");
	}
	else if (NG_RPM >= 45)
	{
		ENGINE_STATE = 4;
	}
	else
	{
		if (frame_counter % 20 == 0)
		{
			printf("Engine spooling up.\n");
		}
	}
}

void Hawk::Adour::spoolDown(double frameTime)
{
	double spoolTime = 1.76;
	double spoolRate = (0 - prevRPM) / spoolTime; // (Target Value - Previous RPM Value) / time to spool
	NG_RPM -= frameTime * 3.4;
	// NG_RPM += spoolRate * frameTime;
	NG_RPM = limit(NG_RPM, 0, 100);

	if (NG_RPM < 0.05)
	{
		ENGINE_STATE = 0;
		printf("Engine has stopped!\n");
	}
}

void Hawk::Adour::calcTurbGasTemp(double dt)
{
	double pi = 4.0 * atan(1.0);
	double gasTemp;

	double row = -1;

	// EGT table init
	for (int i = 0; i < egtData.size() - 1; i++)
	{
		if (egtData[i].eRPM <= 0)
		{
			row = i;
		}
	}

	// EGT table rows
	double egtCurrentRow = egtData[row].eRPM;
	double egtNextRow = egtData[row + 1].eRPM;

	// Calculate TGT
	gasTemp = linearInterpolation(egtData[row].eEGT, egtData[row + 1].eEGT, getRPM(), egtCurrentRow, egtNextRow);

	// TGT = gasTemp;

	/*if (NG_RPM <= 30)
	{
		TGT = (NG_RPM * 10) / 4.1;
	}
	else if (NG_RPM >= 35 && NG_RPM < 45)
	{
		TGT = (NG_RPM * 31) / 2.25;
	}
	else if (NG_RPM >= 45 && NG_RPM < 80)
	{
		TGT = (NG_RPM * 40) / 3.0;
	}
	else if (NG_RPM >= 80)
	{
		TGT = (NG_RPM * 55) / 5.5;
	}*/
	TGT = getRPM() * 65;
	TGT = limit(TGT, 0, 800);
}

double Hawk::Adour::getTurbGasTemp()
{
	return TGT * 100;
}

void Hawk::Adour::setPropRotation(const double frameTime)
{
	propPos += (NG_RPM / 100) * frameTime * 7.83;
	if (propPos > 1)
	{
		propPos -= 1;
	}
}

double Hawk::Adour::getPropRotation()
{
	return propPos;
}

void Hawk::Adour::setThrust(const double rpm, double throttle_input)
{
	if (NG_RPM <= 0)
	{
		thrust_N = 0;
	}
	else if (NG_RPM > 50)
	{
		thrust_N = throttle_input * (NG_RPM * 240.0);
		thrust_N = limit(thrust_N, 0, 24000);
	}
}

double Hawk::Adour::getThrustN()
{
	return thrust_N;
}

void Hawk::Adour::update(const double frameTime, const bool isFuelFlow, const double OAT, const double throttle_input, const double rpm, const bool isIdleCutOff, const bool isDCpower)
{
	prevRPM = NG_RPM;

	switch (ENGINE_STATE)
	{
	case 0:
		engineRunning = false;
		break;
	case 1:
	{
		engineRunning = true;
		double growthRate = 0.4 * 40;
		NG_RPM += (growthRate * frameTime);
		NG_RPM = (45 * throttle_input) + 55;
		NG_RPM = limit(NG_RPM, 0.0, 100.0);
		rotationFlag = 0;
		break;
	}
	case 2:
	case 4:
		spoolUp(frameTime);
		if (NG_RPM < 45)
		{
			rotationFlag = 1;
		}
		else if (NG_RPM >= 45)
		{
			rotationFlag = 0;
		}
		break;
	case 3:
		spoolDown(frameTime);
		break;
	}

	std::cout << TGT << std::endl;

	//cockpitAPI.setParamNumber(ccNGRPM, NG_RPM);
	//cockpitAPI.setParamNumber(ccTGT, getTurbGasTemp());
	//cockpitAPI.setParamNumber(ccSTATE, ENGINE_STATE);
	setThrust(rpm, throttle_input);
	setCycledRPM(frameTime);
	setPropRotation(frameTime);
	calcTurbGasTemp(frameTime);
}