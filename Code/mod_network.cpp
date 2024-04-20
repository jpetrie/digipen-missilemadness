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

#include "mod_display.h"
#include "mod_game.h"
#include "mod_network.h"

#include "mod_econ.h"

#include "mod_tank.h"
#include "error_handling.h"

#include "debugging.h"


// -----------------------------------------------------------------------------
//                                                            internal variables
// -----------------------------------------------------------------------------

static WSADATA            iWSAData;     // Holds WinSock data.
static StNet_HostState    iHostData;    // Holds host data.
static StNet_ClientState  iClientData;  // Holds client data.

static bool  iIsHost;     // True if we're hosting a game.
static bool  iIsClient;   // True if we've joined a game.
static bool  iIsPlaying;  // True if the game is in progress.

extern bool started;
extern int iTurn;
extern tank *iTank;
extern bool first;
extern bool allowNextTurn;
extern bool goToPostMortemASAP;
extern bool iInPostMortem;
extern vector<Blast> vBlast;
extern int gItemCtr;
extern int gWeapCtr;
extern int Angle;
extern int Power;
extern int gInitialCash;
extern int cClick;
extern int mClick;
extern int cWeapClick;
extern volatile int mWeapClick;
extern int mIAClick;
extern int cIAClick;
extern int cWAClick;
extern int mWAClick;
extern bool thinking;
extern vector <string> vec_chatList;

extern volatile int weapIndx;
extern int weapAIndx;
extern int itemIndx;
extern int itemAIndx;
extern int joinIndx;

extern int gInitialItem[MAX_ITEMS];
extern int gInitialWeap[MAX_WEAPONS];

extern vector<int> gDeadList;

extern bool launch;
extern bool singlePlayer;

// -----------------------------------------------------------------------------
//                                                                     functions
// -----------------------------------------------------------------------------

// _____________________________________________________________________________
void NetStartup(void)
//
// Starts up the networking layer.
//
{
	dcon << "[STARTUP]\tStarting network services\n";

	WSAStartup(MAKEWORD(2,2),&iWSAData);

	iIsHost    = false;
	iIsClient  = false;
	iIsPlaying = false;

	// Clear data structures.
	iHostData.hostedGameName = "";
	iHostData.peerCount      = 0;
	iHostData.validPeerCount = 0;

	// 
	for(int i = 0; i < kNet_ClientsMax; ++i)
	{
		iHostData.peer[i].sock = 0;
		iHostData.peer[i].ready = false;
	}
}


// _____________________________________________________________________________
void NetShutdown(void)
//
// Shuts down the networking layer and associates it with the specified window.
//
{
	dcon << "[SHUTDOWN]\tStopping network services\n";

	// Ensure all host sockets are released.
	if(NetHosting())
	{
		shutdown(iHostData.metaSock,2);
		closesocket(iHostData.metaSock);

		shutdown(iHostData.listenSock,2);
		closesocket(iHostData.listenSock);

		for(int i = 0; i < kNet_ClientsMax; ++i)
		{
			shutdown(iHostData.peer[i].sock,2);
			closesocket(iHostData.peer[i].sock);
		}
	}
	
	// Ensure all client sockets are released.
	shutdown(iClientData.serverSock,2);
	closesocket(iClientData.serverSock);

	// Clean up WinSock.
	WSACleanup();
}


// _____________________________________________________________________________
void NetIdle(void)
//
// Handles idle-time processing.
//
{
//	dcon << "[NETWORK]\tDoing idle processing...\n";

	if(NetHosting())
	{
		// Ping the metaserver with updated game info.
		if(--iHostData.timeSincePing <= 0 && !NetPlaying())
		{
			NetHostGameUpdate(iHostData.validPeerCount);
			iHostData.timeSincePing = kNet_HostPingDelay;
		}

		// Decrement pending connection delays,
		// drop pending connections that have expired.
		for(int i = 0; i < kNet_ClientsMax; ++i)
		{
			// This is how we check for an actual connection...
			if(iHostData.peer[i].sock != 0)
			{
				iHostData.peer[i].lifetime--;
				if(iHostData.peer[i].lifetime <= 0)
				{
					// Drop the sucker.
					dcon << "\t\tDropping connection at " << iHostData.peer[i].address.c_str() << "\n";

					// Kill a dropped peer.
					if(iHostData.peer[i].valid)
					{
						if(started)
						{
							dcon << "Killed the tank from " << iHostData.peer[i].address.c_str() << "\n";
							iTank[iHostData.peer[i].order].dead = true;
						}
					}
					
					shutdown(iHostData.peer[i].sock,2);
					closesocket(iHostData.peer[i].sock);
					iHostData.peer[i].sock  = 0;
					iHostData.peer[i].valid = false;
					if(started)
					{
						NetClientUpdate(static_cast<char>(iHostData.peer[i].order),static_cast<char>(kNet_TurnKill));// angle of 255 kills the player.
					}
				}
			}
		}
	}
	
	
	if(NetJoined() || NetPlaying())
	{
		// Ping the server if we've been idle for a while.
		iClientData.timeSincePing--;
		if(iClientData.timeSincePing <= 0)
		{
			NetClientPing();
			iClientData.timeSincePing = kNet_ClientPingDelay;
		}
	}
}


