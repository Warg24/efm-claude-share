#pragma once

#include "../ED_FM_Utility.h"
#include "../Airframe/ConstantData.h"
#include "../ExternalFM_API.h"

// static int fm_debug;

namespace Hawk
{
	class Atmosphere
	{
	private:
	public:
		void update(double temperature, double density, double vt, double* coeff);
	};
}


class AerodyneFM
{
public:
	AerodyneFM() = default;

	void Update(double dt);

	void SetAerodynamicAngles(double aoa, double aos);

	void SetFlightControls(double elevator, double aileron, double rudder);
	void SetFlapPerc(double flapPerc);
	void SetSpeedBrakePerc(double sbPerc);
	void SetGearPerc(double gearPerc);

	void SetAtmosphere(double altitude, double density, double temperature, double pressure, double vSound);
	void SetSurfaceHeight(double surfaceHeight);

	void SetCenterOfGravity(const Vec3& cg);

	void SetSpeeds(double v, double mach);

	double getMachSpeed();

	void SetBodyAxis(double u, double v, double w, double udot, double wdot, double p, double q, double r);

	double GetCockpitOverPressure();

	Vec3 GetForces();
	Vec3 GetMoments();

	// int create_debug_fm();

private:
	double  alpha = 0.0;			// in [rad]
	double	beta = 0.0;				// in [rad]

	double	quota_slm_m = 0.0;		// Altitude above sea level in [m]
	double	densita = 0.0;			// Density in [Kg/m3]
	double	pressione = 0.0;		// Atmospheric pressure [Pa]
	double	overpressure = 0.0;		// Overpressure cockpit [Pa]

	double	altezzaSuolo = 0.0;		// altezza suolo-centro aereo [m] - utile per il WoW e effetto suolo.

	double	def_elev = 0.0;			// Elevator deflection	[rad]
	double	def_aileron = 0.0;		// Aileron deflection	[rad]
	double	def_rudder = 0.0;		// Rudder deflection	[rad]
	
	double	flap_pos = 0.0;
	double	gear_pos = 0.0;
	double	sb_pos = 0.0, sb_posEff;

	Vec3    center_of_gravity;

	double  V_scalar_MS = 0.0;		// in [m/s]
	double  Mach = 0.0;

	double  u_ = 0.0, v_ = 0.0, w_ = 0.0, udot_ = 0.0, wdot_ = 0.0, p_ = 0.0, q_ = 0.0, r_ = 0.0;

	double resultX, resultY, resultZ;

	double  X = 0.0, Y = 0.0, Z = 0.0;
	double	N = 0.0, M = 0.0, L = 0.0;

	/*------------------------------------*/
	/*------------Aircraft Data-----------*/
	/*------------------------------------*/
	double  S = 16.685386;			// [m^2]	Wing area					//OK
	double  W_span = 9.72312;		// [m]		Wing span					//OK
	double  MAC = 1.944624;			// [m]		Mean aerodynamic chord		//OK
	double	AR = 5.67;				// [-]		Wing aspect ration			//OK
	double	e = 0.805;				// [-]		Oswald coefficient			//NOTOK


	/*------------------------------------*/
	/*----------Aerodynamic Data----------*/
	/*------------------------------------*/

	double	CD0 = 0.0181;			// CD at AoA = 0
	double	CDDf = 0.1000;			// CD due to flaps deflection
	double	CDGear = 0.0151;		// CD due to Landing gear
	double	CDsb = 0.02;			// CD due to speed brake
	double	CDbeta = 0.08;			// CD due to Angle of side slip

	double	CYbeta = -0.4464;		// Coefficient of lateral force due to Angle of side slip (the CY is the coefficient of lateral force)
	double	CYDr = 0.1413;			// Coefficient of lateral force due to rudder deflection
	double	CYroll = -0.0425;		// Coefficient of lateral force due to roll
	double	CYyaw = -0.1997;		// Coefficient of lateral force due to yaw

	double	ClBeta = -0.0391;		// Coefficient of roll moment due to beta - also called dihedral effect
	double	ClDa = -0.0833;			// Coefficient of roll due to aileron deflection - it is also called aileron effectiveness - acting on this you can increase/decrease your rate roll -0.1733
	double	ClDr = -0.0029;			// Coefficient of roll due to rudder deflection - the action produced is called "induced roll"
	double	ClRoll = -0.5365;		// Damping coefficient of roll - this acts in opposite way to roll, so it is responsible of roll stopping when you put to the center the stick
	double	ClYaw = -0.0879;		// Damping coefficient of roll due to yaw

	double	CnBeta = 1.1283;		// Coefficient of yaw due to Angle of side slip - it is also called "weathervane effect" or "yaw stiffness". It is the tendency to counter the yaw
	double	CnDr = -0.1200;			// Coefficient of yaw due to rudder deflection - it is also called "rudder effectiveness"
	double	CnDa = -0.0174;			// Coefficient of yaw due to aileron deflection - it is also called "adverse yaw"
	double	CnYaw = -17.29;			// Damping coefficient of yaw
	double	CnRoll = -0.754;		// Damping coefficient of yaw due to roll

	double	CmDf = -0.005;			// Coefficient of pitch moment due to flap deflection
	double	CmGear = -0.006;		// Coefficient of pitch moment due to landing gear
	double	Cmsb = -0.0023;			// Coefficient of pitch moment due to speed brake

