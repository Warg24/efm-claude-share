#pragma once
#include "../stdafx.h"
#include <WinSock2.h>

class NetServices
{
public:
	static int sendMessage(SOCKET curSocket, char* message, int messageSize)
	{
		return send(curSocket, message, messageSize, 0);
	}

	static int receiveMessage(SOCKET curSocket, char* buffer, int bufSize)
	{
		return recv(curSocket, buffer, bufSize, 0);
	}
};