// _____________________________________________________________________________
int NetHostGame(int port,string name)
//
// Begins hosting a game on the specified port (fails if this machine is already
// hosting a game or has joined a game already). If this function completes
// successfully, then the game will be in the pre-game stage, awaiting client
// connections.
//
// --> port  The port on which to host the game.
// --> name  The name of the game.
// -R-       Error code, or kNetErr_Success.
//
{
bool  FlgRegistered;

	dassert(port > 1024);    // Must used nonprivledged port.
	dassert(!name.empty());  // Must have a game name.

	if(NetHosting() || NetJoined())
	{
		return (kNetErr_AlreadyInGame);
	}
	else
	{
		dcon << "\n";
		dcon << "[SERVER]\tPreparing to host game...\n";

		// Set up listen address.
		iHostData.listenAddr.sin_family       = AF_INET;
		iHostData.listenAddr.sin_addr.s_addr  = INADDR_ANY;
		iHostData.listenAddr.sin_port         = htons(port);
		memset(&(iHostData.listenAddr.sin_zero),'\0',8);

		// Set up listen socket.
		iHostData.listenSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(iHostData.listenSock == INVALID_SOCKET)
		{
			dcon << "[SERVER]\tERROR: could not create listen socket\n";
			return (kNetErr_CouldNotSocket);
		}

		// Bind the socket and configure async messaging for it.
		if(bind(iHostData.listenSock,(LPSOCKADDR)&iHostData.listenAddr,sizeof(iHostData.listenAddr)) == -1)
		{
			dcon << "[SERVER]\tERROR: could not bind listen socket\n";
			return (kNetErr_CouldNotBind);
		}

		WSAAsyncSelect(iHostData.listenSock,gMainWindow,WM_NETWORK,(FD_ACCEPT | FD_CLOSE));
		if(WSAGetLastError() != 0)
		{
			dcon << "[SERVER]\tERROR: could not select listen socket\n";
			return (kNetErr_CouldNotSelect);
		}
		
		// Listen.
		if(listen(iHostData.listenSock,kNet_ConnectBacklog) == -1)
		{
			dcon << "[SERVER]\tERROR: could not listen on listen socket\n";
			return (kNetErr_CouldNotListen);
		}
		iHostData.hostedGameName = name;
		dcon << "[SERVER]\t\tHosting game \"" << name << "\" at " << NetGetLocalIP() << ":" << port << "\n";

		// Inform the metaserver.
		FlgRegistered = false;

hostent *TheHostInfo;
//char     TheBuffer[256];

		TheHostInfo = gethostbyname(iHostData.cfgMetaName.c_str());
		dcon << "[SERVER]\t\tRegistering game with the metaserver was ";
		iHostData.metaAddr.sin_family       = AF_INET;
		iHostData.metaAddr.sin_addr.s_addr  = inet_addr(iHostData.cfgMetaName.c_str());
		iHostData.metaAddr.sin_port         = htons(iHostData.cfgMetaPort);
		memset(&(iHostData.metaAddr.sin_zero),'\0',8);

		iHostData.metaSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(iHostData.metaSock != INVALID_SOCKET)
		{
			if(connect(iHostData.metaSock,(LPSOCKADDR)&iHostData.metaAddr,sizeof(iHostData.metaAddr)) != -1)
			{
				if(NetSendData(iHostData.metaSock,NetMarshallStartHostPacket(port,name)) == kNetErr_Success)
				{
					FlgRegistered = true;
				}
			}
		}
		dcon << (FlgRegistered ? "successful\n" : "unsuccessful\n");

		// Now join the game.
		dcon << "[SERVER]\t\tJoining game via loopback device...\n";
		NetJoinGame("127.0.0.1",port);

		// Set host state appropriately.
		iIsHost = true;
		iHostData.timeSincePing = kNet_HostPingDelay;
	}

	// Return.
	return (kNetErr_Success);
}


// _____________________________________________________________________________
int NetHostGameUpdate(char playercount)
//
// Updates the game tracking information on the metaserver.
//
{
	dcon << "[SERVER]\tUpdating metaserver tracking information with " << static_cast<int>(playercount) << " players.\n";

	NetSendData(iHostData.metaSock,NetMarshallRefreshHostPacket(playercount));
	return (kNetErr_Success);
}
	

// _____________________________________________________________________________
int NetHostGameStart(void)
//
// Begins the game being hosted. This function does nothing if we're not
// hosting a game or if the game has been started already.
//
{
char             TheSeed;
char           *ThePacket;
vector<string>  TheNames;
int             i;

	dcon << "\n";
	dcon << "[SERVER]\tStarting the game...\n";

	// Remove from metaserver.
	NetSendData(iHostData.metaSock,NetMarshallDropHostPacket());

	// Close the listen socket, we're no longer accepting connections.
	dcon << "[SERVER]\t\tClosing listen socket\n";
	shutdown(iHostData.listenSock,2);
	closesocket(iHostData.listenSock);

	// Drop all connections that are not yet valid.
	iHostData.peerCount = iHostData.validPeerCount;
	for(i = 0; i < kNet_ClientsMax; ++i)
	{
		if(!iHostData.peer[i].valid)
		{
			shutdown(iHostData.peer[i].sock,2);
			closesocket(iHostData.peer[i].sock);
		}
	}

	// Prepare and broadcast the start packet.
	srand(clock());
	TheSeed = rand();// % 256;
	for(i = 0; i < kNet_ClientsMax; ++i)
	{
		if(iHostData.peer[i].valid)
		{
			TheNames.push_back(iHostData.peer[i].name);
		}
	}

	dcon << "[SERVER]\t\tBroadcasting start packet with seed of " << static_cast<unsigned char>(TheSeed) << "\n";
	
	// 'Fake' a broadcast.
	for(i = 0; i < kNet_ClientsMax; ++i)
	{
		if(iHostData.peer[i].valid)
		{
			// Alter turn order byte so it is correct for each player.
			ThePacket = NetMarshallStartPacket(TheSeed,i,TheNames);
			ThePacket[kNet_Marshalled_Type] *= -1;
			NetSendData(iHostData.peer[i].sock,ThePacket);
		}
	}
	
	// We are now playing. Woo.
	iIsPlaying = true;

	// Unready all tanks!
	for(i = 0; i < kNet_ClientsMax; ++i)
	{
		if(iHostData.peer[i].valid)
		{
			iHostData.peer[i].ready = false;
		}
	}

	return (kNetErr_Success);
}


