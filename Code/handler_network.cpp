// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================


// -----------------------------------------------------------------------------
//                                                                      includes
// -----------------------------------------------------------------------------

// tank
#include "tank.h"

#include "mod_network.h"
#include "mod_display.h"
#include "mod_game.h"
#include "mod_terrain.h"
#include "mod_ui.h"

#include "handler_network.h"
#include "handler_ui.h"

#include "error_handling.h"

#include "debugging.h"

extern bool started;
extern bool sentReady;
extern bool showWaitingForSlowpokes;

extern bool allowNextTurn;
// -----------------------------------------------------------------------------
//                                                                     functions
// -----------------------------------------------------------------------------

// _____________________________________________________________________________
void HandleNetwork(HWND wind,WPARAM wParam,LPARAM lParam)
//
// Handler for network messages.
//
// --> wind    Handle to the window that caught the message.
// --> wParam  Message parameters.
// --> lParam  Message parameters.
//
{
char TheBuffer[256];

	switch(WSAGETSELECTEVENT(lParam))
	{
		case FD_ACCEPT:
		{
			// Accept the connection.
			NetHostAddPending(wParam);
		}
		break;
		case FD_CONNECT:
		{
			if(NetSocketIsMeta(wParam))
			// Connected to the metaserver.
			{
				NetSendData(wParam,NetMarshallListHostPacket());
			}
			else
			// Connected to the game.
			{
				NetSendData(wParam,NetMarshallJoinPacket(GameGetPlayerName().c_str()));
			}
		}
		break;
		case FD_READ:
		{
			//dcon << "Got some data\n";
			// Get data and handle it.
			NetRecvData(wParam,TheBuffer);
			if(NetSocketIsMeta(wParam))
			{
			int  totalSize;
			int  hostCount;
			int  size;
			char ipBuf[32];
			char nameBuf[32];
			int portLB;
			int portHB;
			int  players;
			int TheOffset;
			//	dcon << "from the meta";
				totalSize = TheBuffer[0];
				hostCount = TheBuffer[1];
				dcon << hostCount;
				TheOffset = 2;
				for(int i = 0; i < hostCount; ++i)
				{
					players = TheBuffer[TheOffset++];
					
					portLB = TheBuffer[TheOffset++];
					portHB = TheBuffer[TheOffset++];
					size = TheBuffer[TheOffset++];
					memcpy(ipBuf,TheBuffer + TheOffset,size);
					ipBuf[size] = 0;
					TheOffset += size;
					size = TheBuffer[TheOffset++];
					memcpy(nameBuf,TheBuffer + TheOffset,size);
					nameBuf[size] = 0;
					TheOffset += size;

					UIHandleAddHostList(players,(portLB << 8) | portLB,ipBuf,nameBuf);
					dcon << players << " " << portLB << " " << portHB << " " << ipBuf << " " << nameBuf << "\n";
					/*
0012FC5C  19 02 00 00 00 00 0C  .......
0012FC63  34 2E 34 32 2E 31 38  4.42.18
0012FC6A  36 2E 31 32 31 05 77  6.121.w
0012FC71  6F 6F 74 7A CC CC CC  ootzÌ*/
				}
			}
			else
			{
				HandleNetworkIncoming(wParam,TheBuffer);
			}
		}
		break;
		case FD_WRITE:
		{
		}
		break;
		case FD_CLOSE:
		{
		}
		break;
		default:
		{
			// Do nothing.
		}
	}
}


// _____________________________________________________________________________
void HandleNetworkIncoming(WPARAM sock,char *packet)
//
// Handler for incoming network packets.
//
// --> sock    Socket that got the data.
// --> packet  Pointer to packet data.
//
{
	switch(NetGetPacketType(packet))
	{
		case kNet_PacketReady:
		{
			if(NetHosting() && !NetPacketIsBroadcast(packet))
			{
				if(!started)
				{
					NetHostSetReady(sock);
				}
				else if(sentReady == false)
				{
					dcon << "HOST GOT READY PACK.\n";
					if(NetAllowClientTurnIfAllClientsReady(sock))
					{
						
						sentReady = true;
					}
				}
			}
			else
			{
			//	if(sentReady == true)
			//	{
					dcon << "CLIENT GOT READY\n";
					allowNextTurn = true;
					showWaitingForSlowpokes  = false;
			//	}
			}
			
/*
			if(NetHosting() && started)
			{
				dcon << "HOST GOT A READY PACK.\n";
				NetAllowClientTurnIfAllClientsReady(sock);
				sentReady = true;
			}
			else
			{
				showWaitingForSlowpokes = false;
				dcon << "Got okay to fire message!\n";
				allowNextTurn = true;
			}*/
		}
		break;
		// Handle a ping.
		case kNet_PacketPing:
		{
			HandleNetworkPingAction(sock,packet);
		}
		break;
		case kNet_PacketJoin:
		// Handle a join packet.
		{
			HandleNetworkJoinAction(sock,packet);
		}
		break;
		case kNet_PacketChat:
		// Handle a chat packet.
		{
			HandleNetworkChatAction(sock,packet);
		}
		break;
		case kNet_PacketTurn:
		// Handle a turn packet.
		{
			HandleNetworkTurnAction(sock,packet);
		}
		break;
		case kNet_PacketStart:
		{
			if(NetPacketIsBroadcast(packet))
			{
			char  TheSeed;
			char  TheID;
			char  TheCount;
			vector<string> TheNames;

				NetUnmarshallStartPacket(packet,NULL,&TheSeed,&TheID,&TheCount,&TheNames);
				first = true;
				GameStart(TheSeed,TheNames,TheID);
			}
		}
		break;
		case kNet_PacketUpdatePlayer:
		{
			HandleNetworkUpdateAction(sock,packet);
		}
		break;
		default:
		{
		}
	}
}


