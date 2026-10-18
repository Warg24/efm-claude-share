#pragma once
#include <stdio.h>
#include "../ExternalFM_API.h"

namespace Hawk
{
	class Adour
	{
	private:

	public:
		int ENGINE_STATE{ 0 };
		/*	0 = OFF,
			1 = RUNNING,
			2 = STARTING,
			3 = SHUTOFF,
			4 = At 45% RPM - APU Shutdown
		*/
		bool simInit;

		double throttlePos = 0;
		double throttleRate = 0;

		double NG_RPM; // Engine 1 gas producer/core
		double prevRPM;

		double NG_RPM_100;

		double SHP; // Shaft Horsepower

		double TGT; // Turbine Gas Temperature

		double EPSI; // Engine Oil Pressure

		bool isStarterPressed;
		bool engineRunning;
		bool hotStart;
		bool engineFire;

		double TQ; // Engine 1 torque

		double propPos; // Used for PROP_ROTATION animation
		double propBlur; // used for BLUR_STATE animation

		double thrust_N;

		double rotationFlag = 0;

		Adour()
			: simInit(false)
			, throttlePos(0)
			, NG_RPM(0)
			, prevRPM(0)
			, NG_RPM_100(0)
			, SHP(0)
			, TGT(20)
			, EPSI(0)
			, isStarterPressed(false)
			, engineRunning(false)
			, hotStart(false)
			, engineFire(false)
			, TQ(0)
			, propPos(0)
			, propBlur(0)
			, thrust_N(0)
		{}
		~Adour() {}

		// TODO:
		//		- EDPARAM cockpitAPI declare
		//		- Create param_handles for digital displays

		void initCold();

		void initHot();

		double setThrottleInput(double throttle_CMD, double frameTime);

		double getThrottleInput();

		void setStarter(const float value);

		void setCycledRPM(double frameTime);

		double getCycledRPM();

		double getCoreRelatedRPM();

		double getRPM();

		double getPropellerRPM();

		void start(double frameTime);

		void spoolUp(double frameTime);

		void spoolDown(double frameTime);

		void calcTurbGasTemp(double dt);

		double getTurbGasTemp();

		void updateTorque(double frameTime, double throttle_input);

		double getTorque();

		void setPropRotation(const double frameTime);

		double getPropRotation();

		void setPropBlur(const double rpm);

		double getPropBlur();

		void setThrust(const double rpm, double throttle_input);

		double getThrustN();

		void update(const double frameTime, const bool isFuelFlow, const double OAT, const double throttle_input, const double rpm, const bool isIdleCutOff, const bool isDCpower);
	};
}