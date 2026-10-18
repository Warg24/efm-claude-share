// IMPORTANT FOR DLL TO BUILD
#include "stdafx.h"
#include <cmath>
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <format>
// IMPORTANT FOR EFM API CALLS
#include "ExternalFM.h"
#include "ED_FM_Utility.h"
#include "ExternalFM_API.h"
#include "UtilityFunctions.h"
//#include <Cockpit/cockpit_api_declare.h>
// IMPORTANT FOR INPUTS AND CERTAIN FM DATA
#include "Commands.h"
#include "Airframe/ConstantData.h"
// IMPORTANT FOR SYSTEMS, AERODYNAMICS & POWERPLANT
#include "Aerodynamics/AerodyneFM.h" // Aerodynamic Coefficients
#include "Actuators/Actuators.h" // Landing Gear, Airbrake
#include "Airframe/Airframe.h" // Flight Controls, Lights & Other surfaces
#include "Engine/FuelSystem.h" // Fuel System
#include "Engine/APU.h" // Gas Turbine Starter (GTS)
#include "Engine/Adour.h" // PowerPlant
#include "Suspension/Suspension.h" // Suspension and wheels
// #include "Hydraulics/HydraulicSystem.h" // Hydraulic System

//static EDPARAM cockpitAPI;

Vec3	common_moment;
Vec3	common_force;
Vec3    center_of_gravity;
Vec3	wind;
Vec3	velocity_world_cs;
double  throttle = 0;
double  stick_roll = 0;
double  stick_pitch = 0;
double	yaw = 0;
double	gear_CMD = 0;
double	airbrake_CMD = 0;
double	canopy_CMD = 0.0;
double	brake_CMD = 0;
double	flap_CMD = 0;
double	nws_CMD = 0;
double	wheelB_L = 0;
double	wheelB_R = 0;
double	controlLock = 0;

double	steerOption = 0;

double thrust_N = 0;

bool	NWS = false;
double	gain_NWS = 0;

double	rolling_friction_L = 0.025;
double	rolling_friction_R = 0.025;

double  internal_fuel = 0;
double  fuel_consumption_since_last_time = 0;
double  atmosphere_density = 0;
double  aoa = 0, aos = 0;
double  vSuono = 0.0;			// in [m/s]
double	Temperatura = 0.0;		// temperatura in [K]
double	densita = 0.0;			// densit� in [Kg/m3]
double	altitude_FT = 0.0;		// quota sul livello del mare in [m]
double	pressione_Pa = 0.0;		// pressione aria in [Pa]

double	mach = 0.0; // Air speed in Mach
double	g_accel = 0.0; // G Acceleration

double	v_scalar = 0.0;

double	totalVelocity_FPS = 0.0;
double	dynamicPressure_LBFT2 = 0.0;
double	ambientTemperature_DegK = 0.0;
double	ambientDensity_KgPerM3 = 0.0;
double	meterToFoot = 3.28084;

double	deltaTrimPitchUp = 0.0;
double	deltaTrimPitchDown = 0.0;
double	deltaTrimRoll = 0.0;
double	deltaTrimYaw = 0.0;
double	def_TrimPitch = 0.0;
double	def_TrimRoll = 0.0;
double	def_TrimYaw = 0.0;
bool	def_TrimStop = true;

bool	Trim_reset = false;
bool	AntiSkid = false;

/*------------------------------------*/
/*-----------Altre variabili----------*/
/*------------------------------------*/
int 	i, j;
double	pi = 4.0 * atan(1.0);		// Pi (3.14159....)
double	elevator_PCT = 0.0;
double	aileron_PCT = 0.0;
double	rudder_PCT = 0.0;
double	m;						// massa in [kg]
double  g = 9.806;				// accelerazione di gravit� [m/s^2]
double	outside_air_temp = 0;

double	theta = 0.0, phi = 0.0, psi = 0.0;
double  u = 0.0, v = 0.0, w = 0.0, udot = 0.0, wdot = 0.0, p = 0.0, q = 0.0, r = 0.0, pDot = 0.0, rDot = 0.0;

int ForceReturnCounter = 0;

AerodyneFM FlightModel;
Actuators Controllers;
Hawk::Airframe FlightControls;
HawkFuelSystem Fuel;
EngineAPU GTS;
Hawk::Adour RRAdour;
Hawk::Atmosphere Atmosphere;
LandingGear Suspension;
// Hawk::HydraulicSystem Hydraulics;

#ifdef FSRA_VRA_DATA
// Alternate local build: VRA Hawk aerodynamics and flight controls (see VRA/VraAero.h)
#include "VRA/VraAero.h"
vra::Aero VraAeroModel;
vra::Controls VraControls;
vra::Options VraOptions;
double surfaceHeight_M = 0.0;
#endif

double deltaTime;
static float elapsedTime;

double flapPos = 0.0f;

