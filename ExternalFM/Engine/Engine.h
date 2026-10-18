#pragma once

class Engine
{
public:
	int ENGINE_STATE{ 0 };

	bool engineRunning = false;
	bool simInit;

	double thrust_N = 0.0;
	double RPM = 0.0;
	double prevRPM = 0.0;
	double EGT = 0.0;
	double fuelFlow = 0.0;

	bool LPCock = false;
	bool HPShutOff = false;
	bool FuelPump = false;
	bool ignition = false;
	bool engStartIgn = false;
	bool engStart = false;

	double throttle = 0.0;

	Engine()
		: engineRunning(false)
		, simInit(false)
		, thrust_N(0.0)
		, RPM(0.0)
		, EGT(0.0)
		, fuelFlow(0.0)
		, LPCock(false)
		, HPShutOff(false)
		, FuelPump(false)
		, ignition(false)
		, engStartIgn(false)
		, engStart(false)
		, throttle(0.0)
	{}
	~Engine() {}

	void initCold();
	void initHot();
	void initHotInAir();

	void setThrottle(double value) { throttle = value; };
	double getThrottle() { return throttle; };

	double getRPM() { return RPM / 100; };
	double getEGT() { return EGT; };
	double getThrust() { return thrust_N; };
	double getFuelFlow() { return fuelFlow; };

	void setLPCock(double value)
	{
		if (value == 1)
		{
			LPCock = true;
		}
		else
		{
			LPCock = false;
		}
	};
	double getLPCock() { return LPCock; };
	void setHPShutOff(double value)
	{
		if (value == 1)
		{
			HPShutOff = true;
		}
		else
		{
			HPShutOff = false;
		}
	};
	bool getHPCock() { return HPShutOff; };

	void setIgnition(double value)
	{
		if (value == 1)
		{
			ignition = true;
		}
		else
		{
			ignition = false;
		}
	};
	bool getIgnition() { return ignition; };

	void setEngStart(double value)
	{
		if (value == 1)
		{
			engStart = true;
		}
		else
		{
			engStart = false;
		}
	};
	double getEngStart() { return engStart; };

	void setStartIgn(double value)
	{
		if (value == 1)
		{
			engStartIgn = true;
		}
		else
		{
			engStartIgn = false;
		}
	};
	double getEngStartIgn() { return engStartIgn; };

	void simulate(double mach, double alt, double dt);
private:
};
