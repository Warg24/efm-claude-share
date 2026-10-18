#pragma once

#include "../ED_FM_Utility.h"

class FuelSystem
{
public:
	double volume;
	double fuel;

	double x;
	double y;
	double z;

	FuelSystem(double _volume = 0, double _fuel = 0)
		: volume(_volume)
		, fuel(_fuel)
		, x(0)
		, y(0)
		, z(0)
	{}
	~FuelSystem() {}

	double addFuel(const double addition);

	double decFuel(const double decrement);

	double getSpace() const;
};

class HawkFuelSystem
{
protected:
	bool is_unlimited_fuel = false;
	double previous_usage = 0;

	FuelSystem LeftMainTank = 313.5;
	FuelSystem RightMainTank = 313.5;
	FuelSystem LeftTipTank = 0;
	FuelSystem RightTipTank = 0;
	FuelSystem Center1Tank = 322.5;
	FuelSystem Center2Tank = 322.5;
	FuelSystem External1Tank = 0;
	FuelSystem External2Tank = 0;

public:
	HawkFuelSystem() = default;
	bool isIdleCutOff = false;

	bool isFuelFlow = false;

	void initCold();

	void initHot();

	void refuelAdd(const double fuel);

	void setInternalFuel(const double fuel);
	
	double getInternalFuel() const;

	bool isLowFuel() const;

	void setIdleCutOff(float value);

	double getIdleCutOff();

	void setUnilimitedFuel(bool status);

	double getUsageSinceLastFrame() const;

	void clearUsageSinceLastFrame();

	void update(const double rpm, const double frameTime);
};