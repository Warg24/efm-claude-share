#ifndef __ACTUATORS__
#define __ACTUATORS__

class Actuators
{
protected:
	
public:
	Actuators() = default;

	bool simInit = false;

	double canopyCMD = 0;
	double flapPos_DEG = 0;
	double flapRate_DPS = 0;
	double gear_state = 0;
	double gear_rate = 0;
	double airbrake_state = 0;
	double airbrake_rate = 0;
	double canopyPos = 0;
	double canopyRate = 0;
	double throttlePos = 0;
	double throttleRate = 0;

	double gear_actuator(double gear_CMD, double frameTime);

	double  airbrake_actuator(double airbrake_CMD, double frameTime);

	void setCanopy(double dt);

	double getCanopy() { return canopyPos; }

	void simulate(double dt);
};

#endif