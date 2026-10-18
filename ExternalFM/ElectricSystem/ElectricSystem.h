#pragma once

class HawkElectricSystem
{
public:
	double volts;
	bool ac1;
	bool ac2;
	bool ac3;

	bool batt1;
	bool batt2;

	HawkElectricSystem()
		: volts(0)
		, ac1(false)
		, ac2(false)
		, ac3(false)
		, batt1(false)
		, batt2(false)
	{}
	~HawkElectricSystem() {}

	void initCold();
	void initHot();
	void initHotInAir() { initHot(); };

	void SetCommand(const int command, const float value);

	void updateAC1(double dt);
	void updateAC2(double dt);
	void updateAC3(double dt);

	void updateVolts(double dt);

	double getAC1() { return ac1; };
	double getAC2() { return ac2; };
	double getAC3() { return ac3; };

	double getVolts() { return volts; };
	bool getBatt1() { return batt1; };
	bool getBatt2() { return batt2; };

	void simulate(double dt);
private:
};
