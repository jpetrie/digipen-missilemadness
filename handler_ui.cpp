// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// 
//	owned by: Jesica McCrory
// =============================================================================



#include "tank.h"

#include "mod_network.h"
#include "mod_display.h"
#include "mod_game.h"
#include "mod_ui.h"
#include "mod_econ.h"
#include "handler_network.h"
#include "handler_redraw.h"
#include "handler_ui.h"

#include "error_handling.h"

#include "debugging.h"
#include <cmath>
// =============================================================================
//	function prototypes
// =============================================================================

//static int StrLen(const char *str);
static void SourceCheck(InputSource is);
static void EnterInput(void);

static void AngleCheck (HWND wind, int x,  int y);

static void PowerCheck (HWND wind,int x, int y);
    
static void WeaponCheck (HWND wind,int x, int y);
static void FireCheck (HWND wind,int x, int y);

static void ItemCheck (HWND wind,int x, int y);
static void UseCheck (HWND wind,int x, int y);

static void ChatCheck ();
static void ChatScrollCheck (HWND wind,int x, int y);
static void SendCheck ();



// =============================================================================
//	shared variables
// =============================================================================

bool inputSelected = false;
char *ChatBuffer;
char *InputBuffer;
bool buffer1Full = false;

//int zero = 0;	// keep track of which vector is in the zero place
int indx = 0;
int ctr = 0;

InputSource inputSource;
extern bool singlePlayer;
extern bool started;
extern bool iTankUsedItem;

// from mod_econ.cpp
extern int gInitialWeap[MAX_WEAPONS];
extern int gInitialItem[MAX_ITEMS];
extern int gItemCtr;
extern int gWeapCtr;

// =============================================================================
//	function definitions
// =============================================================================
int cClick = 0;
int mClick = 0;
int cWeapClick = 0;
volatile int mWeapClick = 0;
int mIAClick = 0;
int cIAClick = 0;
int cWAClick = 0;
int mWAClick = 0;

