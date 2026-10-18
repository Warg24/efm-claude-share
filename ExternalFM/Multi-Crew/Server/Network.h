#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "../NetServices.h"
#include <map>
#include <string>
#include "../Network.h"

#pragma comment (lib, "Ws2_32.lib")
#define MAX_PACKET_SIZE 1000000
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "13"

namespace Server
{
	class Network
	{
	public:
		SOCKET NetsyncListenSocket;
		SOCKET NetsyncClientSocket;
		int iResult;
		std::map<unsigned int, SOCKET> sessions;

		Network()
		{
			WSADATA wsaData;

			NetsyncListenSocket = INVALID_SOCKET;
			NetsyncClientSocket = INVALID_SOCKET;

			struct addrinfo* result = NULL;
			struct addrinfo hints;

			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0)
			{
				ImpNet::ComServerOnline = true;
			}

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			iResult = getaddrinfo(NULL, (char*)std::to_string((unsigned)(int)ImpNet::NetSync_PC_Port).c_str(), &hints,
				&result);

			if (iResult != 0)
			{
				WSACleanup();
			}

			NetsyncListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

			if (NetsyncListenSocket == INVALID_SOCKET)
			{
				freeaddrinfo(result);
				WSACleanup();
			}

			u_long iMode = 1;
			iResult = ioctlsocket(NetsyncListenSocket, FIONBIO, &iMode);

			if (iResult == SOCKET_ERROR)
			{
				closesocket(NetsyncListenSocket);
				WSACleanup();
			}

			iResult = bind(NetsyncListenSocket, result->ai_addr, (int)result->ai_addrlen);

			if (iResult == SOCKET_ERROR)
			{
				freeaddrinfo(result);
				closesocket(NetsyncListenSocket);
				WSACleanup();
			}

			freeaddrinfo(result);

			iResult = listen(NetsyncListenSocket, SOMAXCONN);

			if (iResult == SOCKET_ERROR)
			{
				closesocket(NetsyncListenSocket);
				WSACleanup();
			}

			if (NetsyncListenSocket != INVALID_SOCKET)
			{
				ImpNet::NetSyncMessage = "Netsync Standby";
				ImpNet::ComServerOnline = true;
			}
			else
			{
				ImpNet::ComServerOnline = false;
				ImpNet::NetSyncMessage = "Netsync Disable";
			}
		}

		~Network() {}

		bool acceptNewClient(unsigned int& id)
		{
			NetsyncClientSocket = accept(NetsyncListenSocket, NULL, NULL);

			if (NetsyncClientSocket != INVALID_SOCKET)
			{
				char value = 1;
				setsockopt(NetsyncClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

				sessions.insert(std::pair<unsigned int, SOCKET>(id, NetsyncClientSocket));

				return true;
			}

			return false;
		}

		int receiveData(unsigned int client_id, char* recvbuf)
		{
			if (sessions.find(client_id) != sessions.end())
			{
				SOCKET NetsynccurrentSocket = sessions[client_id];
				iResult = NetServices::receiveMessage(NetsynccurrentSocket, recvbuf, MAX_PACKET_SIZE);

				if (iResult == 0)
				{
					ImpNet::NetSyncMessage = "recData failed";
				}

				return iResult;
			}

			return 0;
		}

		void sendToAll(char* packets, int totalSize)
		{
			SOCKET NetsynccurrentSocket;
			std::map<unsigned int, SOCKET>::iterator iter;
			int iSendResult;

			for (iter = sessions.begin(); iter != sessions.end(); iter++)
			{
				NetsynccurrentSocket = iter->second;
				iSendResult = NetServices::sendMessage(NetsynccurrentSocket, packets, totalSize);

				if (iSendResult == SOCKET_ERROR)
				{
					ImpNet::NetSyncMessage = "send failed";
				}
			}
		}

		void ServerSessionsRemove()
		{
			if (!sessions.empty())
			{
				SOCKET NetsynccurrentSocket;
				std::map<unsigned int, SOCKET>::iterator iter;

				for (iter = sessions.begin(); iter != sessions.end(); iter++)
				{
					NetsynccurrentSocket = iter->second;
					closesocket(NetsynccurrentSocket);
				}
				sessions.clear();
			}
		}

		void ServerNetworkClose()
		{
			closesocket(NetsyncListenSocket);
			closesocket(NetsyncClientSocket);
			WSACleanup();
		}
	};
}
