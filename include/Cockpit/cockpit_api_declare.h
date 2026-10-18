#pragma once
#include "ccParametersAPI.h"

class EDPARAM
{
public:
	EDPARAM()
	{
		ed_param_api = ed_get_cockpit_param_api();
	}

	cockpit_param_api ed_param_api;

	void* getParamHandle(const char* name)
	{
		return ed_param_api.pfn_ed_cockpit_get_parameter_handle(name);
	}

	void setParamNumber(void* handle, double value)
	{
		ed_param_api.pfn_ed_cockpit_update_parameter_with_number(handle, value);
		//cockpitAPI.setParamNumber(cockpitAPI.getParamHandle("Bell_Alarm_state"), Bell_Alarm_state);
	}

	void setParamString(void* handle, const char* string)
	{
		ed_param_api.pfn_ed_cockpit_update_parameter_with_string(handle, string);
		//cockpitAPI.setParamString(ParmHandle_NetsyncMessage, (const char *)ImpNet::NetSyncMessage);
	}

	double getParamNumber(void* handle)
	{
		double res = 0;
		ed_param_api.pfn_ed_cockpit_parameter_value_to_number(handle, res, false);
		return res;
	}

	void getParamString(void* handle, char* buffer, unsigned buffer_size)
	{
		ed_param_api.pfn_ed_cockpit_parameter_value_to_string(handle, buffer, buffer_size);
	}

	int compareParams(void* handle1, void* handle2)
	{
		return ed_param_api.pfn_ed_compare_parameters(handle1, handle2);
	}
};