void HandleMouseUI(HWND wind, WPARAM wParam,LPARAM lParam)
{
	int x,y;
	x=LOWORD(lParam);
	y=HIWORD(lParam);

	switch (gCurrentScreen)
	{ 
		case Title:
		{
			if (x > k_TitleX && x < k_TitleX2)
			{
				
				if (y > k_TitleOptionsY && y <	k_TitleOptionsY2)
					PostQuitMessage(0); // quit game
				else if (y > k_TitleJoinY && y < k_TitleJoinY2)
					gCurrentScreen = JoinGameSetup;
				else if (y > k_TitleHostY && y < k_TitleHostY2)
					gCurrentScreen = HostGameSetup;
				else if (y > k_Title1PlayerY && y < k_Title1PlayerY2)
				{	
					singlePlayer = true;
					gCurrentScreen = PreBattle;
					PurchaseWeapon(1);
					NetHostGame(2808, "127.0.0.1");
				}
			}
		//	else if (x > k_TitleX && y > 0 && x < SCREEN_WIDTH - k_TitleX && y < baseY * 5)
		//		gCurrentScreen = Credits;
		}
			
		break;


		case HostGameSetup:
		{
			if (x > k_portX1 && y > k_portY1 && x < k_portX2 && y < k_portY2)
			{
				if (inputSource != DEFAULT)
					EnterInput();

				if (!inputSelected)
				{
					inputSource = PORT;
					SourceCheck(inputSource);
				}
												
			}
			else if (x > k_gameNameX1 && y > k_gameNameY1 && x < k_gameNameX2 && y < k_gameNameY2)
			{
				if (inputSource != DEFAULT)
					EnterInput();

				if (!inputSelected)
				{
					inputSource = GAMENAME;
					SourceCheck(inputSource);
				}

			}
			
			else if (x > k_playerNameX1 && y > k_playerNameY1 && x < k_playerNameX2 && y < k_playerNameY2)
			{
				if (inputSource != DEFAULT)
					EnterInput();
				if (!inputSelected)
				{
					inputSource = PLAYERNAME;
					SourceCheck(inputSource);					
				}
			}
			else if (x > k_GeneralRtX && y > k_GeneralY && x < k_GeneralRtX2 && y < k_GeneralY2)
			{
				if (inputSelected)
				{
					EnterInput();
				}
				portUI = defaultPort;
				nameUI = defaultGame;
				numPlayers = maxPlayers;
				maxRounds = defaultRounds;
				playName = defaultName;
				
				errorUI = false;
				
				// jpetrie  Added, I think this is where I need to reset network.
				NetResetAllState();

				gCurrentScreen = Title;
			}
			else if (x > k_GeneralLftX && y > k_GeneralY && x < k_GeneralLftX2 && y < k_GeneralY2)
			{
				if (inputSelected)
				{
					EnterInput();
				}
				
				if (portUI > 0 && nameUI.size() > 0)
				{
					errorUI = false;
					if (NetHostGame(portUI, nameUI) == kNetErr_Success)
						gCurrentScreen = PreGameSetup;
				}
				else
					errorUI = true;
			}
		}
		break;

		case JoinGameSetup:
		{
			if (x > k_joinNameX1 && y > k_playerNameY1 && x < k_joinNameX2 && y < k_playerNameY2)
			{
				if (inputSource != DEFAULT)
					EnterInput();
				if (!inputSelected)
				{
					inputSource = PLAYERNAME;
					SourceCheck(inputSource);
				}
			}
			else if (x > k_jInfoX1 && y > k_jInfoY1 && x < k_jInfoX2 && y < k_jInfoY2 && inputSource != HOSTLIST)
			{
				if (inputSource != DEFAULT)
					EnterInput();
				inputSource = HOSTLIST;
				inputSelected = true;
			}
			else if (x > k_jInfoX2 - k_scrollArrowH && y > k_jInfoY1 && x < k_jInfoX2 && y < k_jInfoY1 + k_scrollArrowH && vec_hostList.size() != 0)
			{
				// up arrow
				if (joinIndx > 0)
					--joinIndx;
			}
			else if (x > k_jInfoX2 - k_scrollArrowH && y > k_jInfoY2 - k_scrollArrowH && x < k_jInfoX2 && y < k_jInfoY2 && vec_hostList.size() != 0)
			{
				// down arrow
				if (joinIndx < vec_hostList.size()-1)
					++joinIndx;
			}
			else if (x > k_manualIPX1 && y >  k_manualIPY1 && x < k_manualIPX2 && y < k_manualIPY2)
			{
				if (inputSource != DEFAULT)
					EnterInput();
				if (!inputSelected)
				{
					inputSource = IP;
					SourceCheck(inputSource);			
				}
			}
			else if (x > k_manualPortX1 && y >  k_manualPortY1 && x < k_manualPortX2 && y < k_manualPortY2)
			{
				if (inputSource != DEFAULT)
					EnterInput();
				if (!inputSelected)
				{
					inputSource = PORT;
					SourceCheck(inputSource);
				}
			}
			else if (x > k_GeneralRtX && y > k_GeneralY && x < k_GeneralRtX2 && y < k_GeneralY2)
			{
				if (inputSelected)
				{
					EnterInput();
				}
				portUI = defaultPort;
				ipUI = '\0';
				
				gCurrentScreen = Title;

				// jpetrie  Added, I think this is where I need to reset network.
				NetResetAllState();
			}
			else if (x > k_GeneralLftX && y > k_GeneralY && x < k_GeneralLftX2 && y < k_GeneralY2)
			{
				if (inputSelected)
				{
					EnterInput();
				}
				if (portUI > 1024 && ipUI.size() > 0)
				{
					errorUI = false;
					if (NetJoinGame(ipUI, portUI) == kNetErr_Success)
						gCurrentScreen = PreGameSetup;
				}
				else
					errorUI = true;
			}
		}
		break;

		case PreGameSetup:
		{
			if (x > k_preChatX && y > k_preChatInY1 && x < k_preChatX + k_chatW && y < k_preChatInY2)
				ChatCheck();
			else if (x > (k_preChatX + k_chatW - k_scrollArrowH) && x < (k_preChatX + k_chatW))
				ChatScrollCheck(wind, x, y);
			else if (x > k_GeneralRtX && y > k_GeneralY && x < k_GeneralRtX2 && y < k_GeneralY2)
			{
				// jpetrie  Added, I think this is where I need to reset network.
				NetResetAllState();
				ipUI = '\0';
				gCurrentScreen = Title;
			}
			else if (x > k_GeneralLftX && x < k_GeneralLftX2 && y > k_GeneralY && y < k_GeneralY2)
			{
				vec_chatList.clear();
				gCurrentScreen = PreBattle;
				PurchaseWeapon(1);
			}
		}
		break;

		case PreBattle:
		{
			if(x > k_weapAvailX1 && y > k_weapAvailY1 && x < k_weapAvailX2 - k_scrollArrowH && y < k_weapAvailY2)
			{
				inputSource = WEAPON_A;
				inputSelected = true;
				mWAClick = (y - k_weapAvailY1)/k_scrollArrowH; //index into text display 0, 1 or 2
				
				weapAIndx = weapAIndx - cWAClick + mWAClick;
				cWAClick = mWAClick;
			}
			else if (x > k_weapAvailX2 - k_scrollArrowH && y > k_weapAvailY1 && x < k_weapAvailX2 && y < k_weapAvailY1+k_scrollArrowH)
			{		
				if (inputSelected)
				{							
					// up
					if (weapAIndx > 2)
					{
						--weapAIndx;				
						if (cWAClick > 0)
						{
							--cWAClick;
							--mWAClick;
						}
					}
				}
				inputSelected = true;
				inputSource = WEAPON_A;
			}
			else if(x > k_weapAvailX2 - k_scrollArrowH && y > k_weapAvailY2 - k_scrollArrowH && x < k_weapAvailX2 && y < k_weapAvailY2)
			{
				if (inputSelected)
				{
					if(weapAIndx < 20 -1)
					{
						++weapAIndx;
						if (cWAClick < (numDispVecPBattle-1))
						{
							++cWAClick;
							++mWAClick;
						}
					}
				}
				inputSelected = true;
				inputSource = WEAPON_A;
			}
			
			else if (x > k_itemAvailX1 && y > k_itemAvailY1 && x < (k_itemAvailX2 - k_scrollArrowH) && y < k_itemAvailY2)
			{
				inputSource = ITEM_A;
				inputSelected = true;

				mIAClick = (y - k_itemAvailY1)/k_scrollArrowH; //index into text display 0, 1 or 2
				
				
				itemAIndx = itemAIndx - cIAClick + mIAClick;
				cIAClick = mIAClick;
			}
			else if (x > k_itemAvailX2 - k_scrollArrowH && y > k_itemAvailY1 && x < k_itemAvailX2 && y < k_itemAvailY1+k_scrollArrowH)
			{	
				if (inputSelected)
				{
					if (itemAIndx > 0)
					{
						--itemAIndx;
						if (cIAClick > 0)
						{
							--cIAClick;
							--mIAClick;
						}
					}
				}	
				inputSource = ITEM_A;
				inputSelected = true;
			}
			else if (x > k_itemAvailX2 - k_scrollArrowH && y > k_itemAvailY2 - k_scrollArrowH && x < k_itemAvailX2 && y < k_itemAvailY2 && cIAClick < (iItemList.size()-1))
			{		
				if (inputSelected)
				{
					if (itemAIndx < iItemList.size()-1)
					{
						++itemAIndx;
						if (cIAClick < (numDispVecPBattle-1))
						{
							++cIAClick;
							++mIAClick;
						}
					}
				}
				inputSource = ITEM_A;
				inputSelected = true;
			}
			else if (x > k_weapHaveX1 && y > k_weapHaveY1 && x < k_weapHaveX2 && y < k_weapHaveY2)
			{
				inputSource = WEAPON;
				inputSelected = true;
				mWeapClick = (y - k_weapHaveY1)/k_scrollArrowH; //index into text display 0, 1 or 2
				
				weapIndx = weapIndx - cWeapClick + mWeapClick;
				cWeapClick = mWeapClick;
			}
			else if (x > k_weapHaveX2 - k_scrollArrowH && y > k_weapHaveY1 && x < k_weapHaveX2 && y < k_weapHaveY1 + k_scrollArrowH)
			{	
				if (inputSelected)
				{
					if (weapIndx > 0)
				{			
							--weapIndx;
						if (cWeapClick > 0)
						{
							--cWeapClick;
							--mWeapClick;
						}
				}
				}
				inputSelected = true;
				inputSource = WEAPON;
			}
			else if (x > k_weapHaveX2 - k_scrollArrowH && y > k_weapHaveY2 - k_scrollArrowH && x < k_weapHaveX2 && y < k_weapHaveY2)
			{
				if (inputSelected)
			{	
					if (weapIndx < gWeapCtr - 1)
					{
						++weapIndx;
						if (cWeapClick < (numDispVecPBattle-1))
				{	
							++cWeapClick;
							++mWeapClick;
						}
				}
				}
				inputSelected = true;
				inputSource = WEAPON;
			}
			else if (x > k_itemHaveX1 && y > k_itemHaveY1 && x < (k_itemHaveX2 - k_scrollArrowH) && y < k_itemHaveY2)
			{
				inputSource = ITEM;
				inputSelected = true;

				mClick = (y - k_itemHaveY1)/k_scrollArrowH; //index into text display 0, 1 or 2
				
				itemIndx = itemIndx - cClick + mClick;
				cClick = mClick;
			}
			else if (x > k_itemHaveX2 - k_scrollArrowH && y > k_itemHaveY1 && x < k_itemHaveX2 && y < k_itemHaveY1+k_scrollArrowH)
			{
				if (inputSelected)
				{
					if (itemIndx > 0)
					{
						--itemIndx;
						if (cClick > 0)
						{
							--cClick;
							--mClick;
						}
					}
				}
				inputSource = ITEM;
				inputSelected = true;
				}
			else if (x > k_itemHaveX2 - k_scrollArrowH && y > k_itemHaveY2 - k_scrollArrowH && x < k_itemHaveX2 && y < k_itemHaveY2)
			{
				if (inputSelected)
				{
					if (itemIndx < gItemCtr - 1)
					{
						++itemIndx;
						if (cClick < (numDispVecPBattle-1))
						{
							++cClick;
							++mClick;
						}
					}
				}
				inputSelected = true;
				inputSource = ITEM;
			}
			else if (x > k_GeneralRtX && y > k_GeneralY && x < k_GeneralRtX2 && y < k_GeneralY2)
			{
			//	if(NetHosting())
			//	{
			//		if (NetHostGameStart() == kNetErr_Success)
				//	{
						inputSelected = false;
						
						NetClientIsReady();
						weapIndx = 0;
						itemIndx = 0;
						mClick = 0;
						cClick = 0;
						cWeapClick = 0;
						mWeapClick = 0;
						cWAClick = 0;
						cIAClick = 0;
						inputSource = WEAPON;
						gCurrentScreen = Battle;	
				//	}
			//	}
			//	else
			//	{
			//		gCurrentScreen = Battle;
			//	}
			}
		}
		break;

		case Battle:
		{
			// FIXME: account for dblclicks
			if (x > k_angPowDown10X && y > k_TopRowY && x < (k_angPowDown10X + k_buttonsBattleW) && y < (k_TopRowY + k_angPowHW) )
				AngleCheck(wind,x,y);
			
			else if (x > k_angPowDown10X && y > k_bottomRowY && x < (k_angPowDown10X + k_buttonsBattleW) && y < (k_bottomRowY + k_angPowHW) )
				PowerCheck(wind,x,y);

			else if (x > k_weaponX && y > k_TopRowY && x < k_weaponX + k_weaponW - k_scrollArrowH && y < (k_TopRowY + k_scrollArrowH * numElements) )
			{
				inputSource = WEAPON;
				mWeapClick = (y - k_TopRowY)/k_scrollArrowH; //index into text display 0, 1 or 2
							
				weapIndx = weapIndx - cWeapClick + mWeapClick;
				cWeapClick = mWeapClick;
			}
			else if (x > (k_weaponX + k_weaponW - k_scrollArrowH) && y > k_TopRowY && x < (k_weaponX + k_weaponW) && y < (k_TopRowY + k_scrollArrowH * numElements) )
			{
				if (inputSource == WEAPON)
					WeaponCheck(wind,x,y);
				inputSource = WEAPON;
			}
			else if (x > k_fireX && y > k_TopRowY && x < (k_fireX + k_angPowHW) && y < k_TopRowY + k_angPowHW)
			{
				if (inputSource == WEAPON)
				{
					inputSelected = false;
					inputSource = DEFAULT;
					FireCheck (wind, x, y);
				}
				
			}
			
			else if (iTank[iTurnOrderID].item_ctr !=0 && x > k_itemX && y > k_bottomRowY && x < (k_itemX + k_itemW - k_scrollArrowH) && y < (k_bottomRowY + k_scrollArrowH * numElements) )
			{
				inputSource = ITEM;
				mClick = (y - k_bottomRowY)/k_scrollArrowH; //index into text display 0, 1 or 2
				
				itemIndx = itemIndx - cClick + mClick;
				cClick = mClick;
			}
			else if (x > (k_itemX + k_itemW - k_scrollArrowH) && y > k_bottomRowY && x < (k_itemX + k_itemW) && y < (k_bottomRowY + k_scrollArrowH * numElements) )	
			{
				if (inputSource == ITEM)
					ItemCheck (wind,x,y);
				inputSource = ITEM;
			}

			else if (x > k_useX && y > k_bottomRowY && x < (k_useX + k_angPowHW) && y < (k_bottomRowY + k_angPowHW) )
			{
				if (inputSource == ITEM)
				{
					inputSelected = false;
					inputSource = DEFAULT;
					UseCheck (wind, x, y);
				}
				
			}

			else if (started && x > k_chatX && y > k_TopRowY + k_scrollArrowH * k_chatMultiplier && x < (k_chatX + k_chatW) && y < k_TopRowY + k_scrollArrowH * (k_chatMultiplier+2) )
				ChatCheck ();

			else if (x > (k_chatX + k_chatW - k_scrollArrowH) && y > k_TopRowY && x < (k_chatX + k_chatW) && y < (k_TopRowY + k_scrollArrowH * k_chatMultiplier) )
				ChatScrollCheck (wind, x, y);

			else if (x > k_OriginX +2 && y > k_bottomRowY && x < (k_OriginX+2 + k_angPowHW) && y < (k_bottomRowY + k_angPowHW) )
				SendCheck ();
		}
		break;

		case PostMortem:
		{
			if (x > k_GeneralRtX && y > k_GeneralY && x < k_GeneralRtX2 && y < k_GeneralY2)
			{
				// jpetrie  Added, I think this is where I need to reset network.
				NetResetAllState();

				gCurrentScreen = Title;
			}
		}
		break;
	}
}