// _____________________________________________________________________________
int NetHostAddPending(SOCKET sock)
//
// Accepts a network connection from a socket.
//
// If the connection was accepted, the remote machine is treated as a 'pending'
// client and counts towards the maximum number of clients allowed in the game.
// However the client is not actually included in the game until it sends a
// join packet. Pending clients who do not send a join packet after a certain
// period of time are dropped from the game.
//
// This call has no effect if the game has started and will return failure.
//
// -R-  A result code, or kNetErr_Success.
//
{
SOCKADDR_IN  TheAddr;
int          TheAddrLen;

	if(!NetPlaying())
	{
		if(sock != INVALID_SOCKET)
		{
			// Prevent more than the maximum number of players from joining the game.
			if(iHostData.peerCount >= kNet_ClientsMax)
			{
				return (kNetErr_GameIsFull);
			}
			else
			{
				// Accept the connection.
				TheAddrLen = sizeof(TheAddr);
				iHostData.peer[iHostData.peerCount].sock = accept(sock,(LPSOCKADDR)&TheAddr,&TheAddrLen);
				if(iHostData.peer[iHostData.peerCount].sock == INVALID_SOCKET)
				{
					dcon << "[SERVER]\tERROR: could not accept on listen socket\n";
					return (kNetErr_CouldNotAccept);
				}

				// Accepted sockets retain the async messaging properties of the socket that
				// accepted them; we need to change them.
				WSAAsyncSelect(iHostData.peer[iHostData.peerCount].sock,gMainWindow,WM_NETWORK,(FD_READ | FD_WRITE | FD_CLOSE));
				if(WSAGetLastError() != kNetErr_Success)
				{
					dcon << "[SERVER]\tERROR: could not select accepted socket\n";
					return (kNetErr_CouldNotSelect);
				}

				// Obtain the rest of the information.
				iHostData.peer[iHostData.peerCount].address  = inet_ntoa(TheAddr.sin_addr);
				iHostData.peer[iHostData.peerCount].port     = ntohs(TheAddr.sin_port);
				iHostData.peer[iHostData.peerCount].valid    = false;
				iHostData.peer[iHostData.peerCount].lifetime = kNet_PendingLifetime;
				dcon << "[SERVER]\t\t" << iHostData.peer[iHostData.peerCount].address << " connected\n";
				iHostData.peerCount++;
			}
			return (kNetErr_Success);
		}
	}
	return (kNetErr_Failure);
}


// _____________________________________________________________________________
int NetHostAddPeer(SOCKET sock,char *packet)
//
// Adds the machine at the remote end of `sock` to the game list, but only if
// the machine is in the list of pending connections. If the game has already
// started, this function returns failure and has n effect.
//
// --> sock    Socket connected to the client.
// --> packet  Pointer to a the marshalled join packet sent by the client.
// -R-         The peer ID of the new peer if successful.
//             kNetErr_Failure otherwise.
//
{
bool    FlgFound;
char    TheNameLength;
string  TheName;
int     ThePeer;

	FlgFound = false;
	if(!NetPlaying())
	{
		// Find the socket in the peer list.
		for(ThePeer = 0; ThePeer < kNet_ClientsMax; ++ThePeer)
		{
			if(iHostData.peer[ThePeer].sock == sock)
			{
				// Update the peer list to reflect that this client has been
				// accepted into the game.
				NetUnmarshallJoinPacket(packet,NULL,&TheNameLength,&TheName);

				iHostData.peer[ThePeer].name     = TheName;
				iHostData.peer[ThePeer].order    = iHostData.validPeerCount;
				iHostData.peer[ThePeer].valid    = true;
				iHostData.peer[ThePeer].lifetime = kNet_PeerLifetime;
				iHostData.validPeerCount++;
				dcon << "[SERVER]\t\t" << iHostData.peer[ThePeer].address << " joined as \'" << iHostData.peer[ThePeer].name << "\'\n";

				// We found what we were looking for.
				FlgFound = true;
				break;
			}
		}
	}
	
	return (FlgFound ? ThePeer : kNetErr_Failure);
}


// _____________________________________________________________________________
int NetHostBroadcast(char *data)
//
// Sends packet data to all clients.
//
// The buffer pointed to by `data` must be length-prefixed, and the length byte
// be included in the length (i.e., a ping packet -- which contains no payload
// data -- is one byte for the length prefix and one byte for the packet code.
// The value of the length byte in this case would be two).
//
// This function will release the memory pointed to by `data` if it is
// successful in sending it. The data will be released via the delete[]
// operator, so it must be allocated on the free-store with the new[]
// operator.
//
// Because of the above two caveats, it is generally easier to pass the return
// from a packet-marshalling function (such as NetMarshallPingPacket()), rather
// than attemping to coerce your own data buffer.
//
// This function will send the entire buffer across the network.
//
// --> s     The socket to send the data over.
// --> data  Pointer to the data buffer.
// -R-       General-purpose return code (success or failure).
//           If kNetErr_Failure is returned, `data` is still valid.
//
{
char *TheBuffer;

	for(int i = 0; i < kNet_ClientsMax; ++i)
	{
		if(iHostData.peer[i].valid)
		{
			TheBuffer = new char[data[0] + 1];
			memcpy(TheBuffer,data,data[0]);
			TheBuffer[TheBuffer[0]] = '\0';
			
			// Invert the packet type code because we're sending a broadcast packet.
			TheBuffer[kNet_Marshalled_Type] *= -1;

			NetSendData(iHostData.peer[i].sock,TheBuffer);
		}
	}

	return (kNetErr_Success);
}


// _____________________________________________________________________________
int NetJoinGame(string address,int port)
//
// Joins the game at the specified address and port; has no effect if the player
// has already joined a game or if the game is in progress. If this function
// returns successfully, the player will be in the pre-game section of the game.
//
// -R-  General purpose return code (success or failure).
{
	dassert(port > 1024);       // Must used nonprivledged port.
	dassert(!address.empty());  // Must have an address.

	address = NetGetIPFromDNS(address.c_str());

	dcon << "\n";
	dcon << "[CLIENT]\tPreparing to join game...\n";

	// Set up listen address.
	iClientData.serverAddr.sin_family       = AF_INET;
	iClientData.serverAddr.sin_addr.s_addr  = inet_addr(address.c_str());
	iClientData.serverAddr.sin_port         = htons(port);
	memset(&(iClientData.serverAddr.sin_zero),'\0',8);

	// Set up server socket.
	iClientData.serverSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(iClientData.serverSock == INVALID_SOCKET)
	{
		dcon << "[CLIENT]\tERROR: could not create join socket\n";
		return (kNetErr_CouldNotSocket);
	}

	// Configure async messaging and connect.
	WSAAsyncSelect(iClientData.serverSock,gMainWindow,WM_NETWORK,(FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE));
	if(WSAGetLastError() != 0)
	{
		dcon << "[CLIENT]\tERROR: could not select join socket\n";
		return (kNetErr_CouldNotSelect);
	}

	// Note that we cannot error-check this function here, as it always returns failure
	// since we set it up for async messaging.
	connect(iClientData.serverSock,(LPSOCKADDR)&iClientData.serverAddr,sizeof(iClientData.serverAddr));
	
	dcon << "[CLIENT]\t\tJoining the game at " << address << ":" << port << "\n\n";

	// Set state appropriately.
	iIsClient = true;
	iClientData.timeSincePing = kNet_ClientPingDelay;

	// Return.
	return (kNetErr_Success);
}