void allTrimReset(const double dt);
void allTrimReset(const double dt)
{
	if (def_TrimPitch > 0.0)
	{
		def_TrimPitch -= 1.25 * dt;
		if (def_TrimPitch < 0.0)
		{
			def_TrimPitch = 0.0;

		}
	}
	else
	{
		def_TrimPitch += 1.25 * dt;
		if (def_TrimPitch > 0.0)
		{
			def_TrimPitch = 0.0;
		}
	}

	if (def_TrimRoll > 0.0)
	{
		def_TrimRoll -= 1.25 * dt;
		if (def_TrimRoll < 0.0)
		{
			def_TrimRoll = 0.0;
		}
	}
	else
	{
		def_TrimRoll += 1.25 * dt;
		if (def_TrimRoll > 0.0)
		{
			def_TrimRoll = 0.0;
		}
	}

	if (def_TrimYaw > 0.0)
	{
		def_TrimYaw -= 1.25 * dt;
		if (def_TrimYaw < 0.0)
		{
			def_TrimYaw = 0.0;
		}
	}
	else
	{
		def_TrimYaw += 1.25 * dt;
		if (def_TrimYaw > 0.0)
		{
			def_TrimYaw = 0.0;
		}
	}

	if (def_TrimPitch == 0.0 && def_TrimRoll == 0.0 && def_TrimYaw == 0.0)
	{
		Trim_reset = false;
	}
}

void init(const char* config)
{
	
}

void cleanup()
{
	
}

void ed_fm_set_plugin_data_install_path(const char* installpath)
{
	
}


inline void sum_vec3(Vec3& dest, const Vec3& src)
{
	dest.x += src.x;
	dest.y += src.y;
	dest.z += src.z;
}

void add_local_moment(const Vec3& Moment)
{
	sum_vec3(common_moment, Moment);
}


void add_local_force(const Vec3& Force, const Vec3& Force_pos)
{
	common_force.x += Force.x;
	common_force.y += Force.y;
	common_force.z += Force.z;

	Vec3 delta_pos(Force_pos.x - center_of_gravity.x,
		Force_pos.y - center_of_gravity.y,
		Force_pos.z - center_of_gravity.z);

	Vec3 delta_moment = cross(delta_pos, Force);

	common_moment.x += delta_moment.x;
	common_moment.y += delta_moment.y;
	common_moment.z += delta_moment.z;
}


void simulate_fuel_consumption(double dt)
{
	fuel_consumption_since_last_time = 10 * throttle * dt; //10 kg persecond
	if (fuel_consumption_since_last_time > internal_fuel)
		fuel_consumption_since_last_time = internal_fuel;
	internal_fuel -= fuel_consumption_since_last_time;
}

void ed_fm_add_local_force(double& x, double& y, double& z, double& pos_x, double& pos_y, double& pos_z)
{
	x = common_force.x;
	y = common_force.y;
	z = common_force.z;
	pos_x = center_of_gravity.x;
	pos_y = center_of_gravity.y;
	pos_z = center_of_gravity.z;
}

void ed_fm_add_global_force(double& x, double& y, double& z, double& pos_x, double& pos_y, double& pos_z)
{

}

void ed_fm_add_global_moment(double& x, double& y, double& z)
{

}

void ed_fm_add_local_moment(double& x, double& y, double& z)
{
	x = common_moment.x;
	y = common_moment.y;
	z = common_moment.z;
}