// _____________________________________________________________________________
void HandleNetworkPingAction(WPARAM sock,char *packet)
//
// Handles pings.
//
// --> sock    Socket that got the data.
// --> packet  Pointer to packet data.
//
{
	if(NetHosting())
	{
		NetHostAcceptPing(sock);
	}
}

// _____________________________________________________________________________
void HandleNetworkJoinAction(WPARAM sock,char *packet)
//
// Handles joins.
//
// --> sock    Socket that got the data.
// --> packet  Pointer to packet data.
//
{
int         ThePeer;
StNet_Peer  ThePeerInfo;
char        TheBuffer[256];
	
	// We only accept joins if we haven't started the game yet, and we're hosting.
	if(NetHosting() && !NetPlaying())
	{
		ThePeer = NetHostAddPeer(sock,packet);
		if(ThePeer == kNetErr_Failure)
		{
			dcon << "[* WARNING *]\tA JOIN packet received but the join attempt failed.\n";
		}
		else
		{
			// Display some information indicating who joined.
			ThePeerInfo = NetGetPeerInfo(ThePeer);
			sprintf(TheBuffer,"**** \'%s\' has joined.",ThePeerInfo.name.c_str());
			UIHandleChat(TheBuffer);
		}
	}
}


// _____________________________________________________________________________
void HandleNetworkChatAction(WPARAM sock,char *packet)
//
// Handles chats.
//
// --> sock    Socket that got the data.
// --> packet  Pointer to packet data.
//
{
string  TheChat;
char    TheChatBuf[256];

	if(NetPacketIsBroadcast(packet))
	// If the packet was broadcast by the host, we handle it.
	{
		NetUnmarshallChatPacket(packet,NULL,NULL,&TheChat);
	//	dcon << TheChat;
		strcpy(TheChatBuf,TheChat.c_str());
		if(TheChatBuf[strlen(TheChatBuf) - 1] == '\n')
			TheChatBuf[strlen(TheChatBuf)] = '\0';
		UIHandleChat(TheChatBuf);
	}
	else if(NetHosting())
	// Otherwise, if we're hosting, we broadcast it to everybody.
	{
		NetHostBroadcast(packet);
	}
}


// _____________________________________________________________________________
void HandleNetworkTurnAction(WPARAM sock,char *packet)
//
// Handles turns.
//
// --> sock    Socket that got the data.
// --> packet  Pointer to packet data.
//
{
unsigned char  ThePower;
unsigned char  TheAngle;
unsigned char  TheWeapon;
unsigned char  TheSeed;
 
	if(NetPacketIsBroadcast(packet))
	// If the packet was broadcast by the host, we handle it.
	{
		NetUnmarshallTurnPacket(packet,NULL,&ThePower,&TheAngle,&TheWeapon,&TheSeed);
		dcon << "Turn: p" << static_cast<int>(ThePower) << " a" << static_cast<int>(TheAngle)
		     << " w" << static_cast<int>(TheWeapon) << " s" << static_cast<int>(TheSeed) << "\n";
		GameProcessTurn(ThePower,TheAngle,TheWeapon,TheSeed);
	}
	else if(NetHosting())
	// Otherwise, if we're hosting, we broadcast it to everybody.
	{
		NetHostBroadcast(packet);
	}
}


// _____________________________________________________________________________
void HandleNetworkUpdateAction(WPARAM sock,char *packet)
//
// Handles updates.
//
// --> sock    Socket that got the data.
// --> packet  Pointer to packet data.
//
{
char TheID;
char TheAngle;

	if(NetPacketIsBroadcast(packet))
	// If the packet was broadcast by the host, we handle it.
	{
		NetUnmarshallUpdatePlayerPacket(packet,NULL,&TheID,&TheAngle);
		dcon << "Update packet for " << static_cast<int>(TheID) << ", it is now at " << static_cast<int>(TheAngle) << "\n";
		GameUpdateTank(TheID,TheAngle);
	}
	else if(NetHosting())
	// Otherwise, if we're hosting, we broadcast it to everybody.
	{
		NetHostBroadcast(packet);
	}
}