#include <vector>

typedef struct
{
	double eRPM;
	double eEGT;
} EGTScalar;

typedef struct
{
	double eThrottle;
	double eFuelFlow;
} FuelFlowScalar;

typedef struct
{
	double eMach;
	double eThrust;
} ThrustScalar;

typedef struct
{
	double eAlt;
	double eThrustEff;
} scalarAlt;

std::vector<EGTScalar> egtData =
{
	// RPM | EGT
	{0,		0.0},
	{0.20,	0.0},
	{0.25,	0.0},
	{0.30,	0.0},
	{0.35,	0.0},
	{0.40,	20.0},
	{0.45,	31.0},
	{0.50,	32.0},
	{0.55,	30.0},
	{0.60,	32.0},
	{0.65,	32.0},
	{0.70,	35.0},
	{0.80,	40.0},
	{1.00,	65.0},
	{1.05,	70.0}
};

std::vector<FuelFlowScalar> ffData =
{
	// Throttle   FF
	{0,			  0.0},
	{0.2,		  0.2},
	{0.4,		  0.3},
	{0.6,		  0.75},
	{0.7,		  0.8},
	{0.8,		  0.9},
	{1.0,	      1.0}
};

// NEW 08 JAN 2025

std::vector<ThrustScalar> thrustData =
{
	{0.0,		21750.0},
	{0.1,		21000.0},
	{0.2,		20000.0},
	{0.3,		19430.0},
	{0.4,		18760.0},
	{0.5,		14080.0},
	{0.6,		10500.0},
	{0.7,		8110.0},
	{0.8,		7920.0},
	{0.90,		6570.0},
	{0.95,		5000.0},
	{1.0,		4410.0},
	{1.1,		4250.0},
	{1.2,		4100.0},
	{1.3,		3000.0},
	{2.5,		2700.0},
	{3.9,		2000.0},
};

// BACKUP OLD 08 JAN 2025
// 
//std::vector<ThrustScalar> thrustData =
//{
//	// M		Thrust[N]
//	{0,			23130.75},
//	{0.2,		23130.75},
//	{0.4,		23000},
//	{0.6,		22000},
//	{0.8,		20000}
//};

std::vector<scalarAlt> scalarDueToAlt =
{
	// Alt [Ft]	    eThrust
	{0,				1.0},
	{5000,			0.99},
	{10000,			0.9},
	{15000,			0.8},
	{20000,			0.8},
	{25000,			0.7},
	{30000,			0.1}
};