void ed_fm_simulate(double dt)
{
	deltaTime = dt;

	if (def_TrimStop == true)
	{
		elapsedTime = 0.0f;
	}
	else if (def_TrimStop == false)
	{
		elapsedTime += deltaTime;
	}

	ForceReturnCounter = 0;
	// Very important! clear out the forces and moments before you start calculated
	// a new set for this run frame
	common_force = Vec3();
	common_moment = Vec3();

	Vec3 airspeed;

	airspeed.x = velocity_world_cs.x - wind.x;
	airspeed.y = velocity_world_cs.y - wind.y;
	airspeed.z = velocity_world_cs.z - wind.z;

	Fuel.update(RRAdour.getRPM(), dt);

	// double V_scalar = sqrt(airspeed.x * airspeed.x + airspeed.y * airspeed.y + airspeed.z * airspeed.z);

	// double Mach = V_scalar / vSuono;

	totalVelocity_FPS = sqrt(airspeed.x * airspeed.x + airspeed.y * airspeed.y + airspeed.z * airspeed.z);
	if (totalVelocity_FPS < 0.01)
	{
		totalVelocity_FPS = 0.01;
	}

	double* temp;
	temp = (double*)malloc(9 * sizeof(double));
	Atmosphere.update(outside_air_temp, densita, totalVelocity_FPS, temp);
	pressione_Pa = temp[0];
	mach = temp[1];

	FlightModel.SetSpeeds(totalVelocity_FPS, mach);

	//only run gts update if the engine is not running
	if (RRAdour.ENGINE_STATE != 1)
	{
		GTS.update(dt, RRAdour.ENGINE_STATE);
	}

	FlightControls.setThrottleInput(throttle, dt);
	RRAdour.calcTurbGasTemp(outside_air_temp);
	RRAdour.update(dt, Fuel.isFuelFlow, outside_air_temp, FlightControls.getThrottleInput(), RRAdour.NG_RPM, Fuel.isIdleCutOff, true);
	// Hydraulics.update(dt);

	// Stops the airbrake from being deployed if the gear are down
	if (gear_CMD > 0)
	{
		if (airbrake_CMD > 0)
		{
			airbrake_CMD = 0;
		}
	}

	FlightControls.setAirbrake(airbrake_CMD, dt);
	FlightControls.setElevator(stick_pitch, def_TrimPitch, dt);
	FlightControls.setAileronL(stick_roll, def_TrimRoll, dt);
	FlightControls.setAileronR(-stick_roll, def_TrimRoll, dt);
	FlightControls.setRudder(yaw, def_TrimYaw, dt);
	FlightControls.setFlapL(flap_CMD, dt);
	FlightControls.setFlapR(flap_CMD, dt);
	FlightControls.setWheelBrakeL(wheelB_L, dt);
	FlightControls.setWheelBrakeR(wheelB_R, dt);

	FlightModel.SetFlightControls(FlightControls.getElevator(), FlightControls.getAilerons(), FlightControls.getRudder());
	FlightModel.SetFlapPerc(FlightControls.getFlaps()); // <- put here the flap_pos
	FlightModel.SetSpeedBrakePerc(FlightControls.getAirbrake()); // <- put here the sb_pos
	double gearPerc = Controllers.gear_actuator(gear_CMD, dt);
	FlightModel.SetGearPerc(gearPerc); // <- put here the gear_pos

	Controllers.simulate(dt);

	// trim reset
	if (Trim_reset == true)
	{
		allTrimReset(dt);
	}

	FlightModel.Update(dt);

#ifdef FSRA_VRA_DATA
	{
		// FSRA keeps u,w,udot,wdot,p,q,r in its own axes; convert back to DCS body axes.
		vra::AeroInput in;
		in.vx = u;
		in.vy = -w;
		in.ax = udot;
		in.ay = -wdot;
		in.omega_x = p;
		in.omega_y = -r;
		in.omega_z = q;
		in.aoa = aoa;
		in.aos = aos;
		in.pitch = theta;
		in.roll = phi;
		in.V = totalVelocity_FPS; // airspeed in m/s despite the name
		in.rho = densita;
		in.a = vSuono;
		in.h_agl_ft = altitude_FT - surfaceHeight_M * meterToFoot - 5.0;
		in.mass = m;
		in.flaps = FlightControls.getFlaps();
		in.airbrake = FlightControls.getAirbrake();
		in.gear = gearPerc;

		// FSRA stores negated stick axes; VRA expects the raw DCS values
		VraControls.setStick(-stick_pitch, -stick_roll, -yaw);
		const double vraMach = vSuono > 0.0 ? totalVelocity_FPS / vSuono : 0.0;
		const vra::AeroOutput out = VraAeroModel.compute(in, VraControls.compute(vraMach), VraOptions);

		add_local_force(Vec3(out.force.x, out.force.y, out.force.z),
			Vec3(center_of_gravity.x, center_of_gravity.y, 0.0));
		add_local_moment(Vec3(out.moment.x, out.moment.y, out.moment.z));
	}
#else
	auto result = FlightModel.GetForces();

	auto X = result.x;
	auto Y = result.y;
	auto Z = result.z;

	Vec3 aerodynamic_force1(X, 0.0, 0.0);
	Vec3 aerodynamic_force_pos1(center_of_gravity.x, center_of_gravity.y, 0);
	add_local_force(aerodynamic_force1, aerodynamic_force_pos1);

	Vec3 aerodynamic_force2(0.0, Z, 0.0);
	Vec3 aerodynamic_force_pos2(center_of_gravity.x, center_of_gravity.y, 0.0);
	add_local_force(aerodynamic_force2, aerodynamic_force_pos2);

	Vec3 aerodynamic_force3(0.0, 0.0, Y);
	Vec3 aerodynamic_force_pos3(center_of_gravity.x, center_of_gravity.y, 0.0);
	add_local_force(aerodynamic_force3, aerodynamic_force_pos3);

	result = FlightModel.GetMoments();

	auto L = result.x;
	auto M = result.y;
	auto N = result.z;

	Vec3 roll_moment(L, 0.0, 0.0);
	add_local_moment(roll_moment);

	Vec3 yaw_moment(0.0, -N, 0.0);
	add_local_moment(yaw_moment);

	Vec3 pitch_moment(0.0, 0.0, M);
	add_local_moment(pitch_moment);
#endif

	// TODO activate this later
	//simulate_fuel_consumption(dt);

	Fuel.clearUsageSinceLastFrame();

	Controllers.simInit = true;
	FlightControls.simInit = true;
	RRAdour.simInit = true;
}