void UIHandleKey(WPARAM wParam)
{
	switch (gCurrentScreen)
	{
	case JoinGameSetup:
		{
			if (wParam == VK_TAB)
			{
				InputSource is = inputSource;
				
				if (inputSource != HOSTLIST)
					EnterInput();
				
				switch (is) 
				{
				case IP:
					{
						inputSource = PORT;
						SourceCheck(inputSource);
					}
					break;
				case PORT:
					{
						inputSource = PLAYERNAME;
						SourceCheck(inputSource);
					}
					break;
				case PLAYERNAME:
					{
						inputSource = HOSTLIST;
						inputSelected = true;
					}
					break;
				case HOSTLIST:
					{
						inputSource = IP;
						SourceCheck(inputSource);
					}
					break;
				}
			break;
			}
		}
		// fall through
	case HostGameSetup:
		{
			if(wParam == VK_RETURN)
			{ 				
				EnterInput();
				break;
			}
			else if (wParam == VK_BACK && ctr > 0)
			{
				InputBuffer[--ctr] = '\0';
				break;
			}
			else if (wParam == VK_TAB)
			{
				InputSource is = inputSource;
				EnterInput();
				switch (is) 
				{
				case PORT:
					{
						inputSource = GAMENAME;
						SourceCheck(inputSource);
					}
					break;
				case GAMENAME:
					{
						inputSource = PLAYERNAME;
						SourceCheck(inputSource);
					}
					break;
				/*case MAXPLAYERS:
					{
						inputSource = MAXROUNDS;
						SourceCheck(inputSource);
					}
					break;
				case MAXROUNDS:
					{
						inputSource = PLAYERNAME;
						SourceCheck(inputSource);
					}
					break;*/
				case PLAYERNAME:
					{
						inputSource = PORT;
						SourceCheck(inputSource);
					}
					break;
				}
			break;
			}
			else
			{
				switch (inputSource)
				{
				case PORT:
					{
						if (ctr < maxPort-1)
						{
							InputBuffer[ctr++] = static_cast <int> (wParam);
							InputBuffer[ctr] = '\0';
						}
					}
					break;
				case GAMENAME:
					{
						if (ctr < kNet_GameStrLimit-1)
						{
							InputBuffer[ctr++] = static_cast <char> (wParam);
							InputBuffer[ctr] = '\0';
						}
					}
					break;
				
				case IP:
					// fall through
				case PLAYERNAME:
					{
						if (ctr < kNet_NameStrLimit-1)
						{
							InputBuffer[ctr++] = static_cast <char> (wParam);
							InputBuffer[ctr] = '\0';
						}
					}
					break;
				}
			}
		}
		break;
	case PreBattle:
		{
			if (wParam == VK_RETURN)
				EnterInput();
		}
		break;
	case PreGameSetup:
		// fall through
	case Battle:
		{
			if (wParam != VK_RETURN)
			{
				if (!buffer1Full)
				{
					if (wParam == VK_BACK)
						ChatBuffer[--ctr] = '\0';
					else
					{
						ChatBuffer[ctr++] = static_cast <char> (wParam);
						ChatBuffer[ctr] = '\0';
					}
				}
				else if (ctr <= chatMaxLetters)
				{
					if (wParam == VK_BACK)
						InputBuffer[--ctr] = '\0';
					else
					{
						InputBuffer[ctr++] = static_cast <char> (wParam);
						InputBuffer[ctr] = '\0';
					}
				}			
				else if (ctr > chatMaxLetters && !buffer1Full)
				{
					buffer1Full = true;
					
					InputBuffer = new char[kNet_ChatStrLimit];
					InputBuffer[0] = '\0';
					
					ctr = 0;
				}
			}
			else
				SendCheck();
		}
		break;
	}
}

