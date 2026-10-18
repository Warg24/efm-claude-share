#pragma once

#include "../ExternalFM_API.h"

class HawkSounders
{
private:
public:
	EXTERNALFM_API void sounderCreate();

	EXTERNALFM_API void setCurrentState(double arg1, double arg2, double arg3, double arg4, double arg5, double arg6, double argq_x, double argq_y, double argq_z, double argq_w);

	EXTERNALFM_API void update(int apu_state, int engine_state, double canopy, double ext_volume, double cpt_volume, double hlmt_volume);
};