void ed_fm_set_surface(double		h,//surface height under the center of aircraft
	double		h_obj,//surface height with objects
	unsigned	surface_type,
	double		normal_x,//components of normal vector to surface
	double		normal_y,//components of normal vector to surface
	double		normal_z//components of normal vector to surface
)
{
#ifdef FSRA_VRA_DATA
	surfaceHeight_M = h;
#endif
	FlightModel.SetSurfaceHeight(h);	// utilizzata per calcolo effetto suolo. � l'altezza del terreno rispetto al livello del mare

}

void ed_fm_set_atmosphere(double h,//altitude above sea level
	double t,//current atmosphere temperature , Kelwins
	double a,//speed of sound
	double ro,// atmosphere density
	double p,// atmosphere pressure
	double wind_vx,//components of velocity vector, including turbulence in world coordinate system
	double wind_vy,//components of velocity vector, including turbulence in world coordinate system
	double wind_vz //components of velocity vector, including turbulence in world coordinate system
)
{
	/*wind.x = wind_vx;
	wind.y = wind_vy;
	wind.z = wind_vz;*/
	FlightModel.SetAtmosphere(h, ro, t, p, a);

	outside_air_temp = t; // ambientTemperature_DegK
	altitude_FT = h * 3.28084; // M to Ft conversion
	densita = ro; // ambientDensity_KgPerM3
	pressione_Pa = p; // ps_LBFT2
	vSuono = a;
}
/*
called before simulation to set up your environment for the next step
*/
void ed_fm_set_current_mass_state(double mass,
	double center_of_mass_x,
	double center_of_mass_y,
	double center_of_mass_z,
	double moment_of_inertia_x,
	double moment_of_inertia_y,
	double moment_of_inertia_z
)
{
	center_of_gravity.x = center_of_mass_x;
	center_of_gravity.y = center_of_mass_y;
	center_of_gravity.z = center_of_mass_z;
	m = mass;			// in [kg]

	FlightModel.SetCenterOfGravity(center_of_gravity);
}
/*
called before simulation to set up your environment for the next step
*/
void ed_fm_set_current_state(double ax,//linear acceleration component in world coordinate system
	double ay,//linear acceleration component in world coordinate system
	double az,//linear acceleration component in world coordinate system
	double vx,//linear velocity component in world coordinate system
	double vy,//linear velocity component in world coordinate system
	double vz,//linear velocity component in world coordinate system
	double px,//center of the body position in world coordinate system
	double py,//center of the body position in world coordinate system
	double pz,//center of the body position in world coordinate system
	double omegadotx,//angular accelearation components in world coordinate system
	double omegadoty,//angular accelearation components in world coordinate system
	double omegadotz,//angular accelearation components in world coordinate system
	double omegax,//angular velocity components in world coordinate system
	double omegay,//angular velocity components in world coordinate system
	double omegaz,//angular velocity components in world coordinate system
	double quaternion_x,//orientation quaternion components in world coordinate system
	double quaternion_y,//orientation quaternion components in world coordinate system
	double quaternion_z,//orientation quaternion components in world coordinate system
	double quaternion_w //orientation quaternion components in world coordinate system
)
{
}


