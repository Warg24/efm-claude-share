#include "Sound.h"
#include <SounderAPI.h>

static ed::Sounder sounder;

void HawkSounders::sounderCreate()
{
	sounder.create("avSounderDevice", "HawkSounders");
}

void HawkSounders::setCurrentState(double arg1, double arg2, double arg3, double arg4, double arg5, double arg6, double argq_x, double argq_y, double argq_z, double argq_w)
{
	sounder.setParam(ED_SounderAPI()->bindParam("posx"), arg4);
	sounder.setParam(ED_SounderAPI()->bindParam("posy"), arg5);
	sounder.setParam(ED_SounderAPI()->bindParam("posz"), arg6);
	sounder.setParam(ED_SounderAPI()->bindParam("orienta"), argq_x);
	sounder.setParam(ED_SounderAPI()->bindParam("orientb"), argq_y);
	sounder.setParam(ED_SounderAPI()->bindParam("orientc"), argq_z);
	sounder.setParam(ED_SounderAPI()->bindParam("orientd"), argq_w);
	sounder.setParam(ED_SounderAPI()->bindParam("velx"), arg1);
	sounder.setParam(ED_SounderAPI()->bindParam("vely"), arg2);
	sounder.setParam(ED_SounderAPI()->bindParam("velz"), arg3);
}

void HawkSounders::update(int apu_state, int engine_state, double canopy, double ext_volume, double cpt_volume, double hlmt_volume)
{
	sounder.sendEvent(ED_SounderAPI()->bindEvent("GTS"), apu_state, ext_volume, cpt_volume);
	sounder.sendEvent(ED_SounderAPI()->bindEvent("ADOUR"), engine_state, ext_volume, cpt_volume);
	sounder.sendEvent(ED_SounderAPI()->bindEvent("ADOUR_START"), engine_state, ext_volume);
	sounder.sendEvent(ED_SounderAPI()->bindEvent("CANOPY"), canopy, ext_volume, cpt_volume);
}