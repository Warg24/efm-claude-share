#pragma once
#include <string>

#define MAX_PACKET_SIZE 1000000

enum PacketConfig
{
	CptCmd = 1,
	CtrlRollEnable = 3,
	CtrlPitchEnable = 4,
	pic_cold_start = 5,
	pic_hot_start = 6,
	pic_hot_start_in_air = 7,
	pic_connected = 8,
	instr_connected = 9,
	RollInput = 10,
	PitchInput = 11,
	YawInput = 12,
	ThrottleInput = 13,
	L_wheelBrake = 14,
	R_wheelBrake = 15,
};

struct Packet
{
	unsigned int Header;
	int command;
	int command_value;
	float var_value;
	std::string NetString;

	void serialize(char* data)
	{
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char* data)
	{
		memcpy(this, data, sizeof(Packet));
	}
};
