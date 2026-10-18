#include "FuelSystem.h"
#include "../ED_FM_Utility.h"

#include <cmath>
#include <vector>

double FuelSystem::addFuel(const double addition)
{
	double space = volume - fuel;
	if (space < addition)
	{
		fuel = volume;
		return (addition - space);
	}
	fuel += addition;
	return 0;
}

double FuelSystem::decFuel(const double decrement)
{
	if (fuel < decrement)
	{
		double tmp = decrement - fuel;
		fuel = 0;
		return tmp;
	}
	fuel -= decrement;
	return 0;
}

double FuelSystem::getSpace() const
{
	return (volume - fuel);
}

void HawkFuelSystem::initCold()
{
	isIdleCutOff = false;
	isFuelFlow = true;
}

void HawkFuelSystem::initHot()
{
	isIdleCutOff = false;
	isFuelFlow = true;
}

void HawkFuelSystem::refuelAdd(const double fuel)
{
	double addition = fuel;
	addition = LeftMainTank.addFuel(addition);
	addition = RightMainTank.addFuel(addition);
	addition = Center1Tank.addFuel(addition);
	addition = Center2Tank.addFuel(addition);
	addition = LeftTipTank.addFuel(addition);
	addition = RightTipTank.addFuel(addition);
}

void HawkFuelSystem::setInternalFuel(const double fuel)
{
	LeftMainTank.fuel = 0;
	RightMainTank.fuel = 0;
	Center1Tank.fuel = 0;
	Center2Tank.fuel = 0;
	LeftTipTank.fuel = 0;
	RightTipTank.fuel = 0;
	refuelAdd(fuel);
}

double HawkFuelSystem::getInternalFuel() const
{
	return (LeftMainTank.fuel + RightMainTank.fuel + Center1Tank.fuel + Center2Tank.fuel + LeftTipTank.fuel + RightTipTank.fuel);
}

bool HawkFuelSystem::isLowFuel() const
{
	if (getInternalFuel() <= 45)
	{
		return true;
	}
}

void HawkFuelSystem::setIdleCutOff(float value)
{
	if (value == 1)
	{
		isIdleCutOff = true;
		return;
	}
	else if (value == 0)
	{
		isIdleCutOff = false;
		return;
	}
	isIdleCutOff = false;
}

double HawkFuelSystem::getIdleCutOff()
{
	return isIdleCutOff;
}

void HawkFuelSystem::setUnilimitedFuel(bool status)
{
	is_unlimited_fuel = status;
}

double HawkFuelSystem::getUsageSinceLastFrame() const
{
	return previous_usage;
}

void HawkFuelSystem::clearUsageSinceLastFrame()
{
	previous_usage = 0;
}

void HawkFuelSystem::update(const double rpm, const double frameTime)
{
	if (is_unlimited_fuel == true)
	{
		isFuelFlow = true;
		return;
	}

	double fueltmp = rpm * 0.0364 * frameTime;
	previous_usage += fueltmp;

	fueltmp = LeftMainTank.decFuel(fueltmp);
	fueltmp = RightMainTank.decFuel(fueltmp);
	fueltmp = Center1Tank.decFuel(fueltmp);
	fueltmp = Center2Tank.decFuel(fueltmp);
	fueltmp = LeftTipTank.decFuel(fueltmp);
	fueltmp = RightTipTank.decFuel(fueltmp);

	if (getInternalFuel() > 0 && isIdleCutOff == false)
	{
		isFuelFlow = true;
	}
	else
	{
		isFuelFlow = false;
	}
}