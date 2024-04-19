// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================

// -----------------------------------------------------------------------------
//                                                                         guard
// -----------------------------------------------------------------------------

#ifndef _TANK_MOD_NETWORK_H_
#define _TANK_MOD_NETWORK_H_

#pragma warning (disable:4786)
// -----------------------------------------------------------------------------
//                                                                      includes
// -----------------------------------------------------------------------------

// tank
#include "tank.h"


// -----------------------------------------------------------------------------
//                                                                     constants
// -----------------------------------------------------------------------------

// string max
const int  kNet_NameStrLimit = 16;
const int  kNet_GameStrLimit = 100;
const int  kNet_ChatStrLimit = 250;

// client max
const int  kNet_ClientsMax     = 8;
const int  kNet_ConnectBacklog = 10;

// lifetimes
const int  kNet_PendingLifetime = 10000;  // Gameloops.
const int  kNet_PeerLifetime    = 10000;  // Gameloops.
const int  kNet_ClientPingDelay = kNet_PeerLifetime / 2;
const int  kNet_HostPingDelay   = 200;

// ports
const int  kNet_DefaultMetaPort = 2807;
const int  kNet_DefaultGamePort = 2808;

// packet codes
const char  kNet_PacketJoin         = 1;  // Join game packet.
const char  kNet_PacketDrop         = 2;  // Leave game packet
const char  kNet_PacketStartHost    = 3;  // Start hosting packet.  (sent to metaserver)
const char  kNet_PacketStopHost     = 4;  // Stop hosting packet.   (sent to metaserver)
const char  kNet_PacketRefreshHost  = 5;  // Update hosting packet. (sent to metaserver)
const char  kNet_PacketListHost     = 6;  // Fetch host list.       (sent to metaserver)
const char  kNet_PacketStart        = 7;  // Game start packet.
const char  kNet_PacketChat         = 8;  // Game chat packet.
const char  kNet_PacketTurn         = 9;  // Game turn packet.
const char  kNet_PacketPing         = 10; // Connection refresh packet.
const char  kNet_PacketDropPlayer   = 11; // Drop player packet.
const char  kNet_PacketUpdatePlayer = 12; // Update player (angle, etc.).
const char  kNet_PacketReady        = 13; // 

// packet format codes
const char  kNet_Marshalled_Size = 0;
const char  kNet_Marshalled_Type = 1;

const char  kNet_MarshalledJoin_NameSize = 2;
const char  kNet_MarshalledJoin_NameBase = 3;

const char  kNet_MarshalledStartHost_PortLB   = 2;  // Low byte of port word.
const char  kNet_MarshalledStartHost_PortHB   = 3;  // High byte of port word.
const char  kNet_MarshalledStartHost_NameSize = 4;
const char  kNet_MarshalledStartHost_NameBase = 5;

const char  kNet_MarshalledRefreshHost_PlayerCount = 2;

const char  kNet_MarshalledChat_ChatSize = 2;
const char  kNet_MarshalledChat_ChatBase = 3;

const char  kNet_MarshalledTurn_Power  = 2;
const char  kNet_MarshalledTurn_Angle  = 3;
const char  kNet_MarshalledTurn_Weapon = 4;
const char  kNet_MarshalledTurn_Seed   = 5;

const char  kNet_MarshalledStart_Seed        = 2;
const char  kNet_MarshalledStart_WhoAmI      = 3;
const char  kNet_MarshalledStart_PlayerCount = 4;
const char  kNet_MarshalledStart_PlayerBase  = 5;

const char  kNet_MarshalledDropPlayer_ID = 2;

const char  kNet_MarshalledUpdatePlayer_ID = 2;
const char  kNet_MarshalledUpdatePlayer_Angle = 3;


// special packet data codes
const unsigned char  kNet_TurnDead = 0xFF; // POWER value of dead tank's turn packet.
const unsigned char  kNet_TurnItem = 0xFF; // ANGLE value of tank using the item; item code in WEAPON value.
const unsigned char  kNet_TurnKill = 0xFF; // ANGLE value to kill a tank remotely.

// error codes
const int  kNetErr_Failure         = -1;  // General purpose failure.
const int  kNetErr_Success         = 0;   // General purpose success.
const int  kNetErr_GameIsFull      = 1;   // Game is full.
const int  kNetErr_AlreadyInGame   = 2;   // Machine is already in or hosting a game.
const int  kNetErr_CouldNotSocket  = 32;  // Could not create socket.
const int  kNetErr_CouldNotBind    = 33;  // Could not bind socket.
const int  kNetErr_CouldNotSelect  = 34;  // Could not select socket in async mode.
const int  kNetErr_CouldNotListen  = 35;  // Could not listen on socket.
const int  kNetErr_CouldNotConnect = 36;  // Could not connect on socket.
const int  kNetErr_CouldNotAccept  = 37;  // Could not accept on a socket.