void ed_fm_set_current_state_body_axis(double ax,//linear acceleration component in body coordinate system
	double ay,//linear acceleration component in body coordinate system
	double az,//linear acceleration component in body coordinate system
	double vx,//linear velocity component in body coordinate system
	double vy,//linear velocity component in body coordinate system
	double vz,//linear velocity component in body coordinate system
	double wind_vx,//wind linear velocity component in body coordinate system
	double wind_vy,//wind linear velocity component in body coordinate system
	double wind_vz,//wind linear velocity component in body coordinate system

	double omegadotx,//angular accelearation components in body coordinate system
	double omegadoty,//angular accelearation components in body coordinate system
	double omegadotz,//angular accelearation components in body coordinate system
	double omegax,//angular velocity components in body coordinate system
	double omegay,//angular velocity components in body coordinate system
	double omegaz,//angular velocity components in body coordinate system
	double yaw,  //radians
	double pitch,//radians
	double roll, //radians
	double common_angle_of_attack, //AoA radians
	double common_angle_of_slide   //AoS radians
)
{
	wind.x = wind_vx;
	wind.y = wind_vy;
	wind.z = wind_vz;

	//uso questi poich� mi serve per calcolare l'airspeed
	velocity_world_cs.x = vx;
	velocity_world_cs.y = vy;
	velocity_world_cs.z = vz;

	// prendo dal simulatore angolo di attacco e di sideslip
	FlightModel.SetAerodynamicAngles(common_angle_of_attack, common_angle_of_slide);
	aoa = common_angle_of_attack;
	aos = common_angle_of_slide;

	// prendo assetto, velocit� e accelerazioni lineari, velocit� angolari
	FlightModel.SetBodyAxis(vx, vz, -vy, ax, -ay, omegax, omegaz, -omegay);

	// prendo accelerazione lungo X per il calcolo dell'attrito
	//groundModel.SetLinearAcceleration(ax);

	theta = pitch;
	phi = roll;
	psi = -yaw;

	u = vx;
	v = vz;
	w = -vy;
	udot = ax;
	wdot = -ay;

	g_accel = ay;

	p = omegax;
	q = omegaz;
	r = -omegay;
}
/*
input handling
*/
void ed_fm_set_command(const int command,
	const float value)
{
	float trimPitchRate = 0.3f;
	float trimRollRate = 0.3f;
	float trimYawRate = 0.3f;

#ifdef FSRA_VRA_DATA
	// VRA trim (same command IDs); FSRA's own trim still drives the surface animations
	VraControls.onCommand(command);
	if (command == trimReset)
	{
		VraControls.resetTrim();
	}
#endif

	switch (command)
	{
	case ControlLock:
		controlLock = value;
		break;
		// analog input (axis)
	case JoystickThrottle:
		// Adour.setThrottle((-value + 1.0) / 2.0);
		throttle = (-value + 1.0) / 2.0;
		break;
		// analogic input (axis)
	case JoystickPitch:
		if (controlLock == 0)
		{
			stick_pitch = -value;
		}
		else
		{
			stick_pitch = 0;
		}
		break;
	case JoystickRoll:
		if (controlLock == 0)
		{
			stick_roll = -value;
		}
		else
		{
			stick_roll = 0;
		}
		break;
		// analogic input (axis)	
	case JoystickYaw:
		yaw = -value;
		break;
	case trimUp:
		def_TrimStop = false;
		deltaTrimPitchUp = (exp(trimPitchRate * elapsedTime) - 1.0f) * deltaTime; // sembra che il valore sia invertito
		def_TrimPitch += deltaTrimPitchUp;
		def_TrimPitch = limit(def_TrimPitch, -7.0, 7.0);
		break;
	case trimDown:
		def_TrimStop = false;
		deltaTrimPitchDown = (exp(trimPitchRate * elapsedTime) - 1.0f) * deltaTime; // sembra che il valore sia invertito
		def_TrimPitch -= deltaTrimPitchDown;
		def_TrimPitch = limit(def_TrimPitch, -7.0, 7.0);
		break;
	case trimLeft:
		deltaTrimRoll = exp(trimRollRate * deltaTime) - 1.0f;
		def_TrimRoll -= deltaTrimRoll;
		def_TrimRoll = limit(def_TrimRoll, -7.0, 7.0);
		break;
	case trimRight:
		deltaTrimRoll = exp(trimRollRate * deltaTime) - 1.0f;
		def_TrimRoll += deltaTrimRoll;
		def_TrimRoll = limit(def_TrimRoll, -7.0, 7.0);
		break;
	case trimLeftRudder:
		deltaTrimYaw = exp(trimYawRate * deltaTime) - 1.0f;
		def_TrimYaw -= deltaTrimYaw;
		def_TrimYaw = limit(def_TrimYaw, -7.0, 7.0);
		break;
	case trimRightRudder:
		deltaTrimYaw = -exp(trimYawRate * deltaTime) - 1.0f;
		def_TrimYaw += deltaTrimYaw;
		def_TrimYaw = limit(def_TrimYaw, -7.0, 7.0);
		break;
	case trimReset:
		Trim_reset = true;
		// printf("Trim reset pressed!\n");
		break;
	case trimStop:
		def_TrimStop = true;
		break;
	case airbrakeTOGGLE:
		if (airbrake_CMD == 1)
		{
			airbrake_CMD = 0.0;
		}
		else if (airbrake_CMD == 0)
		{
			airbrake_CMD = 1.0;
		}
		break;
	case airbrakeEXTEND:
		airbrake_CMD = 1.0;
		if (gear_CMD > 0)
		{
			airbrake_CMD = 0.0;
		}
		break;
	case airbrakeRETRACT:
		airbrake_CMD = 0.0;
		break;

	case LandingGearToggle:
		if (gear_CMD == 1.0)
		{
			if (Suspension.wheelLeft.strutCompression > 0 || Suspension.wheelRight.strutCompression > 0)
			{
				gear_CMD = 1.0;
			}
			else
			{
				gear_CMD = 0.0;
			}
		}
		else if (gear_CMD == 0.0)
		{
			gear_CMD = 1.0;
		}
		break;
	case LandingGearDown:
		gear_CMD = 1.0;
		break;
	case LandingGearUp:
		if (Suspension.wheelLeft.strutCompression > 0 || Suspension.wheelLeft.strutCompression > 0)
		{
			gear_CMD = 1.0;
		}
		else
		{
			gear_CMD = 0.0;
		}
		break;

	case WheelBrakeOn:
		wheelB_L = 1;
		wheelB_R = 1;
		break;
	case WheelBrakeOff:
		wheelB_L = 0;
		wheelB_R = 0;
		break;

	case WheelBrakeOn_L:
		wheelB_L = 1;
		break;
	case WheelBrakeOff_L:
		wheelB_L = 0;
		break;

	case WheelBrakeOn_R:
		wheelB_R = 1;
		break;
	case WheelBrakeOff_R:
		wheelB_R = 0;
		break;

	case WheelBrake_L:
		wheelB_L = value;
		break;

	case WheelBrake_R:
		wheelB_R = value;
		break;

	case flapsDOWN:
		if (flap_CMD < 0.5)
		{
			flap_CMD = 0.5;
		}
		else if (flap_CMD <= 0.5)
		{
			flap_CMD = 1.0;
		}
		break;
	case flapsUP:
		if (flap_CMD <= 0.5)
		{
			flap_CMD = 0.0;
		}
		else if (flap_CMD <= 1.0)
		{
			flap_CMD = 0.5;
		}
		break;

	case canopyToggle:
		if (Controllers.canopyCMD == 1.0)
		{
			Controllers.canopyCMD = 0.0;
		}
		else if (Controllers.canopyCMD == 0.0)
		{
			Controllers.canopyCMD = 1.0;
		}
		break;

	case Eject:
		Controllers.canopyPos = 1;
		break;

	case GTS_PUSH:
		if (GTS.APU_STATE == 0)
		{
			GTS.APU_STATE = 2;
		}
		break;

	case engineStart:
		if (RRAdour.ENGINE_STATE == 0)
		{ 
			RRAdour.ENGINE_STATE = 2; 
		}
		break;
	case engineStop:
		if (RRAdour.ENGINE_STATE == 1)
		{ 
			RRAdour.ENGINE_STATE = 3; 
		}
		break;

	case iCommandPlaneLGPAntiSkid:
		if (value == 1)
		{
			AntiSkid = true;
		}
		else
		{
			AntiSkid = false;
		}
		break;

	default:
	{
	}
	break;
	}
}
/*
	Mass handling

	will be called  after ed_fm_simulate :
	you should collect mass changes in ed_fm_simulate

	double delta_mass = 0;
	double x = 0;
	double y = 0;
	double z = 0;
	double piece_of_mass_MOI_x = 0;
	double piece_of_mass_MOI_y = 0;
	double piece_of_mass_MOI_z = 0;

	//
	while (ed_fm_change_mass(delta_mass,x,y,z,piece_of_mass_MOI_x,piece_of_mass_MOI_y,piece_of_mass_MOI_z))
	{
	//internal DCS calculations for changing mass, center of gravity,  and moments of inertia
	}
*/
bool ed_fm_change_mass(double& delta_mass,
	double& delta_mass_pos_x,
	double& delta_mass_pos_y,
	double& delta_mass_pos_z,
	double& delta_mass_moment_of_inertia_x,
	double& delta_mass_moment_of_inertia_y,
	double& delta_mass_moment_of_inertia_z
)
{
	if (fuel_consumption_since_last_time > 0)
	{
		delta_mass = fuel_consumption_since_last_time;
		delta_mass_pos_x = -1.0;
		delta_mass_pos_y = 1.0;
		delta_mass_pos_z = 0;

		delta_mass_moment_of_inertia_x = 0;
		delta_mass_moment_of_inertia_y = 0;
		delta_mass_moment_of_inertia_z = 0;

		fuel_consumption_since_last_time = 0; // set it 0 to avoid infinite loop, because it called in cycle 
		// better to use stack like structure for mass changing 
		return true;
	}
	else
	{
		return false;
	}
}
/*
	set internal fuel volume , init function, called on object creation and for refueling ,
	you should distribute it inside at different fuel tanks
*/
void   ed_fm_set_internal_fuel(double fuel)
{
	Fuel.setInternalFuel(fuel);
}
/*
	get internal fuel volume
*/
double ed_fm_get_internal_fuel()
{
	return Fuel.getInternalFuel();
}
/*
	set external fuel volume for each payload station , called for weapon init and on reload
*/
void  ed_fm_set_external_fuel(int	 station,
	double fuel,
	double x,
	double y,
	double z)
{

}
/*
	get external fuel volume
*/
double ed_fm_get_external_fuel()
{
	return 0;
}