// _____________________________________________________________________________
int NetHostAcceptPing(SOCKET sock)
//
// Accepts a ping from a client.
//
// -R-  General purpose return code (success or failure).
{
	for(int i = 0; i < kNet_ClientsMax; ++i)
	{
		if(iHostData.peer[i].sock == sock)
		{
			iHostData.peer[i].lifetime = kNet_PeerLifetime;
		}
	}

	return (kNetErr_Success);
}


// _____________________________________________________________________________
int NetClientPing(void)
//
// Sends a ping message from the client to the host.
//
// -R-  General purpose return code (success or failure).
{
char *TheBuffer;
int   TheResult;

	// Build the ping.
	TheBuffer = new char[3];
	TheBuffer[0] = 2;
	TheBuffer[1] = kNet_PacketPing;
	TheBuffer[2] = '\0';

	// Send the ping.
	// NetSendData() will release the memory for us.
	TheResult = NetSendData(iClientData.serverSock,TheBuffer);

	return (TheResult);
}


// _____________________________________________________________________________
int NetClientChat(string chat)
//
// Sends a chat message from the client to the host.
//
// -R-  General purpose return code (success or failure).
{
string  TheChat;

	// Format the chat.
	TheChat = GameGetPlayerName() + ": " + chat;

	// Send the chat.
	return (NetSendData(iClientData.serverSock,NetMarshallChatPacket(TheChat)));
}


// _____________________________________________________________________________
int NetClientTurn(unsigned char power,unsigned char angle,unsigned char weapon,unsigned char seed)
//
// Sends a turn message from the client to the host.
//
// -R-  General purpose return code (success or failure).
{
	// Send the chat.
	return (NetSendData(iClientData.serverSock,NetMarshallTurnPacket(power,angle,weapon,seed)));
}


// _____________________________________________________________________________
int NetClientTurn_Dead(void)
//
// Sends a turn message from the client to the host indicating that the tank
// sending the packet is dead.
//
// -R-  General purpose return code (success or failure).
{
	// Send the chat.
	return (NetSendData(iClientData.serverSock,NetMarshallTurnPacket(kNet_TurnDead,0,0,0)));
}



// _____________________________________________________________________________
int NetClientTurn_Item(unsigned char item)
//
// Sends a turn message from the client to the host indicating that the tank
// is using the item specified by `item`.
//
// -R-  General purpose return code (success or failure).
{
	// Send the chat.
	return (NetSendData(iClientData.serverSock,NetMarshallTurnPacket(0,kNet_TurnItem,item,0)));
}


// _____________________________________________________________________________
int NetClientUpdate(char id,char ang)
//
// Sends a turn message from the client to the host indicating that the tank
// sending the packet is dead.
//
// -R-  General purpose return code (success or failure).
{
	// Send the chat.
	return (NetSendData(iClientData.serverSock,NetMarshallUpdatePlayerPacket(id,ang)));
}



// _____________________________________________________________________________
int NetClientMSList(void)
//
// Sends a 'list host' message to the metaserver.
//
// -R-  General purpose return code (success or failure).
{
	iClientData.metaAddr.sin_family       = AF_INET;
	iClientData.metaAddr.sin_addr.s_addr  = inet_addr(iHostData.cfgMetaName.c_str());// okay to use hostdata in this case
	iClientData.metaAddr.sin_port         = htons(iHostData.cfgMetaPort);
	memset(&(iClientData.metaAddr.sin_zero),'\0',8);

	iClientData.metaSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(iClientData.metaSock != INVALID_SOCKET)
	{
		// Configure async messaging and connect.
		WSAAsyncSelect(iClientData.metaSock,gMainWindow,WM_NETWORK,(FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE));
		connect(iClientData.metaSock,(LPSOCKADDR)&iClientData.metaAddr,sizeof(iClientData.metaAddr));
	}

	// Failed.
	return (kNetErr_Failure);
}


// _____________________________________________________________________________
int NetSendData(SOCKET s,char *data)
//
// Low-level function to transmit packet data.
//
// The buffer pointed to by `data` must be length-prefixed, and the length byte
// be included in the length (i.e., a ping packet -- which contains no payload
// data -- is one byte for the length prefix and one byte for the packet code.
// The value of the length byte in this case would be two).
//
// This function will release the memory pointed to by `data` if it is
// successful in sending it. The data will be released via the delete[]
// operator, so it must be allocated on the free-store with the new[]
// operator.
//
// Because of the above two caveats, it is generally easier to pass the return
// from a packet-marshalling function (such as NetMarshallPingPacket()), rather
// than attemping to coerce your own data buffer.
//
// This function will send the entire buffer across the network.
//
// --> s     The socket to send the data over.
// --> data  Pointer to the data buffer.
// -R-       General-purpose return code (success or failure).
//           If kNetErr_Failure is returned, `data` is still valid.
//
{
int  TheTotalBytes;
int  TheSentBytes;
int  TheCount;

	TheTotalBytes = data[0];
	TheSentBytes  = 0;

	// Make sure the entire buffer is sent.
	while(TheSentBytes < TheTotalBytes)
	{
		TheCount = send(s,(data + TheSentBytes),(TheTotalBytes - TheSentBytes),0);

		if(TheCount == kNetErr_Failure)
		{
			return (kNetErr_Failure);
		}

		TheSentBytes += TheCount;
	}

	// Release the buffer.
	delete[] data;
	return (kNetErr_Success);
}


// _____________________________________________________________________________
int NetRecvData(SOCKET s,char *data)
//
// Low-level function to receive packet data.
//
// The buffer pointed to by `data` must be pre-allocated; it will be filled
// out with the incoming network data, and length-prefixed.
//
// --> s     The socket to get the data from.
// --> data  Pointer to buffer to fill with incoming data.
// -R-       General-purpose return code (success or failure).
//
{
int  TheTotalBytes;
int  TheRecvBytes;
int  TheCount;

	TheCount = recv(s,data,256,0);
	if(TheCount == kNetErr_Failure)
	{
		return (kNetErr_Failure);
	}

	TheTotalBytes = data[0];
	TheRecvBytes  = TheCount;

	// Make sure the entire buffer is received.
	while(TheRecvBytes < TheTotalBytes)
	{
		TheCount = recv(s,(data + TheRecvBytes),256,0);
		if(TheCount == kNetErr_Failure)
		{
			return (kNetErr_Failure);
		}

		TheRecvBytes += TheCount;
	}

	// Return.
	return (kNetErr_Success);
}


