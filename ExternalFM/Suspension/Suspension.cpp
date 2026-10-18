#include "Suspension.h"

void Wheel::setActingForce(double x, double y, double z)
{
	actingForce.x = x;
	actingForce.y = y;
	actingForce.z = z;
}

void Wheel::setActingForcePoint(double x, double y, double z)
{
	actingForcePoint.x = x;
	actingForcePoint.y = y;
	actingForcePoint.z = z;
}

void Wheel::setStrutCompression(const double compression)
{
	strutCompression = compression;
}

double Wheel::getUpLock() const
{
	if (wheelStrutDownAngle == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void Wheel::setStrutAngle(const double angle)
{
	wheelStrutDownAngle = angle;
}

void Wheel::setIntegrityFactor(double d)
{
	integrityFactor = d;
}

void Wheel::setWheelSpeed(double speed)
{
	wheelSpeed = speed;
}