void ed_fm_refueling_add_fuel(double fuel)
{
	return Fuel.refuelAdd(fuel);
}

void ed_fm_set_draw_args_v2(float* drawargs, size_t size)
{
	// Flaperons
	drawargs[9] = FlightControls.getFlaps();
	drawargs[10] = FlightControls.getFlaps();

	// Ailerons
	drawargs[11] =  -FlightControls.getAilerons();
	drawargs[12] = FlightControls.getAilerons();

	// Elevons
	drawargs[15] = -FlightControls.getElevator();
	drawargs[16] = -FlightControls.getElevator();

	// Rudder
	drawargs[17] = FlightControls.getRudder();

	// Airbrakes
	drawargs[21] = FlightControls.getAirbrake();

	// Canopy
	drawargs[38] = Controllers.getCanopy();
}

void ed_fm_set_fc3_cockpit_draw_args_v2(float* drawargs, size_t size)
{
	drawargs[181] = Controllers.getCanopy();
	drawargs[200] = throttle;
	drawargs[607] = FlightControls.getFlaps();

	drawargs[100] = -FlightControls.getElevator();

	drawargs[305] = RRAdour.rotationFlag;
	drawargs[304] = GTS.flag;
	drawargs[301] = RRAdour.getRPM();

	drawargs[524] = FlightControls.getWheelBrakeL();
	drawargs[523] = FlightControls.getWheelBrakeR();

	drawargs[600] = -stick_pitch;
	drawargs[601] = -stick_roll;
	drawargs[602] = -yaw;
}

