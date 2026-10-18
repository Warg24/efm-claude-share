#pragma once
#include <stdio.h>

class EngineAPU
{
private:

protected:

public:
	int APU_STATE{ 0 };
	/*	0 = OFF,
		1 = RUNNING,
		2 = STARTING,
		3 = SHUTOFF,
	};*/
	int APU_DURATION{ 0 };

	double APU_RPM = 0.0; // APU gas producer/core

	double thrust_N; // APU Thrust [N] Newtons

	double Temp;

	double Pressure;

	bool starterPressed;

	bool shutOffPressed;

	double flag = 0;

	bool APURunning;
	bool hotStart;
	bool APUfire;

	EngineAPU()
		: APU_RPM(0)
		, thrust_N(0)
		, Temp(0)
		, Pressure(0)
		, starterPressed(false)
		, shutOffPressed(false)
		, APURunning(false)
		, hotStart(false)
		, APUfire(false)
	{}
	~EngineAPU() {}

	void initCold();

	void initHot();

	double getCoreRelatedRPM();

	double getRPM();

	double getFlagON();

	double getThrust();

	void spoolUp(double frameTime);

	void spoolDown(double frameTime);

	void update(const double frameTime, int engine_state);
};
