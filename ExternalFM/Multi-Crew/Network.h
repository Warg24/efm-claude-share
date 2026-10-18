#pragma once

#ifndef __DCS__
#define __DCS__

class ImpNet
{
public:
	static char* NetSyncMessage;
	static char* CrewMember_IP;
	static double NetSync_PC_Port;
	static bool ClientConnectError;
	static bool ComServerOnline;
	static bool Clientinit;
	static bool ServerInit;
	static bool AllStop;

	static bool PIC_Connected;
	static bool INSTR_Connceted;
	static bool INSTR_Initializing;
	static bool Instructor_Operational_Side;
	static bool Pilots_Both_Connected;

	static int CptCmd;
	static float CptCmdVal;
	static double LatY;
};

#endif