	//double	CLalpha = 6.1834;	// Not used in this FM
	double	CLde_flap = 0.2213;		// Lift coefficient due to flaps deflection
	double	CLalphaDot = 1.5137;	// Lift coefficient due to velocity of variation of AoA - Not used, but you can test it
	double	CLq = -5.4668;			// Lift coefficient due to pitch rate - Not used, but you can test it
	double	CLde = -0.5548;			// Lift coefficient due to elevator deflection - negative in this FM
	double	CLsb = -0.0400;			// Lift coefficient due to speed brake - not coded in yet

	double	Cm0 = -0.0;				// Coefficient of pitch moment at AoA = 0
	double	Cmalpha = -1.2537;		// Coefficient of pitch moment due to AoA - Also called "pitch stiffness"
	double	CmalphaDot = -1.8546;	// Coefficient of pitch moment due to the velocity of variation of AoA - negative in this FM
	double	Cmq = -27.6543;			// Coefficient of pitch moment due to pitch rate - it is a pitch damping
	double	Cmde = -0.9451;			// Coefficient of pitch moment due to elevator deflection - also called "elevator effectiveness" - negative in this FM -1.2551
	double	CDalpha = 0.15;			// CD due to AoA - Not used

	// OLD VALUES - BACKUP FOR REFERENCE

	//double	CD0 = 0.0293;			// CD at AoA = 0
	//double	CDDf = 0.0791;			// CD due to flaps deflection
	//double	CDGear = 0.0410;		// CD due to Landing gear
	//double	CDsb = 0.0835;			// CD due to speed brake
	//double	CDbeta = 0.08;			// CD due to Angle of side slip

	//double	CYbeta = -0.0459;		// Coefficient of lateral force due to Angle of side slip (the CY is the coefficient of lateral force)
	//double	CYDr = -0.0063;			// Coefficient of lateral force due to rudder deflection
	//double	CYroll = -0.0489;		// Coefficient of lateral force due to roll
	//double	CYyaw = 0.0000;			// Coefficient of lateral force due to yaw

	//double	ClBeta = 0.0193;		// Coefficient of roll moment due to beta - also called dihedral effect
	//double	ClDa = -0.0483;			// Coefficient of roll due to aileron deflection - it is also called aileron effectiveness - acting on this you can increase/decrease your rate roll
	//double	ClDr = 0.0049;			// Coefficient of roll due to rudder deflection - the action produced is called "induced roll"
	//double	ClRoll = -0.1508;		// Damping coefficient of roll - this acts in opposite way to roll, so it is responsible of roll stopping when you put to the center the stick
	//double	ClYaw = -0.0024;		// Damping coefficient of roll due to yaw

	//double	CnBeta = 0.16;			// Coefficient of yaw due to Angle of side slip - it is also called "weathervane effect" or "yaw stiffness". It is the tendency to counter the yaw
	//double	CnDr = -0.0981;			// Coefficient of yaw due to rudder deflection - it is also called "rudder effectiveness"
	//double	CnDa = -0.0098;			// Coefficient of yaw due to aileron deflection - it is also called "adverse yaw"
	//double	CnYaw = -0.9399;		// Damping coefficient of yaw
	//double	CnRoll = -0.0282;		// Damping coefficient of yaw due to roll
	//
	//double	CmDf = -0.03;			// Coefficient of pitch moment due to flap deflection
	//double	CmGear = 0.002;			// Coefficient of pitch moment due to landing gear
	//double	Cmsb = 0.0008;			// Coefficient of pitch moment due to speed brake

	////double	CLalpha = 6.1834;	// Not used in this FM
	//double	CLde_flap = -0.1800;	// Lift coefficient due to flaps deflection
	//double	CLalphaDot = -9.0938;	// Lift coefficient due to velocity of variation of AoA - Not used, but you can test it
	//double	CLq = -6.0513;			// Lift coefficient due to pitch rate - Not used, but you can test it
	//double	CLde = -0.0010;			// Lift coefficient due to elevator deflection - negative in this FM

	//double	Cm0 = -0.0215;			// Coefficient of pitch moment at AoA = 0
	//double	Cmalpha = -2.9838;		// Coefficient of pitch moment due to AoA - Also called "pitch stiffness"
	//double	CmalphaDot = 0.0688;	// Coefficient of pitch moment due to the velocity of variation of AoA - negative in this FM
	//double	Cmq = -27.2871;			// Coefficient of pitch moment due to pitch rate - it is a pitch damping
	//double	Cmde = -0.9252;			// Coefficient of pitch moment due to elevator deflection - also called "elevator effectiveness" - negative in this FM
	//double	CDalpha = 0.14;			// CD due to AoA - Not used

	//double	Cm0 = -0.0215;			// Coefficient of pitch moment at AoA = 0
	//double	Cmalpha = -2.9838;		// Coefficient of pitch moment due to AoA - Also called "pitch stiffness"
	//double	CmalphaDot = -9.0938;	// Coefficient of pitch moment due to the velocity of variation of AoA - negative in this FM
	//double	Cmq = -42.2871;			// Coefficient of pitch moment due to pitch rate - it is a pitch damping
	//double	Cmde = -1.1522;			// Coefficient of pitch moment due to elevator deflection - also called "elevator effectiveness" - negative in this FM
	//double	CDalpha = 0.14;			// CD due to AoA - Not used

};