void ed_fm_configure(const char* cfg_path)
{

}

double ed_fm_get_param(unsigned param_enum)
{
	switch (param_enum)
	{
	case ED_FM_ENGINE_0_RPM:
		return GTS.getRPM();
	case ED_FM_ENGINE_0_CORE_RELATED_RPM:
		return GTS.getCoreRelatedRPM();
	case ED_FM_ENGINE_0_THRUST:
		return GTS.getThrust();
	case ED_FM_ENGINE_0_RELATED_THRUST:
		return GTS.getThrust();

	case ED_FM_ENGINE_1_RPM:
	case ED_FM_ENGINE_1_RELATED_RPM:
	case ED_FM_ENGINE_1_CORE_RPM:
	case ED_FM_ENGINE_1_CORE_RELATED_RPM:
		return RRAdour.getRPM();
	case ED_FM_ENGINE_1_THRUST:
	case ED_FM_ENGINE_1_CORE_RELATED_THRUST:
		return RRAdour.getThrustN();

	case ED_FM_FUEL_TOTAL_FUEL:
		return Fuel.getInternalFuel();
	case ED_FM_FUEL_LOW_SIGNAL:
		return Fuel.isLowFuel();

	case ED_FM_SUSPENSION_0_GEAR_POST_STATE:
		return Controllers.gear_state;
	case ED_FM_SUSPENSION_0_RELATIVE_BRAKE_MOMENT:
		return 0.001;


	case ED_FM_SUSPENSION_0_WHEEL_SELF_ATTITUDE:
		return 1.0;

	case ED_FM_SUSPENSION_1_GEAR_POST_STATE:
		return Controllers.gear_state;
	case ED_FM_SUSPENSION_1_RELATIVE_BRAKE_MOMENT:
		return wheelB_L * 0.5;//limit(wheelB_L + brake_CMD, 0.0, 1.0);
	case ED_FM_SUSPENSION_2_GEAR_POST_STATE:
		return Controllers.gear_state;
	case ED_FM_SUSPENSION_2_RELATIVE_BRAKE_MOMENT:
		return wheelB_R * 0.5;//limit(wheelB_R + brake_CMD, 0.0, 1.0);


	case ED_FM_ANTI_SKID_ENABLE:
		return AntiSkid;

	case ED_FM_COCKPIT_PRESSURIZATION_OVER_EXTERNAL:
		// additional pressure from pressurization system , pascal , actual cabin pressure will be AtmoPressure + COCKPIT_PRESSURIZATION_OVER_EXTERNAL
		return FlightModel.GetCockpitOverPressure();
	default:
		// silence compiler warning(s)
		break;
	}

	return 0;

}

double ed_fm_get_shake_amplitude()
{
	if (g_accel > 45.0) // at 4.5Gs the cockpit will "shake"
	{
		return (g_accel / 150);
	}

	if (FlightControls.getAirbrake() > 0.2 && FlightModel.getMachSpeed() >= 0.1)
	{
		return FlightControls.getAirbrake() * (FlightModel.getMachSpeed());
	}

	if (FlightControls.getFlaps() > 0.2 && FlightModel.getMachSpeed() >= 0.1)
	{
		return FlightControls.getFlaps() * (FlightModel.getMachSpeed());
	}

	if (aoa > 10 && FlightModel.getMachSpeed() >= 0.1)
	{
		return aoa;
	}

	if (Suspension.wheelNose.strutCompression > 0)
	{
		return Suspension.wheelNose.wheelSpeed * 0.002;
	}

	else
	{
		return 0;
	};
}

void ed_fm_suspension_feedback(int idx, const ed_fm_suspension_info* info)
{
	switch (idx)
	{
	case 0:
		Suspension.wheelNose.setActingForce(info->acting_force[0], info->acting_force[1], info->acting_force[2]);
		Suspension.wheelNose.setActingForcePoint(info->acting_force_point[0], info->acting_force_point[1], info->acting_force_point[2]);
		Suspension.wheelNose.setIntegrityFactor(info->integrity_factor);
		Suspension.wheelNose.setStrutCompression(info->struct_compression);
		Suspension.wheelNose.setWheelSpeed(info->wheel_speed_X);
		break;
	case 1:
		Suspension.wheelLeft.setActingForce(info->acting_force[0], info->acting_force[1], info->acting_force[2]);
		Suspension.wheelLeft.setActingForcePoint(info->acting_force_point[0], info->acting_force_point[1], info->acting_force_point[2]);
		Suspension.wheelLeft.setIntegrityFactor(info->integrity_factor);
		Suspension.wheelLeft.setStrutCompression(info->struct_compression);
		Suspension.wheelLeft.setWheelSpeed(info->wheel_speed_X);
		break;
	case 2:
		Suspension.wheelRight.setActingForce(info->acting_force[0], info->acting_force[1], info->acting_force[2]);
		Suspension.wheelRight.setActingForcePoint(info->acting_force_point[0], info->acting_force_point[1], info->acting_force_point[2]);
		Suspension.wheelRight.setIntegrityFactor(info->integrity_factor);
		Suspension.wheelRight.setStrutCompression(info->struct_compression);
		Suspension.wheelRight.setWheelSpeed(info->wheel_speed_X);
		break;
	default:
		break;
	}
}

