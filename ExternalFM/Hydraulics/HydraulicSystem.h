#pragma once

// #include "../ExternalFM_API.h"
namespace Hawk
{
	class HydraulicSystem
	{
	private:
	public:
		double hyd1;
		double hyd2;
		double hyd1_value;
		double hyd2_value;
		bool hyd2_reset;

		void initCold();

		void initHot();

		void initHotInAir();

		void setHyd2Reset(const float value);

		void setHyd1(double dt);
		double getHyd1() { return hyd1; }

		void setHyd2(double dt);
		double getHyd2() { return hyd2; }

		void update(double dt);
	};
}
