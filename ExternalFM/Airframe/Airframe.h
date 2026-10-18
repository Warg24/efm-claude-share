#ifndef __AIRFRAME__
#define __AIRFRAME__

// #include "../ExternalFM_API.h"

namespace Hawk
{
	class Systems
	{
	private:
	public:
		class avIntLights
		{
		private:
		public:
			avIntLights() = default;

			void initCold();

			void initHot();

			void initHotInAir();

			void update(double frameTime);
		};
		class avExtLights
		{
		private:
		public:

			double lightNav = 0.0;

			double lightTaxi = 0.0;

			double lightAntiCol_TOP = 0.0; // -1.0 = RED | 0.0 = OFF | 1.0 = WHITE

			double lightAntiCol_BTM = 0.0; // -1.0 = RED | 0.0 = OFF | 1.0 = WHITE

			avExtLights() = default;

			void initCold();

			void initHot();

			void initHotInAir();

			void setNavLt(double navLt_CMD);

			void setTaxiLt(double taxiLt_CMD);

			void setAntiColTop(double acTop_CMD);

			void setAntiColBtm(double acBtm_CMD);

			double getNavLt();

			double getTaxiLt();

			double getAntiColTop();

			double getAntiColBtm();

			void update(double frameTime);
		};
	};

	class Airframe
	{
	private:

	public:
		Airframe() = default;

		bool simInit = false;

		double L_flapPos_DEG = 0;
		double L_flapRate_DPS = 0;

		double R_flapPos_DEG = 0;
		double R_flapRate_DPS = 0;

		double airbrake_state = 0;
		double airbrake_rate = 0;

		double elevPos_DEG = 0;
		double elevRate_DPS = 0;

		double L_ailPos_DEG = 0;
		double L_ailRate_DPS = 0;

		double R_ailPos_DEG = 0;
		double R_ailRate_DPS = 0;

		double rudPos_DEG = 0;
		double rudRate_DPS = 0;

		double throttlePos = 0;
		double throttleRate = 0;

		double wheelBrakeL = 0;
		double wheelBrakeR = 0;

		double PlaneDRM = 0;

		double setFlapL(double flap_CMD, double frameTime);

		double setFlapR(double flap_CMD, double frameTime);

		double setAirbrake(double airbrake_CMD, double frameTime);

		double setElevator(double pitch_CMD, double pitchTrim_CMD, double frameTime);

		double setAileronL(double roll_CMD, double rollTrim_CMD, double frameTime);

		double setAileronR(double roll_CMD, double rollTrim_CMD, double frameTime);

		double setRudder(double yaw_CMD, double yawTrim_CMD, double frameTime);

		double setThrottleInput(double throttle_CMD, double frameTime);

		double setWheelBrakeL(double command, double dt);

		double setWheelBrakeR(double command, double dt);

		double getFlaps();

		double getAirbrake();

		double getElevator();

		double getAilerons();

		double getRudder();

		double getThrottleInput();

		double getWheelBrakeL();

		double getWheelBrakeR();

		double setDRMState(double DRMState);

		double getDRMState();
	};
}

#endif