// _____________________________________________________________________________
int NetGetPacketType(const char *data)
//
// Returns the packet type corrisponding to the data pointed to by `data`.
// Returns kNetErr_Failure if the data is malformed.
//
// --> data  Pointer to packet data.
// -R-       Packet type of data.
//
{
	dassert(data != NULL); // Must have valid data.

	// Obfuscated if() block to make sure the packet header is, in fact,
	// a valid packet header (without crashing).
	if(data[kNet_Marshalled_Size] != '\0' && data[kNet_Marshalled_Type] != '\0')
	{
		// Packet could be a broadcast packet, so return the absolute value of the code.
		return ((data[kNet_Marshalled_Type] > 0) ? (data[kNet_Marshalled_Type]) : -(data[kNet_Marshalled_Type]));
	}
	else
	{
		return (kNetErr_Failure);
	}
}


// _____________________________________________________________________________
char* NetMarshallJoinPacket(string name)
//
// Returns a pointer to data marshalled for sending as a join packet. The buffer
// is null-terminated.
//
// The format of the packet is:
//
//   1b  packet size (does not account for null terminator).
//   1b  packet type
//   1b  name length
//   xb  name data
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> name  The name of the player who is joining.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	dassert(name.length() <= kNet_NameStrLimit);  // Clamp name limit.

	TheBuffer                               = new char[4 + name.length()];
	TheBuffer[kNet_Marshalled_Size]         = 3 + name.length();   // Packet size.
	TheBuffer[kNet_Marshalled_Type]         = kNet_PacketJoin;     // Packet type.
	TheBuffer[kNet_MarshalledJoin_NameSize] = name.length();       // Name length.
	strcpy(TheBuffer + kNet_MarshalledJoin_NameBase,name.c_str()); // Name data.
	TheBuffer[TheBuffer[0]] = 0;                                   // Null terminator.
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallStartHostPacket(short port,string name)
//
// Returns a pointer to data marshalled for sending as a start-host packet. The
// buffer is null-terminated.
//
// The format of the packet is:
//
//   1b  packet size (does not account for null terminator).
//   1b  packet type
//   2b  game port
//   1b  name length
//   xb  name data
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> port  The port the game is hosted on.
// --> name  The name of the hosted game.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	dassert(port > 1024);                         // Must have valid port.
	dassert(name.length() <= kNet_GameStrLimit);  // Clamp name limit.

	TheBuffer                                    = new char[6 + name.length()];
	TheBuffer[kNet_Marshalled_Size]              = 5 + name.length();                        // Packet size.
	TheBuffer[kNet_Marshalled_Type]              = kNet_PacketStartHost;                     // Packet type.
	TheBuffer[kNet_MarshalledStartHost_PortLB]   = static_cast<char>(port & 0x00FF);				 // Low byte of port.
	TheBuffer[kNet_MarshalledStartHost_PortHB]   = static_cast<char>((port & 0xFF00) >> 8);	 // High byte of port.
	TheBuffer[kNet_MarshalledStartHost_NameSize] = name.length();                            // Name length.
	strcpy(TheBuffer + kNet_MarshalledStartHost_NameBase,name.c_str());                      // Name data.
	TheBuffer[TheBuffer[0]];                                                                 // Null terminator.
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallRefreshHostPacket(char playercount)
//
// Returns a pointer to data marshalled for sending as a refres-host packet. The
// buffer is null-terminated.
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> port  The port the game is hosted on.
// --> name  The name of the hosted game.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	TheBuffer                                    = new char[4];
	TheBuffer[kNet_Marshalled_Size]              = 3;                        // Packet size.
	TheBuffer[kNet_Marshalled_Type]              = kNet_PacketRefreshHost;                     // Packet type.
	TheBuffer[kNet_MarshalledRefreshHost_PlayerCount] = playercount;                            // Name length.
	TheBuffer[TheBuffer[0]] = '\0';                                                          // Null terminator.
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallListHostPacket(void)
//
// Returns a pointer to data marshalled for sending as a list-host packet. The
// buffer is null-terminated.
//
// The packet has no payload.
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> port  The port the game is hosted on.
// --> name  The name of the hosted game.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	TheBuffer                       = new char[2];
	TheBuffer[kNet_Marshalled_Size] = 2;                    // Packet size.
	TheBuffer[kNet_Marshalled_Type] = kNet_PacketListHost;  // Packet type.
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallDropHostPacket(void)
//
// Returns a pointer to data marshalled for sending as a drop-host packet. The
// buffer is not null-terminated.
//
// The packet has no payload.
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> port  The port the game is hosted on.
// --> name  The name of the hosted game.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	TheBuffer                       = new char[2];
	TheBuffer[kNet_Marshalled_Size] = 2;                    // Packet size.
	TheBuffer[kNet_Marshalled_Type] = kNet_PacketStopHost;  // Packet type.
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallChatPacket(string chat)
//
// Returns a pointer to data marshalled for sending as a chat packet. The buffer
// is null-terminated.
//
// The format of the packet is:
//
//   1b  packet size (does not account for null terminator).
//   1b  packet type
//   1b  message length
//   xb  message data
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> chat  The chat message.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	dassert(chat.length() <= kNet_ChatStrLimit);  // Clamp chat limit.

	TheBuffer = new char[4 + chat.length()];
	TheBuffer[kNet_Marshalled_Size] = 3 + chat.length();           // Packet size.
	TheBuffer[kNet_Marshalled_Type] = kNet_PacketChat;             // Packet type.
	TheBuffer[kNet_MarshalledChat_ChatSize] = chat.length();       // Chat length.
	strcpy(TheBuffer + kNet_MarshalledChat_ChatBase,chat.c_str()); // Chat data.
	TheBuffer[TheBuffer[0]];                                       // Null terminator.
	return (TheBuffer);
}



