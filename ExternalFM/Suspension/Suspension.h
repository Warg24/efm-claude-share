#pragma once

#include <cmath>
#include "../ED_FM_Utility.h"

class Wheel
{
protected:
public:
	double strutCompression = 0.0;
	double wheelStrutDownAngle = 0.0;
	double brakeInput = 0.0;

	Vec3 actingForce;
	Vec3 actingForcePoint;
	double integrityFactor = 0.0;
	double wheelSpeed = 0.0;

	Wheel() :
		actingForce(),
		actingForcePoint()
	{}
	~Wheel() {}

	void setActingForce(double x, double y, double z);
	void setActingForcePoint(double x, double y, double z);

	void setStrutCompression(const double compression);

	bool isWow() const { return strutCompression > 0.0; }

	double getUpLock() const;

	void setStrutAngle(const double angle);

	void setIntegrityFactor(double d);

	void setWheelSpeed(const double speed);
};

class LandingGear
{
private:
public:
	Wheel wheelNose;
	Wheel wheelLeft;
	Wheel wheelRight;

	bool parkingBrakeOn = false;
};