void UIHandleChat(string ThisChat)
{
	vec_chatList.push_back(ThisChat);
}

void UIHandleAddHostList(int players, int port, string address, string name)
{
	HostInfo hi;
	
	hi.plyrs = players;
	hi.port = port;
	hi.addrs = address;
	hi.name = name;

	vec_hostList.push_back(hi);
}

void ClearPreGameChat()
{
	vec_chatList.clear();
}

static void SourceCheck(InputSource is)
{
	switch (is)
	{
	case IP:
		// fall through
	case PLAYERNAME:
		{
			InputBuffer = new char[kNet_NameStrLimit];
				InputBuffer[ctr] = '\0';
			inputSelected = true;
		}
		break;
	case PORT:
		{
			InputBuffer = new char[maxPort];
			InputBuffer[ctr] = '\0';
			inputSelected = true;
		}
		break;

	case GAMENAME:
		{
			InputBuffer = new char[kNet_GameStrLimit];
			InputBuffer[ctr] = '\0';
			inputSelected = true;
		}
		break;
	}
	/*if (is != DEFAULT)
	{
		InputBuffer[ctr] = '\0';
		inputSelected = true;
	}*/
}

static void EnterInput(void)
{
	switch (inputSource)
	{
		case IP:
			{
				ipUI = InputBuffer;
				inputSelected = false;
				ctr = 0;
				delete [] InputBuffer;
				inputSource = DEFAULT;
			}
			break;
		case PORT:		
			{
				portUI = atoi(InputBuffer);
				inputSelected = false;
				ctr = 0;
				delete [] InputBuffer;
				inputSource = DEFAULT;
			}
			break;
		case GAMENAME: 	
			{
				nameUI = InputBuffer;
				inputSelected = false;
				ctr = 0;
				delete [] InputBuffer;
				inputSource = DEFAULT;
			}
			break;
		
		case PLAYERNAME:
			{
				playName = InputBuffer;
				GameSetPlayerName(playName);
				inputSelected = false;
				ctr = 0;
				delete [] InputBuffer;
				inputSource = DEFAULT;
			}
			break;
		case HOSTLIST:
			{
				if (vec_hostList.size() != 0)
				{
					ipUI = vec_hostList[joinIndx].addrs;
					portUI = vec_hostList[joinIndx].port;
				}
				inputSelected = false;
				inputSource = DEFAULT;
			}
			break;
		case WEAPON_A:
			{
				PurchaseWeapon(weapAIndx);
				inputSelected = false;
				inputSource = DEFAULT;
			}
			break;
		case ITEM_A:
			{
				PurchaseItem(itemAIndx);
				inputSelected = false;
				inputSource = DEFAULT;
			}
			break;
		case WEAPON:
			{
					SellWeapon(weapIndx);
				inputSelected = false;
				inputSource = DEFAULT;
			}
			break;
		case ITEM:
			{
				SellItem(itemIndx);
				inputSelected = false;
				inputSource = DEFAULT;
			}
			break;
		}

}

