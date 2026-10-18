#pragma once
#include "Network.h"
#include "../NetData.h"

namespace Server
{
	class Game
	{
	public:
		bool ClientConnected = false;
		bool ReceiptFromClientsCheck = false;
		unsigned int client_id = 0;
		Game()
		{
			NetworkServer = new Network();
		};
		~Game() {}

		void ServerSessionsRemove()
		{
			NetworkServer->ServerSessionsRemove();
		}

		void ServerNetworkClose()
		{
			NetworkServer->ServerNetworkClose();
		}

		void Finalize_Initial_Contact()
		{
			Packet packet;
			const unsigned int packet_size = sizeof(Packet);
			if (ClientConnected == true)
			{
				if (ReceiptFromClientsCheck == true)
				{
					if (ImpNet::PIC_Connected == true && ImpNet::INSTR_Connceted == true)
					{
						ImpNet::Pilots_Both_Connected = true;
						ImpNet::NetSyncMessage = "Crew Connected";
					}
				}
				else
				{
					std::map<unsigned int, SOCKET>::iterator iter;
					for (iter = NetworkServer->sessions.begin(); iter != NetworkServer->sessions.end(); iter++)
					{
						int packet_length = NetworkServer->receiveData(iter->first, network_data);
						if (packet_length <= 0)
						{
							continue;
						}
						unsigned int i = 0;
						while (i < (unsigned int)packet_length)
						{
							packet.deserialize(&(network_data[i]));
							i += sizeof(Packet);
							switch (packet.Header)
							{
							case pic_cold_start:
								Imp::cold_start = true;
								Imp::hot_start = false;
								Imp::hot_start_in_air = false;
								break;
							case pic_hot_start:
								Imp::cold_start = false;
								Imp::hot_start = true;
								Imp::hot_start_in_air = false;
								break;
							case pic_hot_start_in_air:
								Imp::cold_start = false;
								Imp::hot_start = true;
								Imp::hot_start_in_air = false;
								break;

							case pic_connected:
								ImpNet::PIC_Connected = true;
								ImpNet::INSTR_Initializing = true;
								ReceiptFromClientsCheck = true;
								break;
							case instr_connected:
								ImpNet::INSTR_Connceted = true;
								Imp::HaveControl = true;
								ReceiptFromClientsCheck = true;
								break;
							default:
								break;
							}
						}
					}
				}
			}
			else
			{
				if (NetworkServer->acceptNewClient(client_id))
				{
					client_id++;
					ReceiptFromClientsCheck = false;
					ClientConnected = true;
				}
			}
		}

		void TransmitCommandPackets()
		{
			Packet packet;
			const unsigned int packet_size = sizeof(Packet);
			char packet_data[packet_size];
			if (Imp::cockpit_command_trigger == true)
			{
				packet.Header = CptCmd;
				packet.command = Imp::cockpit_command;
				packet.command_value = Imp::cockpit_command_value;
				packet.serialize(packet_data);
				NetworkServer->sendToAll(packet_data, packet_size);
				Imp::cockpit_command = 0;
				Imp::cockpit_command_trigger = false;
			}
			if (Imp::joystick_transmit_trigger == true)
			{
				//---------------------------------------------------- Throttle Inputs
				packet.Header = ThrottleInput;
				packet.var_value = (float)Imp::ThrottleInput;
				packet.serialize(packet_data);
				NetworkServer->sendToAll(packet_data, packet_size);
			}
		}

	private:

		Network* NetworkServer;

		char network_data[MAX_PACKET_SIZE];
	};
}