// _____________________________________________________________________________
char* NetMarshallTurnPacket(unsigned char power,unsigned char angle,unsigned char weapon,unsigned char seed)
//
// Returns a pointer to data marshalled for sending as a game turn packet. The
// buffer is null-terminated.
//
// The format of the packet is:
//
//   1b  packet size (does not account for null terminator).
//   1b  packet type
//   1b  shot power
//   1b  shot angle
//   1b  weapon
//   1b  seed value (for random calculations)
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> power   The shot power.
// --> angle   The shot angle.
// --> weapon  The weapon fired.
// --> seed    The seed value for random calculations.
// -R-         Unclaimed pointer to a buffer marshalled as a turn packet.
//
{
char *TheBuffer;

//	dassert();  // ?

	TheBuffer = new char[7];
	TheBuffer[kNet_Marshalled_Size]       = 7;                // Packet size.
	TheBuffer[kNet_Marshalled_Type]       = kNet_PacketTurn;  // Packet type.
	TheBuffer[kNet_MarshalledTurn_Power]  = power;            // Power.
	TheBuffer[kNet_MarshalledTurn_Angle]  = angle;            // Angle.
	TheBuffer[kNet_MarshalledTurn_Weapon] = weapon;           // Weapon.
	TheBuffer[kNet_MarshalledTurn_Seed]   = seed;             // Random value.
	TheBuffer[TheBuffer[0]];                                  // Null terminator.
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallStartPacket(char seed,char whoami,vector<string> &names)
//
// Returns a pointer to data marshalled for sending as a game start packet. The
// buffer is null-terminated.
//
// The format of the packet is:
//
//   1b  packet size (does not account for null terminator).
//   1b  packet type
//   1b  base random value
//   1b  player id
//   1b  player count
//	 
//   for each player:
//       1b  name length
//       xb  name data
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> whoami  Specifies which name in the names list is associated with the
//             target player.
// --> seed    The seed value for random calculations.
// -R-         Unclaimed pointer to a buffer marshalled as a start packet.
//
{
char *TheBuffer;
int   TheOffset;
int   TheSize;

	// Calculate packet size.
	TheSize = 5;
	for(int i = 0; i < names.size(); ++i)
	{
		TheSize += names[i].length();
		TheSize += 1;
	}

	TheBuffer = new char[TheSize + 1];
	TheBuffer[kNet_Marshalled_Size]             = TheSize;           // Packet size.
	TheBuffer[kNet_Marshalled_Type]             = kNet_PacketStart;  // Packet type.
	TheBuffer[kNet_MarshalledStart_Seed]        = seed;              // Random value.
	TheBuffer[kNet_MarshalledStart_WhoAmI]      = whoami;
	TheBuffer[kNet_MarshalledStart_PlayerCount] = names.size();      // Player count.

	// Pack name data.
	TheOffset = kNet_MarshalledStart_PlayerBase;
	for(int j = 0; j < names.size(); ++j)
	{
		TheBuffer[TheOffset++] = names[j].length();
		strcpy(TheBuffer + TheOffset,names[j].c_str());
		TheOffset += names[j].length();
	}

	TheBuffer[TheBuffer[0]] = '\0';                                   // Null terminator.
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallDropPlayerPacket(char id)
//
// Returns a pointer to data marshalled for sending as a drop-client packet. The
// buffer is null-terminated.
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> port  The port the game is hosted on.
// --> name  The name of the hosted game.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	TheBuffer                       = new char[3];
	TheBuffer[kNet_Marshalled_Size] = 3;                    // Packet size.
	TheBuffer[kNet_Marshalled_Type] = kNet_PacketDropPlayer;  // Packet type.
	TheBuffer[kNet_MarshalledDropPlayer_ID] = id;
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallUpdatePlayerPacket(char id,char angle)
//
// Returns a pointer to data marshalled for sending as a update-client packet. The
// buffer is null-terminated.
//
// This function relinquishes ownership of the returned buffer; it is thus the
// responsibility of the calling function to delete[] the buffer.
//
// --> port  The port the game is hosted on.
// --> name  The name of the hosted game.
// -R-       Unclaimed pointer to a buffer marshalled as a join packet.
//
{
char *TheBuffer;

	TheBuffer                       = new char[4];
	TheBuffer[kNet_Marshalled_Size] = 4;                    // Packet size.
	TheBuffer[kNet_Marshalled_Type] = kNet_PacketUpdatePlayer;  // Packet type.
	TheBuffer[kNet_MarshalledUpdatePlayer_ID] = id;
	TheBuffer[kNet_MarshalledUpdatePlayer_Angle] = angle;
	return (TheBuffer);
}


// _____________________________________________________________________________
char* NetMarshallReadyPacket(void)
{
char *TheBuffer;

	TheBuffer                       = new char[2];
	TheBuffer[kNet_Marshalled_Size] = 2;                    // Packet size.
	TheBuffer[kNet_Marshalled_Type] = kNet_PacketReady;  // Packet type.
	return (TheBuffer);
}




// _____________________________________________________________________________
int NetUnmarshallJoinPacket(const char *data,char *packSize,char *nameLength,string *name)
//
// Extracts join packet data.
//
// If the packet data does not constitute a valid join packet, this function 
// returns kNetErr_Failure and the output parameters are untouched. Otherwise,
// they are filled out with appropriate data from the packet.
//
// If any of the output parameters are NULL, they are not filled out.
//
// --> data        Pointer to packet data.
// <-- packSize    Length of the entire packet; can be NULL.
// <-- nameLength  Length of the player name; can be NULL.
// <-- name        Name of the player; can be NULL.
// -R-             General purpose error code.
//
{
char  TheBuffer[256];

	if(NetGetPacketType(data) == kNet_PacketJoin)
	{
		if(packSize != NULL)
		{
			*packSize = data[kNet_Marshalled_Size];
		}
		if(nameLength != NULL)
		{
			*nameLength = data[kNet_MarshalledJoin_NameSize];
		}
		if(name != NULL)
		{
			memcpy(TheBuffer,(data + kNet_MarshalledJoin_NameBase),data[kNet_MarshalledJoin_NameSize]);
			TheBuffer[data[kNet_MarshalledJoin_NameSize]] = '\0';
			*name = TheBuffer;
		}
		return (kNetErr_Success);
	}
	else
	{
		return (kNetErr_Failure);
	}
}


// _____________________________________________________________________________
int NetUnmarshallChatPacket(const char *data,char *packSize,char *nameLength,string *name)
//
// Extracts chat packet data.
//
// If the packet data does not constitute a valid chat packet, this function 
// returns kNetErr_Failure and the output parameters are untouched. Otherwise,
// they are filled out with appropriate data from the packet.
//
// If any of the output parameters are NULL, they are not filled out.
//
// --> data        Pointer to packet data.
// <-- packSize    Length of the entire packet; can be NULL.
// <-- chatLength  Length of the player name; can be NULL.
// <-- chat        Name of the player; can be NULL.
// -R-             General purpose error code.
//
{
char  TheBuffer[256];

	if(NetGetPacketType(data) == kNet_PacketChat)
	{
		if(packSize != NULL)
		{
			*packSize = data[kNet_Marshalled_Size];
		}
		if(nameLength != NULL)
		{
			*nameLength = data[kNet_MarshalledChat_ChatSize];
		}
		if(name != NULL)
		{
			memcpy(TheBuffer,(data + kNet_MarshalledChat_ChatBase),data[kNet_MarshalledChat_ChatSize]);
			TheBuffer[data[kNet_MarshalledChat_ChatSize]] = '\0';
			*name = TheBuffer;
		}
		return (kNetErr_Success);
	}
	else
	{
		return (kNetErr_Failure);
	}
}


// _____________________________________________________________________________
int NetUnmarshallTurnPacket(const char *data,char *packSize,unsigned char *power,unsigned char *angle,unsigned char *weapon,unsigned char *seed)
//
// Extracts turn packet data.
//
// If the packet data does not constitute a valid turn packet, this function 
// returns kNetErr_Failure and the output parameters are untouched. Otherwise,
// they are filled out with appropriate data from the packet.
//
// If any of the output parameters are NULL, they are not filled out.
//
// --> data      Pointer to packet data.
// <-- packSize  Length of the entire packet; can be NULL.
// <-- power     Power of the shot; can be NULL.
// <-- angle     Angle of the shot; can be NULL.
// <-- weapon    Weapon fired; can be NULL.
// <-- seed      Random value; can be NULL.
// -R-           General purpose error code.
//
{
	if(NetGetPacketType(data) == kNet_PacketTurn)
	{
		if(packSize != NULL)
		{
			*packSize = data[kNet_Marshalled_Size];
		}
		if(power != NULL)
		{
			*power = data[kNet_MarshalledTurn_Power];
		}
		if(angle != NULL)
		{
			*angle = data[kNet_MarshalledTurn_Angle];
		}
		if(weapon != NULL)
		{
			*weapon = data[kNet_MarshalledTurn_Weapon];
		}
		if(seed != NULL)
		{
			*seed = data[kNet_MarshalledTurn_Seed];
		}
		return (kNetErr_Success);
	}
	else
	{
		return (kNetErr_Failure);
	}
}


// _____________________________________________________________________________
int NetUnmarshallStartPacket(const char *data,char *packSize,char *seed,char *whoami,char *pcount,vector<string> *names)
//
// Extracts start packet data.
//
// If the packet data does not constitute a valid start packet, this function 
// returns kNetErr_Failure and the output parameters are untouched. Otherwise,
// they are filled out with appropriate data from the packet.
//
// If any of the output parameters are NULL, they are not filled out.
//
// --> data      Pointer to packet data.
// <-- packSize  Length of the entire packet; can be NULL.
// <-- seed      Random value; can be NULL.
// -R-           General purpose error code.
//
{
	if(NetGetPacketType(data) == kNet_PacketStart)
	{
		if(packSize != NULL)
		{
			*packSize = data[kNet_Marshalled_Size];
		}
		if(seed != NULL)
		{
			*seed = data[kNet_MarshalledStart_Seed];
		}
		if(whoami != NULL)
		{
			*whoami = data[kNet_MarshalledStart_WhoAmI];
		}
		if(pcount != NULL)
		{
			*pcount = data[kNet_MarshalledStart_PlayerCount];
		}
		if(names != NULL)
		{
		int  TheOffset;

			names->erase(names->begin(),names->end());
			TheOffset = kNet_MarshalledStart_PlayerBase;
			for(int i = 0; i < data[kNet_MarshalledStart_PlayerCount]; ++i)
			{
			int   TheSize;
			char  TheBuffer[kNet_NameStrLimit];

				TheSize = data[TheOffset++];
				memcpy(TheBuffer,data + TheOffset,TheSize);
				TheBuffer[TheSize] = '\0';
				names->push_back(TheBuffer);
				TheOffset += TheSize;
			}
		}
		return (kNetErr_Success);
	}
	else
	{
		return (kNetErr_Failure);
	}
}


// _____________________________________________________________________________
int NetUnmarshallDropPlayerPacket(const char *data,char *packSize,char *id)
//
// Extracts turn packet data.
//
// If the packet data does not constitute a valid turn packet, this function 
// returns kNetErr_Failure and the output parameters are untouched. Otherwise,
// they are filled out with appropriate data from the packet.
//
// If any of the output parameters are NULL, they are not filled out.
//
// --> data      Pointer to packet data.
// <-- packSize  Length of the entire packet; can be NULL.
// <-- id        Can be NULL.

// -R-           General purpose error code.
//
{
	if(NetGetPacketType(data) == kNet_PacketTurn)
	{
		if(packSize != NULL)
		{
			*packSize = data[kNet_Marshalled_Size];
		}
		if(id != NULL)
		{
			*id = data[kNet_MarshalledDropPlayer_ID];
		}
		return (kNetErr_Success);
	}
	else
	{
		return (kNetErr_Failure);
	}
}


// _____________________________________________________________________________
int NetUnmarshallUpdatePlayerPacket(const char *data,char *packSize,char *id,char *angle)
//
// Extracts update packet data.
//
// If the packet data does not constitute a valid turn packet, this function 
// returns kNetErr_Failure and the output parameters are untouched. Otherwise,
// they are filled out with appropriate data from the packet.
//
// If any of the output parameters are NULL, they are not filled out.
//
// --> data      Pointer to packet data.
// <-- packSize  Length of the entire packet; can be NULL.
// <-- id        Can be NULL.

// -R-           General purpose error code.
//
{
	if(NetGetPacketType(data) == kNet_PacketUpdatePlayer)
	{
		if(packSize != NULL)
		{
			*packSize = data[kNet_Marshalled_Size];
		}
		if(id != NULL)
		{
			*id = data[kNet_MarshalledUpdatePlayer_ID];
		}
		if(angle != NULL)
		{
			*angle = data[kNet_MarshalledUpdatePlayer_Angle];
		}
		return (kNetErr_Success);
	}
	else
	{
		return (kNetErr_Failure);
	}
}


// _____________________________________________________________________________
bool NetHosting(void)
//
// Returns true if the local machine is the game host.
//
// -R-  True if the local machine is the game host; false otherwise.
//
{
	return (iIsHost);
}


// _____________________________________________________________________________
bool NetJoined(void)
//
// Returns true if the local machine has joined a game.
//
// -R-  True if the local machine has joined a game; false otherwise.
//
{
	return (iIsClient);
}


// _____________________________________________________________________________
bool NetPlaying(void)
//
// Returns true if the network game is in progress.
//
// -R-  True if the network game is in progress.
//
{
	return (iIsPlaying);
}


// _____________________________________________________________________________
string NetGetIPFromDNS(const char *dns)
//
// Returns the IP (as a string) associated with the DNS name (or IP) pointed to
// by `dns`.
//
// -R-  The IP of the local machine.
//
{
hostent *TheHostInfo;
string   TheResult;

	if((TheHostInfo = gethostbyname(dns)) == NULL)
 	{
 		errorDisplayAndExit(ErrorCode::Unknown);
  }

  TheResult = inet_ntoa(*((struct in_addr *)TheHostInfo->h_addr));
  return (TheResult);
}


// _____________________________________________________________________________
string NetGetLocalIP(void)
//
// Returns the IP of the local machine as a string.
//
// -R-  The IP of the local machine.
//
{
hostent *TheHostInfo;
char     TheBuffer[256];
string   TheResult;

	gethostname(TheBuffer,256);
	TheHostInfo = gethostbyname(TheBuffer);
	TheResult   = inet_ntoa(*((in_addr *)TheHostInfo->h_addr));
	return (TheResult);
}


// _____________________________________________________________________________
int NetGetPeerCount(void)
//
// Returns the number of connected peers.
// This will return 0 if the machine is not hosting the game.
//
// -R-  The peer count.
//
{
	return ((NetHosting() ? iHostData.peerCount : 0));
}


// _____________________________________________________________________________
StNet_Peer NetGetPeerInfo(int peer)
//
// Returns a copy of the peer info associated with the specified peer ID.
//
// -->  Desired peer ID.
// -R-  The peer info.
//
{
	dassert(peer >= 0 && peer < iHostData.peerCount);  // Must be in valid range.
	return (iHostData.peer[peer]);
}


// _____________________________________________________________________________
bool NetSocketIsMeta(SOCKET sock)
//
// Returns true if `sock` is the metaserver socket.
//
// --> sock  Socket to query.
// -R-       True if the packet is the metaserver socket.
//
{
	return ((NetHosting() ? (sock == iHostData.metaSock) : (sock == iClientData.metaSock)));
}


// _____________________________________________________________________________
bool NetPacketIsBroadcast(char *packet)
//
// Returns true if `packet` is a broadcast packet (i.e., one that is broadcast
// from the host machine.) Broadcast packets are identified by their packet
// type code, which is negative if the packet is a broadcast packet and positive
// otherwise.
//
// --> packet  Packet to query.
// -R-         True if the packet is a broadcast packet.
//
{
	return (packet[kNet_Marshalled_Type] < 0);
}


void NetCfgMetaInfo(string name,int port)
{
	iHostData.cfgMetaName = name;
	iHostData.cfgMetaPort = port;
}






// _____________________________________________________________________________
void NetClientIsReady(void)
//
//
{
	dcon << "[CLIENT]\tSending ready packet...\n";
	NetSendData(iClientData.serverSock,NetMarshallReadyPacket());
}



void NetHostSetReady(WPARAM sock)
{
	if(NetHosting())
	{
		for(int i = 0; i < kNet_ClientsMax; ++i)
		{
			if(iHostData.peer[i].valid && sock == iHostData.peer[i].sock)
			{
				dcon << "Peer " << i << " is ready!\n";
				iHostData.peer[i].ready = true;
			}
		}
	}

	for(int j = 0; j < kNet_ClientsMax; ++j)
	{
		if(!iHostData.peer[j].ready && iHostData.peer[j].valid)
		{
			dcon << "But can't go yet!\n";
			return;
		}
	}

	dcon << "Going now!\n";
	NetHostGameStart();
}



bool NetAllowClientTurnIfAllClientsReady(WPARAM sock)
{
bool gogogo = true;

	if(NetHosting())
	{
		for(int i = 0; i < kNet_ClientsMax; ++i)
		{
			if(iHostData.peer[i].valid && sock == iHostData.peer[i].sock)
			{
				dcon << "Peer " << i << " is ready!\n";
				iHostData.peer[i].ready = true;
			}
		}

		for(int ij = 0; ij < kNet_ClientsMax; ++ij)
		{
			if(iHostData.peer[ij].valid && !iHostData.peer[ij].ready)
			{
				dcon << "Peer " << ij << " is not ready!\n";
				gogogo = false;
			}
		}

		if(gogogo)
		{
			int t = iTurn+1;
			t = t >= iTankCount ? 0 : t;
			dcon << "Next cycle...\n";

			// -jpetrie  Assuming iTurn is set to the correct (turn to go after ready) index.

			//NetHostBroadcast(NetMarshallReadyPacket());

			char *dat = NetMarshallReadyPacket();
			dat[1] = -dat[1]; // yeah, you like that, huh?
			NetSendData(iHostData.peer[iTurn].sock,dat);
		}
		else
		{
			dcon << "Waiting for slow machines...\n";
		}
	}

	return gogogo;
}




void NetResetAllState(void)
{
	iIsHost = false;
	iIsClient = false;
	iIsPlaying = false;

	delete[] iTank;
	iTankCount =0;
	launch = false;

	NetShutdown();
	NetStartup();


	// La la la hack.
	first = false;
	started = false;
	allowNextTurn = true;
	iInPostMortem = false;
	gItemCtr = 0;
	gWeapCtr = 0;

	thinking = false;
	gDeadList.clear();
	vBlast.clear();
	goToPostMortemASAP = false;
	singlePlayer =false;
	
	for(int i = 0; i < SCREEN_WIDTH; ++i)
	{
		
		iSurfaceMap[i].isBroken = false;
		iSurfaceMap[i].yTop = 0;
		iSurfaceMap[i].yMid = 0;
		iSurfaceMap[i].yLow = 0;
	}

	Angle = 90;
	Power = 50;
	gInitialCash = kInitialCash;

	cClick = 0;
	mClick = 0;
	cWeapClick = 0;
	mWeapClick = 0;
	mIAClick = 0;
	cIAClick = 0;
	cWAClick = 0;
	mWAClick = 0;

	weapIndx = 0;
	weapAIndx = 2;
	itemIndx = 0;
	itemAIndx = 0;
	joinIndx = 0;

	dfloaters.clear();
	vec_chatList.clear();
}