static void AngleCheck (HWND wind, int x,  int y)
{
	const int MaxAngle = 180;
	
	if (x > k_angPowDown10X && x < (k_angPowDown10X  + k_angPowHW) )
	{
		if (Angle > (MaxAngle - 10))
			Angle = (Angle - MaxAngle + 10);
		else
		{
			Angle += 10;  
		}
		NetClientUpdate(iTurnOrderID,Angle);
	}

	else if (x > k_angPowDown1X && x < (k_angPowDown1X + k_angPowHW) )
	{
		if (Angle == MaxAngle)
			Angle = 0;
		else
		{
			Angle++;
		}
		NetClientUpdate(iTurnOrderID,Angle);
		
	}

	else if (x > k_angPowUp1X && x < (k_angPowUp1X + k_angPowHW) )
	{
		if (Angle ==0)
			Angle = MaxAngle;
		else
		{
			Angle--;
		}
		NetClientUpdate(iTurnOrderID,Angle);
	}

	else if (x > k_angPowUp10X && x < (k_angPowUp10X + k_angPowHW) )
	{
		if (Angle<10)
			Angle = (MaxAngle + Angle - 10);
		else
		{
			Angle -= 10;
		}
		NetClientUpdate(iTurnOrderID,Angle);
	}

	(Angle < 10) ? numAnglen = 1 : (Angle < 100) ? numAnglen = 2 : numAnglen = 3;
}