void ed_fm_cold_start()
{
	def_TrimPitch = 0.0;
	def_TrimRoll = 0.0;
	def_TrimYaw = 0.0;
#ifdef FSRA_VRA_DATA
	VraControls.resetTrim();
#endif
	stick_pitch = 0;
	stick_roll = 0;
	yaw = 0;
	gear_CMD = 1;
	Controllers.gear_state = 1;
	flap_CMD = 1;
	throttle = 0;
	Fuel.initCold();
	RRAdour.initCold();
	// Hydraulics.initCold();
	GTS.initCold();
	Controllers.canopyCMD = 1.0;
}

void ed_fm_hot_start()
{
	def_TrimPitch = 0.0;
	def_TrimRoll = 0.0;
	def_TrimYaw = 0.0;
#ifdef FSRA_VRA_DATA
	VraControls.resetTrim();
#endif
	stick_pitch = 0;
	stick_roll = 0;
	yaw = 0;
	gear_CMD = 1;
	Controllers.gear_state = 1;
	flap_CMD = 0.5;
	throttle = 0;
	Fuel.initHot();
	RRAdour.initHot();
	// Hydraulics.initHot();
	GTS.initHot();
	Controllers.canopyCMD = 0.0;
}

void ed_fm_hot_start_in_air()
{
	def_TrimPitch = 0.0;
	def_TrimRoll = 0.0;
	def_TrimYaw = 0.0;
#ifdef FSRA_VRA_DATA
	VraControls.resetTrim();
#endif
	stick_pitch = 0;
	stick_roll = 0;
	yaw = 0;
	gear_CMD = 0;
	flap_CMD = 0;
	throttle = 0;
	Fuel.initHot();
	RRAdour.initHot();
	// Hydraulics.initHot();
	GTS.initHot();
	Controllers.canopyCMD = 0.0;
}

void ed_fm_release()
{
	ed_fm_set_current_state(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	ed_fm_set_current_state_body_axis(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	ed_fm_simulate;
	add_local_force(0, 0);
	add_local_moment(0);
	common_moment.x = 0.0;
	common_moment.y = 0.0;
	common_moment.z = 0.0;
	common_force.x = 0.0;
	common_force.y = 0.0;
	common_force.z = 0.0;
	common_force.y = 0.0;
	common_force.z = 0.0;
	mach = 0.0;
	g_accel = 0.0;

	rolling_friction_L = 0.05;
	rolling_friction_R = 0.05;

	airbrake_CMD = 0.0;
	gear_CMD = 0.0;

	GTS.APU_STATE = 0;
	GTS.APU_RPM = 0.0;
	RRAdour.NG_RPM = 0.0;

	stick_pitch = 0;
	stick_roll = 0;
	yaw = 0;
	throttle = 0;
}

void ed_fm_unlimited_fuel(bool value)
{
	Fuel.setUnilimitedFuel(value);
}

bool ed_fm_add_local_force_component(double& x, double& y, double& z, double& pos_x, double& pos_y, double& pos_z)
{
	switch (ForceReturnCounter)
	{
	case 0: // Thrust
		x = RRAdour.getThrustN();
		y = 0;
		z = 0;
#ifdef FSRA_VRA_DATA
		// VRA applies thrust at the CG height (no thrust-line pitching moment)
		pos_x = center_of_gravity.x;
		pos_y = center_of_gravity.y;
		pos_z = 0.0;
#else
		pos_x = thrust_pos.x;
		pos_y = thrust_pos.y;
		pos_z = thrust_pos.z;
#endif
		ForceReturnCounter += 1;
		return true;
		break;
	default:
		x = 0;
		y = 0;
		z = 0;
		pos_x = 0;
		pos_y = 0;
		pos_z = 0;
		return false;
		break;
	}
}

bool ed_fm_add_global_force_component(double& x, double& y, double& z, double& pos_x, double& pos_y, double& pos_z)
{
	return false;
}

bool ed_fm_add_local_moment_component(double& x, double& y, double& z)
{
	return false;
}

bool ed_fm_add_global_moment_component(double& x, double& y, double& z)
{
	return false;
}

bool ed_fm_enable_debug_info()
{
	return false;
}
