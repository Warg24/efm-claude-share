#include "HydraulicSystem.h"
#include "../Engine/Adour.h"
#include "../UtilityFunctions.h"
#include <iostream>
using namespace std;

Hawk::Adour hydEngAPI;

void Hawk::HydraulicSystem::initCold()
{

}

void Hawk::HydraulicSystem::initHot()
{

}

void Hawk::HydraulicSystem::initHotInAir()
{

}

void Hawk::HydraulicSystem::setHyd2Reset(const float value)
{
	if (value == 1)
	{
		hyd2_reset = true;
	}
	else
	{
		hyd2_reset = false;
	}
}

void Hawk::HydraulicSystem::setHyd1(double dt)
{
	double hyd1Time = 2.4;
	double hyd1Rate;
	if (hydEngAPI.ENGINE_STATE == 3)
	{
		hyd1Rate = (66 - hyd1) / hyd1Time;
		hyd1 += hyd1Rate * dt;
	}
	else if (hydEngAPI.ENGINE_STATE == 4)
	{
		hyd1Rate = (0 - hyd1) / hyd1Time;
		hyd1 += hyd1Rate * dt;
	}
	hyd1 = limit(hyd1, 0, 100.0);
}

void Hawk::HydraulicSystem::setHyd2(double dt)
{
	double hyd2Time = 2.4;
	double hyd2Rate;
	if (hydEngAPI.ENGINE_STATE == 1)
	{
		if (hyd2_reset == true && hyd2_value == 0)
		{
			hyd2Rate = (0 - hyd2) / hyd2Time;
			hyd2 += hyd2Rate * dt;
		}
	}
	else if (hydEngAPI.ENGINE_STATE == 4)
	{
		hyd2Rate = (0 - hyd2) / hyd2Time;
		hyd2 += hyd2Rate * dt;
	}
	hyd2 = limit(hyd2, 0, 100.0);
}

void Hawk::HydraulicSystem::update(double dt)
{
	setHyd1(dt);
	setHyd2(dt);
}