static void PowerCheck (HWND wind,int x, int y)
{
	const int MaxPower = 100;
	
	if (x > k_angPowDown10X && x < (k_angPowDown10X  + k_angPowHW) )
	{
		if (Power < 10)
			Power = (MaxPower + Power - 10);
		else
			Power -= 10;
	}

	if (x > k_angPowDown1X && x < (k_angPowDown1X + k_angPowHW) )
	{
		if (Power == 0)
			Power = MaxPower;
		else
			Power--;
	}

	if (x > k_angPowUp1X && x < (k_angPowUp1X + k_angPowHW) )
	{
		if (Power == MaxPower)
			Power = 0;
		else
			Power++;
	}

	if (x > k_angPowUp10X && x < (k_angPowUp10X + k_angPowHW) )
	{
		if (Power > (90))
			Power = (Power - MaxPower + 10);
		else
			Power += 10;  
	}

	(Power < 10) ? numPowlen = 1 : (Power < 100) ? numPowlen = 2 : numPowlen = 3;


}

static void WeaponCheck (HWND wind,int x, int y)
{
	// weapon arrow down
	if( y > (k_TopRowY + k_scrollArrowH * (numElements - 1)) && y < (k_TopRowY + k_scrollArrowH * numElements) && cWeapClick < (iTank[iTurnOrderID].weap_ctr-1)) 	
	{
		if (weapIndx < iTank[iTurnOrderID].weap_ctr-1)
		{
			++weapIndx;
			if (cWeapClick < (numElements-1))
			{
				++cWeapClick;
				++mWeapClick;
			}
		}
		/*char * temp = WeaponName[0];
		int i;

		for (i = 0; i< numWeapons - 1; ++i)
		{
			WeaponName[i]= WeaponName[i+1];
		}
		WeaponName[i] = temp;*/
	}

	// weapon arrow up
	if (y > k_TopRowY && y < (k_TopRowY + k_scrollArrowH) )

	{
		if (weapIndx > 0)
		{
			--weapIndx;
			if (cWeapClick > 0)
			{
				--cWeapClick;
				--mWeapClick;
			}
		}
		/*char * temp = WeaponName[numWeapons - 1];
		int i;

		for (i = numWeapons - 1; i> 0; --i)
		{
			WeaponName[i]= WeaponName[i-1];
		}
		WeaponName[i] = temp;*/
	}

}