// -----------------------------------------------------------------------------
//                                                                       structs
// -----------------------------------------------------------------------------

// _____________________________________________________________________________
struct StNet_Peer
//
// Encapsulates data about a peer (game client) connection.
//
{
	string  address;   // Address of the peer.
	int     port;      // Port the socket is attached to.
	SOCKET  sock;      // Socket the peer connection communicates on.

	int     order;     // In-game position in turn order.
	string  name;      // In-game player name.

	bool    valid;     // True if the peer has been added to the game.
	int     lifetime;  // Current lifetime delay of the connection.

	bool    ready;     // True if peer is ready.
};


// _____________________________________________________________________________
struct StNet_HostState
//
// Encapsulates networking state information for the host.
//
{
	SOCKADDR_IN  metaAddr;    // Address of metaserver.
	SOCKET       metaSock;    // Socket to metaserver.
	SOCKADDR_IN  listenAddr;  // Listen address.
	SOCKET       listenSock;  // Listen socket.

	string      hostedGameName;

	StNet_Peer  peer[kNet_ClientsMax];  // List of connections.
	int         peerCount;              // Count of connections.
	int         validPeerCount;         // Count of players.

	int         timeSincePing;  // Metaserver ping counter.

	string cfgMetaName;
	int    cfgMetaPort;
};


// _____________________________________________________________________________
struct StNet_ClientState
//
// Encapsulates networking state information for the client.
//
{
	SOCKADDR_IN  metaAddr;    // Address of metaserver.
	SOCKET       metaSock;    // Socket to metaserver.
	SOCKADDR_IN  serverAddr;  // Server address.
	SOCKET       serverSock;  // Server socket.

	int          timeSincePing;  // Host ping counter.
};


// -----------------------------------------------------------------------------
//                                                                    prototypes
// -----------------------------------------------------------------------------

// startup / shutdown
void NetStartup(void);
void NetShutdown(void);

// idle time processing
void NetIdle(void);

// hosting
int NetHostGame(int port,string name);
int NetHostGameUpdate(char playercount);
int NetHostGameStart(void);
int NetHostAddPending(SOCKET sock);
int NetHostAddPeer(SOCKET sock,char *packet);
int NetHostBroadcast(char *packet);

int NetHostAcceptPing(SOCKET sock);


// joining
int NetJoinGame(string address,int port);

// client
int NetClientPing(void);
int NetClientChat(string chat);
int NetClientTurn(unsigned char power,unsigned char angle,unsigned char weapon,unsigned char seed);
int NetClientTurn_Dead(void);
int NetClientTurn_Item(unsigned char item);
int NetClientUpdate(char id,char ang);

int NetClientMSList(void);

// sending / receiving
int NetSendData(SOCKET s,char *data);
int NetRecvData(SOCKET s,char *data);

// packets
int   NetGetPacketType(const char *data);

char* NetMarshallJoinPacket(string name);
char* NetMarshallStartHostPacket(short port,string name);
char* NetMarshallRefreshHostPacket(char playercount);
char* NetMarshallDropHostPacket(void);
char* NetMarshallListHostPacket(void);
char* NetMarshallChatPacket(string message);
char* NetMarshallTurnPacket(unsigned char power,unsigned char angle,unsigned char weapon,unsigned char seed);
char* NetMarshallStartPacket(char seed,char whoami,vector<string> &names);
char* NetMarshallDropPlayerPacket(char id);
char* NetMarshallUpdatePlayerPacket(char id,char angle);
char* NetMarshallReadyPacket(void);

int   NetUnmarshallJoinPacket(const char *data,char *packSize,char *nameLength,string *name);
int   NetUnmarshallChatPacket(const char *data,char *packSize,char *chatLength,string *chat);
int   NetUnmarshallTurnPacket(const char *data,char *packSize,unsigned char *power,unsigned char *angle,unsigned char *weapon,unsigned char *seed);
int   NetUnmarshallStartPacket(const char *data,char *packSize,char *seed,char *whoami,char *pcount,vector<string> *pnames);
int   NetUnmarshallDropPlayerPacket(const char *data,char *packSize,char *id);
int   NetUnmarshallUpdatePlayerPacket(const char *data,char *packSize,char *id,char *angle);

// tools
bool   NetHosting(void);
bool   NetJoined(void);
bool   NetPlaying(void);

string     NetGetIPFromDNS(const char *dns);
string     NetGetLocalIP(void);

int        NetGetPeerCount(void);
StNet_Peer NetGetPeerInfo(int peer);

bool  NetSocketIsMeta(SOCKET sock);
bool  NetPacketIsBroadcast(char *packet);

void  NetResetAllState(void);

void NetCfgMetaInfo(string name,int port);


void NetClientIsReady(void);
void NetHostSetReady(WPARAM sock);
bool NetAllowClientTurnIfAllClientsReady(WPARAM sock);

#endif