static void FireCheck (HWND wind,int x, int y)
{
	if (GameCanPlayerFire() )
	{
		dcon << "In the client turn fire check thing " << weapIndx << "\n";	
		NetClientTurn(Power,Angle,iTank[iTurnOrderID].weap[weapIndx],0);
		dcon << "FIREWEAP\n";
		FireWeapon(&iTank[iTurnOrderID],weapIndx);
		dcon << ":!\n";
	}
	inputSource = WEAPON;
}

static void ItemCheck (HWND wind,int x, int y)
{
	// Item arrow down
	if( y > (k_bottomRowY + k_scrollArrowH * (numElements - 1)) && y < (k_bottomRowY + k_scrollArrowH * numElements) && cClick < (iTank[iTurnOrderID].item_ctr-1)) 	
	{
		if (itemIndx < (iTank[iTurnOrderID].item_ctr-1))
		{
			++itemIndx;
			if (cClick < (numElements-1))
			{
				++cClick;
				++mClick;
			}
		}
	}

	// item arrow up
	if (y > k_bottomRowY && y < (k_bottomRowY + k_scrollArrowH) )
	{
		if (itemIndx > 0)
		{
			--itemIndx;
			if (cClick > 0)
			{
				--cClick;
				--mClick;
			}
		}
	}
}

static void UseCheck (HWND wind,int x, int y)
{
	if(!iTankUsedItem && iTurn == iTurnOrderID)
	{
		NetClientTurn(0,255,iTank[iTurnOrderID].item[itemIndx],0);
		FireItem(&iTank[iTurnOrderID],itemIndx);
	}
}

static void ChatCheck ()
{
	if (!inputSelected)
	{
		ChatBuffer = new char[kNet_ChatStrLimit];
		ChatBuffer[0] = '\0';
	}
	inputSelected = true;
}


static void ChatScrollCheck (HWND wind,int x, int y)
{
	if (gCurrentScreen == Battle)
	{
		// chat arrow up
		if (y > k_TopRowY && y < (k_TopRowY + k_scrollArrowH) && indx > 0)
			--indx;
		// chat arrow down
		if (y > k_TopRowY + k_scrollArrowH * (k_chatMultiplier - 1) && y < (k_TopRowY + k_scrollArrowH * k_chatMultiplier) && indx < vec_chatList.size() - numDisplayVectors )
				++indx;
	}
	else
	{
		if (y > k_preChatY1 && y < (k_preChatY1 + k_scrollArrowH) && indx > 0)
			--indx;
		// chat scrollarrow down
		if ((y > k_preChatY2 - k_scrollArrowH) && y < k_preChatY2  && indx < vec_chatList.size() - numDispVecPreGame )
			++indx;
	}
}

static void SendCheck ()
{
	if (inputSelected) 
	{		

		{
			string ThisChat;

			ThisChat = ChatBuffer;
						
			if (NetClientChat(ThisChat) == kNetErr_Success)
			{
				// scrolls vectors so that new vector can be seen
				if (gCurrentScreen == Battle)
				{
					if (vec_chatList.size() > numDisplayVectors-1)
						++indx;
				}
				else
				{
					if (vec_chatList.size() > numDispVecPreGame-1)
						++indx;
				}
			}
		}
		if (buffer1Full)
		{
			if (*InputBuffer)
			{
				string ThisChat2;
				ThisChat2= InputBuffer;

				if (NetClientChat(ThisChat2) == kNetErr_Success)
				{
					// scrolls vectors so that new vector can be seen
					if (gCurrentScreen == Battle)
					{
						if (vec_chatList.size() > numDisplayVectors-1)
							++indx;
					}
					else
					{
						if (vec_chatList.size() > numDispVecPreGame -1)
							++indx;
					}
				}
			}
		}

		delete [] ChatBuffer;
		if (buffer1Full)
		{
			delete [] InputBuffer;
			buffer1Full = false;
		}
	}

	inputSelected = false;
	ctr = 0;
	
}



/*static int StrLen(const char *str)
{
	const char *p = str;
	int ctr = 0;

	while (*p++)
		++ctr;
	return ctr;
}*/

