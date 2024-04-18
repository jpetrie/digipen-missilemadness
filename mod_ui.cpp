// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// 
//	owned by: Jesica McCrory
// =============================================================================
#include "tank.h"

#include "mod_game.h"
#include "mod_terrain.h"
#include "mod_display.h"
#include "mod_weapons.h"
#include "mod_missiles.h"
#include "mod_network.h"
#include "mod_ui.h"
#include "mod_tank.h"
#include "mod_items.h"
#include "windows.h"
#include "mod_weapon_attributes.h"
#include "debugging.h"
#include "mod_ai.h"
#include "handler_ui.h"
#include "mod_econ.h"

#include "mod_sound.h"


// =============================================================================
//	function prototypes
// =============================================================================

static void UIDisplayAngle();
static void UIDisplayPower();
static void UIDisplayWeapon();
static void UIDisplayFire();
static void UIDisplayItem();
static void UIDisplayChat();
static void UIDisplayWind();


// =============================================================================
//	shared variables
// =============================================================================

char* WeaponName[numWeapons] = {"Missle", "Gun", "Cannon", "Bullet"};//,"blah1","blah2","blah3","blah4"};
//char* ItemName[numItems] = {"Shield", "Armor"};


int Angle = 90;
int Power = 50;
static bool on = false;
static int ctdn = 60;

bool errorUI = false;

string defaultName	= "tankplayer";
string defaultGame	= "tankgame";
int defaultPort		= 2808;


string playName = GameGetPlayerName();
int numPlayers = maxPlayers;
int maxRounds = defaultRounds;
int portUI = defaultPort;
string nameUI = defaultGame;
string ipUI;
extern bool started;
extern Weapon m[NUMWEAPONS]; //echan
volatile int weapIndx = 0;
int weapAIndx = 2;
int itemIndx = 0;
int itemAIndx = 0;
int joinIndx = 0;
vector <string> vec_chatList;
vector <HostInfo> vec_hostList;

extern int mClick;
extern volatile int mWeapClick;
extern int mWAClick;
extern int mIAClick;


// from mod_econ.cpp
extern int gInitialWeap[MAX_WEAPONS];
extern int gInitialItem[MAX_ITEMS];
extern int gItemCtr;
extern int gWeapCtr;

extern bool singlePlayer;
extern vector<int> gDeadList;

// =============================================================================
//	function definitions
// =============================================================================

void UI_DisplayCurrentScreen ()
{	
	char numbuf[maxPort];
	switch (gCurrentScreen)
	{
		case Title:
			{		
				BitMap splash;
				splash.file_name = "GameData/Art/missilemadness.bmp";
				splash.height = 600;
				splash.width = 800;
				DisplayBitmap(splash, 0, 0);
				
				//DisplayRect(k_TitleX, k_TitleQuitY, k_TitleX2, k_TitleQuitY2, TOOLBAR, BLACK);
				//DisplayText(k_TitleX + textoffsetX, k_TitleQuitY + textoffsetY, "Quit", LTBLUE);
				
				DisplayRect(k_TitleX, k_TitleOptionsY, k_TitleX2, k_TitleOptionsY2, TOOLBAR, BLACK);
				DisplayText(k_TitleX + textoffsetX, k_TitleOptionsY + textoffsetY, "Quit", LTBLUE);
				
				DisplayRect(k_TitleX, k_TitleJoinY, k_TitleX2, k_TitleJoinY2, TOOLBAR, BLACK);
				DisplayText(k_TitleX + textoffsetX, k_TitleJoinY + textoffsetY, "Join Game", LTBLUE);

				DisplayRect(k_TitleX, k_TitleHostY, k_TitleX2, k_TitleHostY2, TOOLBAR, BLACK);
				DisplayText(k_TitleX + textoffsetX, k_TitleHostY + textoffsetY, "Host Game", LTBLUE);
				
				DisplayRect(k_TitleX, k_Title1PlayerY, k_TitleX2, k_Title1PlayerY2, TOOLBAR, BLACK);
				DisplayText(k_TitleX + textoffsetX, k_Title1PlayerY + textoffsetY, "Single Player", LTBLUE);
				
				int i = 0;
				
				DisplayText(SCREEN_WIDTH/4, k_startCredits + (fontH+2)*i++, "Credits:", BLACK);
				DisplayText(SCREEN_WIDTH/4, k_startCredits + (fontH+2)*i++, "  Scott Smith", BLACK);
				DisplayText(SCREEN_WIDTH/4, k_startCredits + (fontH+2)*i++, "  Patrick Laukaitis", BLACK);
				DisplayText(SCREEN_WIDTH/4, k_startCredits + (fontH+2)*i++, "  Jessica McCrory", BLACK);
				DisplayText(SCREEN_WIDTH/4, k_startCredits + (fontH+2)*i++, "  Josh Petrie", BLACK);
				DisplayText(SCREEN_WIDTH/4, k_startCredits + (fontH+2)*i++, "  James Carnahan ", BLACK);				
				DisplayText(SCREEN_WIDTH/4, k_startCredits + (fontH+2)*i++, "  Eric Roberts", BLACK);

			/*	i = 0;
				DisplayText(SCREEN_WIDTH/4 + 125, k_startCredits + (fontH+2)*i++, " ", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 125, k_startCredits + (fontH+2)*i++, "  \"\"", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 125, k_startCredits + (fontH+2)*i++, "  \"\"", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 125, k_startCredits + (fontH+2)*i++, "  \"More Lines Than Anybody Else\"", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 125, k_startCredits + (fontH+2)*i++, "  \"More Lines Than Anybody But Jessica\"", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 125, k_startCredits + (fontH+2)*i++, "  \"\"", BLACK);				
				DisplayText(SCREEN_WIDTH/4 + 125, k_startCredits + (fontH+2)*i++, "  \"Phantom Missile\"", BLACK);
*/
				i = 0;
				DisplayText(SCREEN_WIDTH/4 + 400, k_startCredits + (fontH+2)*i++, " ", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 400, k_startCredits + (fontH+2)*i++, "  (Producer)", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 400, k_startCredits + (fontH+2)*i++, "  (Product Management)", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 400, k_startCredits + (fontH+2)*i++, "  (User Interface)", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 400, k_startCredits + (fontH+2)*i++, "  (Networking)", BLACK);
				DisplayText(SCREEN_WIDTH/4 + 400, k_startCredits + (fontH+2)*i++, "  (Physics, AI)", BLACK);				
				DisplayText(SCREEN_WIDTH/4 + 400, k_startCredits + (fontH+2)*i++, "  (Battle System)", BLACK);

			}
			break;
		case HostGameSetup:
			{
				DisplayText(SCREEN_WIDTH/3 +50, 5, "HostGameSetup", CYAN);

				DisplayText(k_instructXY, k_instructXY, "To Host a game, click on the information you want to change and",YELLOW);
				DisplayText(k_instructXY, k_instructXY + fontH, "type in what you want it to be. Press <RETURN> to set the new data", YELLOW); 
				DisplayText(k_instructXY, k_instructXY + fontH*2, "or <TAB> to go to the next field. Click on Host Game when finished.", YELLOW);


				DisplayRect(k_portX1,k_portY1,k_portX2, k_portY2, TOOLBAR, BLACK);
				DisplayText(k_portX1, k_portY1 - yTextSub,"Port", CYAN);
				
				
				if (portUI > 0)
				{
					DisplayText(k_portTextX, k_portY1 + yTextAdd, itoa(portUI,numbuf,10), BLACK);
				}

				DisplayRect(k_gameNameX1,k_gameNameY1,k_gameNameX2,k_gameNameY2,TOOLBAR, BLACK);
				DisplayText(k_gameNameX1, k_gameNameY1 - yTextSub, "Game Name", CYAN);
				
				if (nameUI.size() > 0)
				{
					DisplayText(k_gameNameTextX, k_gameNameY1 + yTextAdd, nameUI.c_str(), BLACK);
				}
				
			/*	DisplayRect(k_maxPlayX1, k_maxPlayY1, k_maxPlayX2, k_maxPlayY2, TOOLBAR, BLACK);
				DisplayText(k_maxPlayX1, k_maxPlayY1 - yTextSub, "Max Players", CYAN);

				if (numPlayers > 0)
					DisplayText(k_maxPlayTextX, k_maxPlayY1 + yTextAdd, itoa(numPlayers,numbuf,10), BLACK);

				DisplayRect(k_maxRoundX1, k_maxRoundY1, k_maxRoundX2, k_maxRoundY2, TOOLBAR, BLACK);
				DisplayText(k_maxRoundX1, k_maxRoundY1 - yTextSub, "Max Rounds", CYAN);
				if (maxRounds > 0)
					DisplayText(k_maxRoundTextX, k_maxRoundY1 + yTextAdd, itoa(maxRounds,numbuf,10), BLACK);*/

				// displayed if user enters incorrect input
				if (errorUI)
				{
					DisplayText(k_errorX1, k_errorY1, "Error: One or more fields incorrect.", RED);
				}

				DisplayRect(k_GeneralRtX, k_GeneralY, k_GeneralRtX2, k_GeneralY2, TOOLBAR, BLACK);
				DisplayText(k_GeneralRTextX, k_GeneralTextY, "Return", LTBLUE);

				DisplayRect(k_GeneralLftX, k_GeneralY, k_GeneralLftX2, k_GeneralY2, TOOLBAR, BLACK);
				DisplayText(k_GeneralLTextX, k_GeneralTextY, "Host Game", LTBLUE);

				DisplayRect(k_playerNameX1, k_playerNameY1, k_playerNameX2, k_playerNameY2, TOOLBAR, BLACK);
				DisplayText(k_playerNameX1, k_playerNameY1 - yTextSub, "Player Name", CYAN);
				

				if (playName.size() > 0)
				{
					DisplayText(k_playerNameTextX, k_playerNameY1 + yTextAdd, playName.c_str(), BLACK);
				}

				if (inputSelected)
				{
					switch (inputSource)
					{
					case PORT:
						{
							DisplayRect(k_portX1, k_portY1, k_portX2, k_portY2, LTBLUE, LTBLUE);							
							DisplayText(k_portTextX, k_portY1 + yTextAdd, InputBuffer, BLACK);

						}
						break;
					case GAMENAME:
						{
							DisplayRect(k_gameNameX1,k_gameNameY1,k_gameNameX2,k_gameNameY2,LTBLUE, LTBLUE);						
							DisplayText(k_gameNameTextX, k_gameNameY1 + yTextAdd, InputBuffer, BLACK);

						}
						break;
					
					case PLAYERNAME:
						{
							DisplayRect(k_playerNameX1, k_playerNameY1, k_playerNameX2, k_playerNameY2, LTBLUE, LTBLUE);
							DisplayText(k_playerNameTextX, k_playerNameY1 + yTextAdd, InputBuffer, BLACK);
						}
						break;
					}					
				}
			}
			break;
		case JoinGameSetup:
			{
				DisplayText(k_instructXY, k_instructXY, "To Join a game, select a game from the available list or", YELLOW);
				DisplayText(k_instructXY, k_instructXY + fontH, "click on the IP and type in the address to join a game",YELLOW);
				DisplayText(k_instructXY, k_instructXY + fontH*2, "not tracked by the meta server. After making sure all fields are done,", YELLOW);
				DisplayText(k_instructXY, k_instructXY + fontH*3, "click on Join Game to join.",YELLOW);
				static int updateList = 1;

				if (--updateList == 0)
				{
					vec_hostList.clear();
					NetClientMSList();
					updateList = 1000;
				}
				
				
				
				DisplayText(k_manualIPTextX, k_manualIPY1 - yTextSub*2, "Manual Join Fields", BLUE);

				DisplayRect(k_manualIPX1, k_manualIPY1, k_manualIPX2, k_manualIPY2, TOOLBAR, BLACK); 
				DisplayText(k_manualIPX1, k_manualIPY1 - yTextSub, "IP or DNS", CYAN);
				
				DisplayRect(k_manualPortX1, k_manualPortY1, k_manualPortX2, k_manualPortY2, TOOLBAR, BLACK);
				DisplayText(k_manualPortX1, k_manualPortY1 - yTextSub, "Port", CYAN);

				DisplayRect(k_joinNameX1, k_playerNameY1, k_joinNameX2, k_playerNameY2, TOOLBAR, BLACK);
				DisplayText(k_joinNameX1, k_playerNameY1 - yTextSub, "Player Name", CYAN);
				
				if (playName.size() > 0)
				{
					DisplayText(k_joinNameTextX, k_playerNameY1 + yTextAdd, playName.c_str(), BLACK);
				}
				if (ipUI.size() > 0)
				{
					DisplayText(k_manualIPTextX, k_manualIPY1 + yTextAdd, ipUI.c_str(), BLACK);
				}

				if (portUI > 0)
				{
					DisplayText(k_manualPortTextX, k_manualPortY1 + yTextAdd, itoa(portUI,numbuf,10), BLACK);
				}
				
				DisplayText(SCREEN_WIDTH/3, 5, "JoinGameSetup", CYAN);
				DisplayRect(k_GeneralRtX, k_GeneralY, k_GeneralRtX2, k_GeneralY2, TOOLBAR,BLACK);
				DisplayText(k_GeneralRTextX, k_GeneralTextY, "Return", LTBLUE);

				DisplayRect(k_GeneralLftX, k_GeneralY, k_GeneralLftX2, k_GeneralY2, TOOLBAR,BLACK);
				DisplayText(k_GeneralLTextX, k_GeneralTextY, "Join Game", LTBLUE);
				

				// displayed if user enters incorrect input
				if (errorUI)
				{
					DisplayText(k_errorX1, k_errorY1, "Error: One or more fields incorrect.", RED);
				}
				
				DisplayText(k_jInfoX1,k_jInfoY1 - yTextSub, "Available Games", CYAN);
				
				DisplayRect(k_jInfoX1,k_jInfoY1,k_jInfoX2,k_jInfoY2, TOOLBAR);
				DisplayRect(k_jInfoX2 - k_scrollArrowH, k_jInfoY1, k_jInfoX2, k_jInfoY1 + k_scrollArrowH, BLUE);
				DisplayRect(k_jInfoX2 - k_scrollArrowH, k_jInfoY2 - k_scrollArrowH, k_jInfoX2, k_jInfoY2, BLUE);

				POINT gamePtsUp[3];
				gamePtsUp[0].x = k_jInfoX2 -8;
				gamePtsUp[0].y = k_jInfoY1 + 3;
				gamePtsUp[1].x = k_jInfoX2 -4;
				gamePtsUp[1].y = k_jInfoY1 + 11;
				gamePtsUp[2].x = k_jInfoX2 - 12;
				gamePtsUp[2].y = k_jInfoY1 + 11;

				DisplayPoly (gamePtsUp, 3, 0x000000, 0x000000);

				POINT gamePtsDn[3];
				gamePtsDn[0].x = k_jInfoX2 -8;
				gamePtsDn[0].y = k_jInfoY2 - 3;
				gamePtsDn[1].x = k_jInfoX2 -4;
				gamePtsDn[1].y = k_jInfoY2 - 11;
				gamePtsDn[2].x = k_jInfoX2 - 12;
				gamePtsDn[2].y = k_jInfoY2 - 11;

				DisplayPoly (gamePtsDn, 3, 0x000000, 0x000000);
				
				
			
				if (inputSelected)
				{
					switch (inputSource)
					{
					case PLAYERNAME:
						{
							DisplayRect(k_joinNameX1, k_playerNameY1, k_joinNameX2, k_playerNameY2, LTBLUE);
							DisplayText(k_joinNameTextX, k_playerNameY1 + yTextAdd, InputBuffer, BLACK);
						}
						break;
					case IP:
						{
							DisplayRect(k_manualIPX1, k_manualIPY1, k_manualIPX2, k_manualIPY2, LTBLUE); 
							DisplayText(k_manualIPTextX, k_manualIPY1 + yTextAdd, InputBuffer, BLACK);
						}
						break;
					case PORT:
						{
							DisplayRect(k_manualPortX1, k_manualPortY1, k_manualPortX2, k_manualPortY2, LTBLUE);
							DisplayText(k_manualPortTextX, k_manualPortY1 + yTextAdd, InputBuffer, BLACK);
						}
						break;
					case HOSTLIST:
						{
							DisplayRect(k_jInfoX1, k_jInfoY1, k_jInfoX2 - k_scrollArrowH, k_jInfoY1 + k_scrollArrowH, LTBLUE);
						}
						break;
					}
				}
				
				int i;
				if (joinIndx < numDispVecPreGame)
					i = 0;
				else
					i = joinIndx - (numDispVecPreGame -1);
				for(int row = 0; row < numDispVecPreGame && row < vec_hostList.size(); ++i,++row)
				{
					DisplayText(k_jInfoTextX, k_jInfoY1 + 1 + fontH* row, vec_hostList[i].name.c_str(), BLACK);
					DisplayText(k_jInfoTextX + k_buttonTitleW, k_jInfoY1 + 1 + fontH* row, itoa(vec_hostList[i].plyrs,numbuf,10), BLACK);
				}
			}
			break;
		case PreGameSetup:
			{
				DisplayRect(k_preChatX, k_preChatY1, k_preChatX + k_chatW, k_preChatY2, CHAT_WIN);
				DisplayRect(k_preChatX + k_chatW - k_scrollArrowH, k_preChatY1, k_preChatX + k_chatW, k_preChatY1 + k_scrollArrowH, TOOLBAR);
				DisplayRect(k_preChatX + k_chatW - k_scrollArrowH, k_preChatY2 - k_scrollArrowH, k_preChatX + k_chatW, k_preChatY2, TOOLBAR);
				
				DisplayText(k_instructXY, k_preChatY1,"Chat with other players that are joining",YELLOW);
				DisplayText(k_instructXY,k_preChatY1+ fontH, "while waiting for the host to continue with the game.", YELLOW);
				DisplayText(k_instructXY,k_preChatY1+fontH*2, " Click on the small rectangle, type the text,",YELLOW); 
				DisplayText(k_instructXY,k_preChatY1+fontH*3 ,"then press <RETURN> to send it.",YELLOW);


				DisplayText(k_preChatX, k_preChatY1 - yTextSub, "Chat", BLUE);
				
				// display chat vectors
				int row = 0;
				for (int i = indx; i < vec_chatList.size() && i < indx + numDispVecPreGame; i++)
				{
					DisplayText(k_preChatX + 4, (k_preChatY1 + 2 + fontH * row++), vec_chatList[i].c_str(), BLACK);
				}

				// input chat display
				if (inputSelected)
				{
					DisplayRect(k_preChatX, k_preChatInY1, k_preChatX + k_chatW, k_preChatInY2, LTBLUE);
				
				
						DisplayText(k_preChatX + 4, k_preChatInY1, ChatBuffer, BLACK);
					
					if (buffer1Full)
						DisplayText(k_preChatX + 4, k_preChatInY1 + fontH, InputBuffer, BLACK);
				}
				else
					DisplayRect(k_preChatX, k_preChatInY1, k_preChatX + k_chatW, k_preChatInY2, CHAT_WIN);
				
				POINT chatPtsUp[3];
				chatPtsUp[0].x = k_preChatX + k_chatW -8;
				chatPtsUp[0].y = k_preChatY1 + 3;
				chatPtsUp[1].x = k_preChatX + k_chatW -4;
				chatPtsUp[1].y = k_preChatY1 + 11;
				chatPtsUp[2].x = k_preChatX + k_chatW - 12;
				chatPtsUp[2].y = k_preChatY1 + 11;

				DisplayPoly (chatPtsUp, 3, BLACK, BLACK);

				POINT chatPtsDn[3];
				chatPtsDn[0].x = k_preChatX + k_chatW -8;
				chatPtsDn[0].y = k_preChatY2 - 3;
				chatPtsDn[1].x = k_preChatX + k_chatW -4;
				chatPtsDn[1].y = k_preChatY2 - 11;
				chatPtsDn[2].x = k_preChatX + k_chatW - 12;
				chatPtsDn[2].y = k_preChatY2 - 11;

				DisplayPoly (chatPtsDn, 3, BLACK, BLACK);

				DisplayText(SCREEN_WIDTH/3, 5, "PreGameSetup", CYAN);
				DisplayRect(k_GeneralRtX, k_GeneralY, k_GeneralRtX2, k_GeneralY2, TOOLBAR);
				DisplayText(k_GeneralRTextX, k_GeneralTextY, "Quit", LTBLUE);

				DisplayRect(k_GeneralLftX, k_GeneralY, k_GeneralLftX2, k_GeneralY2, TOOLBAR);
				DisplayText(k_GeneralLTextX, k_GeneralTextY, "Pre-Battle", LTBLUE);
				
				DisplayText(k_preGNameX, k_preGNameY - yTextSub, "Game Properties", BLUE);

				DisplayText(k_preGNameX, k_preGNameY, "Game Name:", YELLOW);
				DisplayText(k_preGName2X, k_preGNameY, nameUI.c_str(), YELLOW);
			
				DisplayText(k_preGIPX, k_preGIPY, "IP:", YELLOW);
				if (NetHosting())
					ipUI = NetGetLocalIP();
				DisplayText(k_preGIP2X, k_preGIPY, ipUI.c_str(), YELLOW);

				DisplayText(k_preGNameX, k_preGPortY, "Port:", YELLOW);
				DisplayText(k_preGName2X, k_preGPortY, itoa(portUI,numbuf,10), YELLOW);
			}
			break;
		case PreBattle:
			{
				int i, row;
				DisplayText(10, 10, "Equipment Selection", CYAN);
			
				DisplayText(k_instructXY, k_instructXY, "Choose the weapons and equipment you want installed on your tank.", YELLOW);
				DisplayText(k_instructXY, k_instructXY + fontH, "Select a weapon or item from the left list and press <RETURN> to purchase the item.", YELLOW);
				DisplayText(k_instructXY, k_instructXY + fontH*2, "Select a weapon or item from the right list and press <RETURN> to sell the item back.",YELLOW);
				DisplayText(k_instructXY, k_instructXY + fontH*3, "Your tank can only carry a limited amount of equipment, so choose carefully.", YELLOW);

				DisplayRect(k_weapAvailX1, k_weapAvailY1, k_weapAvailX2, k_weapAvailY2, TOOLBAR);
				DisplayRect(k_weapAvailX2 - k_scrollArrowH, k_weapAvailY1, k_weapAvailX2, k_weapAvailY1+k_scrollArrowH, BLUE);
				DisplayRect(k_weapAvailX2 - k_scrollArrowH, k_weapAvailY2 - k_scrollArrowH, k_weapAvailX2, k_weapAvailY2, BLUE);
				
				//up
				POINT arrowPts[3];
				arrowPts[0].x = k_weapAvailX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_weapAvailY1 + 3;
				arrowPts[1].x = k_weapAvailX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_weapAvailY1 + 11;
				arrowPts[2].x = k_weapAvailX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_weapAvailY1 + 11;				
				DisplayPoly (arrowPts, 3, BLACK, BLACK);

				//down
				arrowPts[0].x = k_weapAvailX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_weapAvailY2 - 3;
				arrowPts[1].x = k_weapAvailX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_weapAvailY2 - 11;
				arrowPts[2].x = k_weapAvailX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_weapAvailY2 - 11;			
				DisplayPoly (arrowPts, 3, BLACK, BLACK);

				if (inputSource == WEAPON_A)
				{
					/*if (weapAIndx < numDispVecPBattle && weapAIndx > 0)
					{
						DisplayRect(k_weapAvailX1, k_weapAvailY1 + k_scrollArrowH * (weapAIndx-1), k_weapAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_weapAvailY1 + k_scrollArrowH + k_scrollArrowH*(weapAIndx-1), LTBLUE);
					}
					else
					{
						DisplayRect(k_weapAvailX1, k_weapAvailY1 + k_scrollArrowH * (numDispVecPBattle - 1), k_weapAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_weapAvailY1 + k_scrollArrowH*numDispVecPBattle, LTBLUE);
					}*/

					/*if (weapAIndx >= numDispVecPBattle)
						DisplayRect(k_weapAvailX1, k_weapAvailY1 + k_scrollArrowH * (numDispVecPBattle - 1), k_weapAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_weapAvailY1 + k_scrollArrowH*numDispVecPBattle, LTBLUE);
					else if (weapAIndx > 0)*/
					DisplayRect(k_weapAvailX1, k_weapAvailY1 + k_scrollArrowH * mWAClick, k_weapAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_weapAvailY1 + k_scrollArrowH + k_scrollArrowH*mWAClick, LTBLUE);
				}

				// display available weapons in game
				//if (weapAIndx < (numDispVecPBattle-2))
				//	i=2;
				//else
					i = weapAIndx - mWAClick;
				for(row = 0; row < numDispVecPBattle && row < 20; ++i, ++row)
				{
					char buf[256];
					sprintf(buf, "%s",m[i].WeaponNameString.c_str());
					DisplayText(k_weapAvailX1 + 3, k_weapAvailY1 + k_scrollArrowH * row, buf, BLACK);
					sprintf(buf, "$%d",m[i].WeaponCost);
					DisplayText(k_weapAvailX1 + 225, k_weapAvailY1 + k_scrollArrowH * row, buf, BLACK);
				}

				DisplayRect(k_weapHaveX1, k_weapHaveY1, k_weapHaveX2, k_weapHaveY2, TOOLBAR);
				DisplayRect(k_weapHaveX2 - k_scrollArrowH, k_weapHaveY1, k_weapHaveX2, k_weapHaveY1 + k_scrollArrowH, BLUE);
				DisplayRect(k_weapHaveX2 - k_scrollArrowH, k_weapHaveY2 - k_scrollArrowH, k_weapHaveX2, k_weapHaveY2, BLUE);

				//up
				arrowPts[0].x = k_weapHaveX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_weapHaveY1 + 3;
				arrowPts[1].x = k_weapHaveX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_weapHaveY1 + 11;
				arrowPts[2].x = k_weapHaveX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_weapHaveY1 + 11;
				DisplayPoly (arrowPts, 3, BLACK, BLACK);

				//down
				arrowPts[0].x = k_weapHaveX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_weapHaveY2 - 3;
				arrowPts[1].x = k_weapHaveX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_weapHaveY2 - 11;
				arrowPts[2].x = k_weapHaveX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_weapHaveY2 - 11;
				DisplayPoly (arrowPts, 3, BLACK, BLACK);
				
				if (inputSource == WEAPON)
				{
					if (mWeapClick < gWeapCtr)//mWeapClick !=0 && 
						DisplayRect(k_weapHaveX1, k_weapHaveY1 + k_scrollArrowH * mWeapClick, k_weapHaveX1 + k_buttonTitleW *2 - k_scrollArrowH, k_weapHaveY1 + k_scrollArrowH + k_scrollArrowH*mWeapClick, LTBLUE);

					/*if (weapIndx < numDispVecPBattle)
					{
						DisplayRect(k_weapHaveX1, k_weapHaveY1 + k_scrollArrowH * weapIndx, k_weapHaveX1 + k_buttonTitleW *2 - k_scrollArrowH, k_weapHaveY1 + k_scrollArrowH + k_scrollArrowH*weapIndx, LTBLUE);
					}
					else
					{
						DisplayRect(k_weapHaveX1, k_weapHaveY1 + k_scrollArrowH * (numDispVecPBattle - 1), k_weapHaveX1 + k_buttonTitleW *2 - k_scrollArrowH, k_weapHaveY1 + k_scrollArrowH*numDispVecPBattle, LTBLUE);
					}*/
				}

				// display weapons player has purchased	
				if (gWeapCtr > 0)
				{
					//if (weapIndx < 1)
					//	i=0;
					//else
						i = weapIndx - mWeapClick;
					for(row = 0; row < numDispVecPBattle && row < MAX_WEAPONS && row < gWeapCtr; ++i, ++row)
					{
						DisplayText(k_weapHaveX1 + 3, k_weapHaveY1 + k_scrollArrowH * row, m[gInitialWeap[i]].WeaponNameString.c_str(), BLACK);
					}
				}

				DisplayRect(k_itemAvailX1, k_itemAvailY1, k_itemAvailX2, k_itemAvailY2, TOOLBAR);
				DisplayRect(k_itemAvailX2 - k_scrollArrowH, k_itemAvailY1, k_itemAvailX2, k_itemAvailY1+k_scrollArrowH, BLUE);
				DisplayRect(k_itemAvailX2 - k_scrollArrowH, k_itemAvailY2 - k_scrollArrowH, k_itemAvailX2, k_itemAvailY2, BLUE);
				
				//up
				arrowPts[0].x = k_itemAvailX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_itemAvailY1 + 3;
				arrowPts[1].x = k_itemAvailX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_itemAvailY1 + 11;
				arrowPts[2].x = k_itemAvailX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_itemAvailY1 + 11;
				DisplayPoly (arrowPts, 3, BLACK, BLACK);
	
				//down
				arrowPts[0].x = k_itemAvailX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_itemAvailY2 - 3;
				arrowPts[1].x = k_itemAvailX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_itemAvailY2 - 11;
				arrowPts[2].x = k_itemAvailX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_itemAvailY2 - 11;
				DisplayPoly (arrowPts, 3, BLACK, BLACK);

				if (inputSource == ITEM_A)
				{
					if (mIAClick < iItemList.size())
					//DisplayRect(k_itemAvailX1, k_itemAvailY1 + k_scrollArrowH * itemAIndx, k_itemAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_itemAvailY1 + k_scrollArrowH + k_scrollArrowH*itemAIndx, LTBLUE);

						DisplayRect(k_itemAvailX1, k_itemAvailY1 + k_scrollArrowH * mIAClick, k_itemAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_itemAvailY1 + k_scrollArrowH + k_scrollArrowH*mIAClick, LTBLUE);
					/*if (itemAIndx < numDispVecPBattle)
					{
						DisplayRect(k_itemAvailX1, k_itemAvailY1 + k_scrollArrowH * itemAIndx, k_itemAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_itemAvailY1 + k_scrollArrowH + k_scrollArrowH*itemAIndx, LTBLUE);
					}
					else
					{
						DisplayRect(k_itemAvailX1, k_itemAvailY1 + k_scrollArrowH * (numDispVecPBattle - 1), k_itemAvailX1 + k_buttonTitleW *2 - k_scrollArrowH, k_itemAvailY1 + k_scrollArrowH*numDispVecPBattle, LTBLUE);
					}*/
				}

				// display available items in game
				//if (itemAIndx < numDispVecPBattle)
				//	i=0;
				//else
					i = itemAIndx - mIAClick;//(numDispVecPBattle-1);
				for(row = 0; row < numDispVecPBattle && row < iItemList.size(); ++i, ++row)
				{
					char buf[246];
					DisplayText(k_itemAvailX1 + 3, k_itemAvailY1 + k_scrollArrowH * row, iItemList[i].name.c_str(), BLACK);
					sprintf(buf, "$%d",iItemList[i].cost);
					DisplayText(k_itemAvailX1 + 225, k_itemAvailY1 + k_scrollArrowH * row, buf, BLACK);
				}

				DisplayRect(k_itemHaveX1, k_itemHaveY1, k_itemHaveX2, k_itemHaveY2, TOOLBAR);
				DisplayRect(k_itemHaveX2 - k_scrollArrowH, k_itemHaveY1, k_itemHaveX2, k_itemHaveY1+k_scrollArrowH, BLUE);
				DisplayRect(k_itemHaveX2 - k_scrollArrowH, k_itemHaveY2 - k_scrollArrowH, k_itemHaveX2, k_itemHaveY2, BLUE);

				//up
				arrowPts[0].x = k_itemHaveX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_itemHaveY1 + 3;
				arrowPts[1].x = k_itemHaveX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_itemHaveY1 + 11;
				arrowPts[2].x = k_itemHaveX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_itemHaveY1 + 11;	
				DisplayPoly (arrowPts, 3, BLACK, BLACK);

				//down
				arrowPts[0].x = k_itemHaveX1 + k_buttonTitleW *2 -8;
				arrowPts[0].y = k_itemHaveY2 - 3;
				arrowPts[1].x = k_itemHaveX1 + k_buttonTitleW *2 -4;
				arrowPts[1].y = k_itemHaveY2 - 11;
				arrowPts[2].x = k_itemHaveX1 + k_buttonTitleW *2 - 12;
				arrowPts[2].y = k_itemHaveY2 - 11;
				DisplayPoly (arrowPts, 3, BLACK, BLACK);

				if (inputSource == ITEM)
				{
					if (mClick < gItemCtr)
						DisplayRect(k_itemHaveX1, k_itemHaveY1 + k_scrollArrowH * mClick, k_itemHaveX1 + k_buttonTitleW *2 - k_scrollArrowH, k_itemHaveY1 + k_scrollArrowH + k_scrollArrowH*mClick, LTBLUE);

					/*if (itemIndx < numDispVecPBattle)
					{
						DisplayRect(k_itemHaveX1, k_itemHaveY1 + k_scrollArrowH * itemIndx, k_itemHaveX1 + k_buttonTitleW *2 - k_scrollArrowH, k_itemHaveY1 + k_scrollArrowH + k_scrollArrowH*itemIndx, LTBLUE);
					}
					else
					{
						DisplayRect(k_itemHaveX1, k_itemHaveY1 + k_scrollArrowH * (numDispVecPBattle - 1), k_itemHaveX1 + k_buttonTitleW *2 - k_scrollArrowH, k_itemHaveY1 + k_scrollArrowH*numDispVecPBattle, LTBLUE);
					}*/
				}

				// display items player has purchased				
				//if (itemIndx < numDispVecPBattle)
				//	i=0;
				//else
					i = itemIndx - mClick;//(numDispVecPBattle-1);
				for(row = 0; row < numDispVecPBattle && row < gItemCtr; ++i, ++row)
				{
					DisplayText(k_itemHaveX1 + 3, k_itemHaveY1 + k_scrollArrowH * row, iItemList[gInitialItem[i]].name.c_str(), BLACK);
				}
				
				DisplayText(k_cashX, 130, "Available Weapons:", GREEN);
				DisplayText(k_cashX, 330, "Available Items:", GREEN);
				DisplayText(k_cashX + 340, 130, "Your Weapons:", GREEN);
				DisplayText(k_cashX + 340, 330, "Your Items:", GREEN);

				DisplayText(k_cashX, k_cashY, "Cash Remaining: $", GREEN);
				DisplayText(k_cashX, k_cashY, "Cash Remaining: $", GREEN);


				DisplayText(k_cashX, k_cashY, "Cash Remaining: $", GREEN);
				DisplayText(k_cash2X, k_cashY, itoa(gInitialCash,numbuf,10), GREEN);

				DisplayText(k_cashX + 17, k_cashY + 15, "Weapons Left: ", GREEN);
				DisplayText(k_cash2X, k_cashY + 15, itoa(MAX_WEAPONS - gWeapCtr,numbuf,10), GREEN);

				DisplayText(k_cashX + 43, k_cashY + 30, "Items Left: ", GREEN);
				DisplayText(k_cash2X, k_cashY + 30, itoa(MAX_ITEMS - gItemCtr,numbuf,10), GREEN);
				
				DisplayRect(k_GeneralRtX, k_GeneralY, k_GeneralRtX2, k_GeneralY2, TOOLBAR);
				DisplayText(k_GeneralRTextX, k_GeneralTextY, "Ready", LTBLUE);		
			//	DisplayText(k_waitingX, k_GeneralTextY, "Waiting For Other Players", RED);
			}
			break;
		case Battle:
			{
				// toolbar background
				DisplayRect(0, k_toolbarH, SCREEN_WIDTH, SCREEN_HEIGHT, TOOLBAR);
				
				UIDisplayAngle();
				
				UIDisplayPower();

				UIDisplayWeapon();

				UIDisplayFire();

				UIDisplayItem();

				UIDisplayChat();

				//UIDisplayWind();	
			}
		break;
		case PostMortem:
			{
				DisplayText(SCREEN_WIDTH/3, 5, "Post Mortem", CYAN);

				int i = 0;
				int size = gDeadList.size();
				
				DisplayText (k_rankX, k_startResults - fontH, "Battle Results in Order of Death:", RED);
				if (singlePlayer)
					size = 2;

				for (i = 0; i < size; ++i)
				{
					DisplayText(k_resultsX, k_startResults + fontH*i, iTank[gDeadList[i]].name.c_str(), RED);
					DisplayText(k_rankX, k_startResults + fontH*i, itoa(i+1,numbuf,10), RED);
				}
					
				
				DisplayRect(k_GeneralRtX, k_GeneralY, k_GeneralRtX2, k_GeneralY2, TOOLBAR);
				DisplayText(k_GeneralRTextX, k_GeneralTextY, "Title Screen", LTBLUE);
				/*
				for(int fr = 0; fr < gDeadList.size(); ++fr)
				{
					dcon << iTank[gDeadList[fr]].name.c_str() << "\n";
				}*/
			}
		default:
			break;
	}
			
}

static void UIDisplayAngle ()
{
	DisplayText(k_angTextX, k_TopRowTextY, "Angle", LTBLUE);
	DisplayRect(k_angPowRectX, k_TopRowY, k_angPowRectX2, k_TopRowY + k_angPowHW, DKBLUE);
	
	DisplayCircle(k_angPowDown10X + k_angPowRadius, k_TopRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowDown10X + k_textOffset10X_Y, k_TopRowY + k_textOffset10X_Y, "10", LTBLUE);
	
	DisplayCircle(k_angPowDown1X + k_angPowRadius, k_TopRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowDown1X + k_textOffset1X, k_TopRowY + k_textOffset10X_Y, "1", LTBLUE);
	
	DisplayCircle(k_angPowUp1X + k_angPowRadius, k_TopRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowUp1X + k_textOffset1X, k_TopRowY + k_textOffset10X_Y, "1", LTBLUE);
	
	DisplayCircle(k_angPowUp10X + k_angPowRadius, k_TopRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowUp10X + k_textOffset10X_Y, k_TopRowY + k_textOffset10X_Y, "10", LTBLUE);
}

static void UIDisplayPower ()
{
	DisplayText(k_powTextX, k_powTextY, "Power", LTBLUE);	
	DisplayRect(k_angPowRectX, k_bottomRowY, k_angPowRectX2, k_bottomRowY + k_angPowHW, DKBLUE);
	
	DisplayCircle(k_angPowDown10X + k_angPowRadius, k_bottomRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowDown10X + k_textOffset10X_Y, k_bottomRowY + k_textOffset10X_Y, "10", LTBLUE);
	
	DisplayCircle(k_angPowDown1X + k_angPowRadius, k_bottomRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowDown1X + k_textOffset1X, k_bottomRowY + k_textOffset10X_Y, "1", LTBLUE);
	
	DisplayCircle(k_angPowUp1X + k_angPowRadius, k_bottomRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowUp1X + k_textOffset1X, k_bottomRowY + k_textOffset10X_Y, "1", LTBLUE);
	
	DisplayCircle(k_angPowUp10X + k_angPowRadius, k_bottomRowY + k_angPowRadius, k_angPowRadius, BLACK, DKBLUE);
	DisplayText(k_angPowUp10X + k_textOffset10X_Y, k_bottomRowY + k_textOffset10X_Y, "10", LTBLUE);
}
	
static void UIDisplayWeapon()
{
	DisplayText(k_weaponTextX, k_TopRowTextY, "Weapon", RED);
	DisplayRect(k_weaponX, k_TopRowY, k_weaponX + k_weaponW, k_TopRowY + k_scrollArrowH * numElements, BLACK);
	DisplayRect(k_weaponX + k_weaponW - k_scrollArrowH, k_TopRowY, k_weaponX + k_weaponW, k_TopRowY + k_scrollArrowH, RED);
	DisplayRect(k_weaponX + k_weaponW - k_scrollArrowH, k_TopRowY + k_scrollArrowH * (numElements - 1), k_weaponX + k_weaponW, k_TopRowY + k_scrollArrowH * numElements, RED);
	
	if (inputSource == WEAPON)
	{
		DisplayRect(k_weaponX, k_TopRowY + k_scrollArrowH * mWeapClick, k_weaponX + k_weaponW - k_scrollArrowH, k_TopRowY + k_scrollArrowH + k_scrollArrowH*mWeapClick, LTBLUE);

		/*if (weapIndx < (numElements-1))
		{
			DisplayRect(k_weaponX, k_TopRowY + k_scrollArrowH * weapIndx, k_weaponX + k_weaponW - k_scrollArrowH, k_TopRowY + k_scrollArrowH + k_scrollArrowH*weapIndx, LTBLUE);
		}
		else
		{
			DisplayRect(k_weaponX, k_TopRowY + k_scrollArrowH * (numElements - 1), k_weaponX + k_weaponW - k_scrollArrowH, k_TopRowY + k_scrollArrowH*numElements, LTBLUE);
		}*/
	}

	// display weapons in players inventory
	if(started)
	{
		int i = 0;
		if(iTank[iTurnOrderID].weap_ctr == 1)
		{
			i = 0;
		}
		else
		{
			i = (weapIndx - mWeapClick);
		}
		i = (weapIndx - mWeapClick);
		int row = 0;// -jpetrie  Moved decl outside of for() block; compiler was optimizing?
		for(row; row < numElements && i < iTank[iTurnOrderID].weap_ctr; ++i,++row)
		{
		
			//dcon << "i: " << i << "r: " << row << " (wi: " << weapIndx << " wc: " << mWeapClick << ")\n";
			DisplayText(k_weaponX + 3, k_TopRowY + k_scrollArrowH * row, m[ (iTank[iTurnOrderID].weap[i]) ].WeaponNameString.c_str(), RED);
			//m[   iTank[iTurnOrderID].weap[weapIndx]    ].WeaponNameString.c_str();
		}
		
	}
	/*for (int i = 0; i < numElements && i < numWeapons; ++i)
	{
		DisplayText(k_weaponX + 3, k_TopRowY + k_scrollArrowH * i, WeaponName[i], RED);
	}
	DisplayText(k_weaponX + 4, k_TopRowY, WeaponName[0], MAGENTA);*/

	//scroll arrows up and down
	POINT weapPtsUp[3];
	weapPtsUp[0].x = k_weaponX + k_weaponW -8;
	weapPtsUp[0].y = k_TopRowY + 3;
	weapPtsUp[1].x = k_weaponX + k_weaponW -4;
	weapPtsUp[1].y = k_TopRowY + 11;
	weapPtsUp[2].x = k_weaponX + k_weaponW - 12;
	weapPtsUp[2].y = k_TopRowY + 11;

	DisplayPoly (weapPtsUp, 3, BLACK, BLACK);
	
	POINT weapPtsDn[3];
	weapPtsDn[0].x = k_weaponX + k_weaponW -8;
	weapPtsDn[0].y = k_TopRowY + k_scrollArrowH * numElements - 3;
	weapPtsDn[1].x = k_weaponX + k_weaponW -4;
	weapPtsDn[1].y = k_TopRowY + k_scrollArrowH * numElements - 11;
	weapPtsDn[2].x = k_weaponX + k_weaponW - 12;
	weapPtsDn[2].y = k_TopRowY + k_scrollArrowH * numElements - 11;
	
	DisplayPoly (weapPtsDn, 3, BLACK, BLACK);
}
	
static void UIDisplayFire()
{
	
	DisplayText(k_fireX, k_TopRowTextY, "Fire", RED);
	DisplayRect(k_fireX, k_TopRowY, k_fireX + k_angPowHW, k_TopRowY + k_angPowHW, RED);
	
	if (GameCanPlayerFire())
	{
		if (on )
		{
			DisplayRect(k_fireX, k_TopRowY, k_fireX + k_angPowHW, k_TopRowY + k_angPowHW, ORANGE);
		}
		--ctdn;
			
		if (ctdn == 0)
		{
			if (on)
				on = false;
			else
				on = true;
			ctdn = 60;
		}
	}
}
	
static void UIDisplayItem()
{
	DisplayText(k_itemTextX,k_itemTextY, "Item", BLUE);
	DisplayRect(k_itemX, k_bottomRowY, k_itemX + k_itemW, k_bottomRowY + k_scrollArrowH * numElements, BLACK);
	DisplayRect(k_itemX + k_itemW - k_scrollArrowH, k_bottomRowY, k_itemX + k_itemW, k_bottomRowY + k_scrollArrowH, BLUE);
	DisplayRect(k_itemX + k_itemW - k_scrollArrowH, k_bottomRowY + k_scrollArrowH * (numElements - 1), k_itemX + k_itemW, k_bottomRowY + k_scrollArrowH * numElements, BLUE);

	if (/*inputSelected && */inputSource == ITEM)
	{
		DisplayRect(k_itemX, k_bottomRowY + k_scrollArrowH * mClick, k_itemX + k_itemW - k_scrollArrowH, k_bottomRowY + k_scrollArrowH +k_scrollArrowH*mClick, LTBLUE);
		/*if (itemIndx < (numElements - 1))
			DisplayRect(k_itemX, k_bottomRowY + k_scrollArrowH * itemIndx, k_itemX + k_itemW - k_scrollArrowH, k_bottomRowY + k_scrollArrowH +k_scrollArrowH*itemIndx, LTBLUE);
		else
		{
			DisplayRect(k_itemX, k_bottomRowY + k_scrollArrowH * (numElements - 1), k_itemX + k_itemW - k_scrollArrowH, k_bottomRowY + k_scrollArrowH*numElements, LTBLUE);
		}*/
	}

	// display items in players inventory
	if(started)
	{
		int i;
		if (itemIndx < mClick)
			i=0;
		else
			i = itemIndx - mClick;//(numElements-1);

		int row = 0;// -jpetrie  Moved decl outside of for() block; compiler was optimizing?
		for(row; (row < numElements) && (i < iTank[iTurnOrderID].item_ctr); ++i, ++row)
		{
			DisplayText(k_itemX + 3, k_bottomRowY + k_scrollArrowH * row, iItemList[iTank[iTurnOrderID].item[i]].name.c_str(), BLUE);
		}
	}

	/*for (int i = 0; i < iItemList.size() /* numElements && i < numItems ; ++i)
	{
		DisplayText(k_itemX + 3, k_bottomRowY + k_scrollArrowH * i, iItemList[i].name.c_str(), BLUE);
	}*/
//	DisplayText(k_itemX + 4, k_bottomRowY, ItemName[0], LTBLUE);

	//scroll arrows up and down
	POINT itemPtsUp[3];
	itemPtsUp[0].x = k_itemX + k_itemW -8;
	itemPtsUp[0].y = k_bottomRowY + 3;
	itemPtsUp[1].x = k_itemX + k_itemW -4;
	itemPtsUp[1].y = k_bottomRowY + 11;
	itemPtsUp[2].x = k_itemX + k_itemW - 12;
	itemPtsUp[2].y = k_bottomRowY + 11;

	DisplayPoly (itemPtsUp, 3, BLACK, BLACK);
	
	POINT itemPtsDn[3];
	itemPtsDn[0].x = k_itemX + k_itemW -8;
	itemPtsDn[0].y = k_bottomRowY + k_scrollArrowH * numElements - 3;
	itemPtsDn[1].x = k_itemX + k_itemW -4;
	itemPtsDn[1].y = k_bottomRowY + k_scrollArrowH * numElements - 11;
	itemPtsDn[2].x = k_itemX + k_itemW - 12;
	itemPtsDn[2].y = k_bottomRowY + k_scrollArrowH * numElements - 11;
	
	DisplayPoly (itemPtsDn, 3, BLACK, BLACK);
	
	// use button display
	DisplayText(k_useX, k_powTextY, "Use", BLUE);
	DisplayRect(k_useX, k_bottomRowY, k_useX + k_angPowHW, k_bottomRowY + k_angPowHW, BLUE);

}

static void UIDisplayChat()
{
	DisplayText(k_chatTextX, k_TopRowTextY, "Chat", CYAN);
	DisplayRect(k_chatX, k_TopRowY, k_chatX + k_chatW, k_TopRowY + k_scrollArrowH * k_chatMultiplier, CHAT_WIN);
	DisplayRect(k_chatX + k_chatW - k_scrollArrowH, k_TopRowY, k_chatX + k_chatW, k_TopRowY + k_scrollArrowH, BLACK);
	DisplayRect(k_chatX + k_chatW - k_scrollArrowH, k_TopRowY + k_scrollArrowH * (k_chatMultiplier - 1), k_chatX + k_chatW, k_TopRowY + k_scrollArrowH * k_chatMultiplier, BLACK);

	// scroll arrows up and down
	POINT chatPtsUp[3];
	chatPtsUp[0].x = k_chatX + k_chatW -8;
	chatPtsUp[0].y = k_TopRowY + 3;
	chatPtsUp[1].x = k_chatX + k_chatW -4;
	chatPtsUp[1].y = k_TopRowY + 11;
	chatPtsUp[2].x = k_chatX + k_chatW - 12;
	chatPtsUp[2].y = k_TopRowY + 11;

	DisplayPoly (chatPtsUp, 3, BLACK, CYAN);

	POINT chatPtsDn[3];
	chatPtsDn[0].x = k_chatX + k_chatW -8;
	chatPtsDn[0].y = k_TopRowY + k_scrollArrowH * k_chatMultiplier - 3;
	chatPtsDn[1].x = k_chatX + k_chatW -4;
	chatPtsDn[1].y = k_TopRowY + k_scrollArrowH * k_chatMultiplier - 11;
	chatPtsDn[2].x = k_chatX + k_chatW - 12;
	chatPtsDn[2].y = k_TopRowY + k_scrollArrowH * k_chatMultiplier - 11;

	DisplayPoly (chatPtsDn, 3, BLACK, CYAN);

	// display chat vectors
	int row = 0;
	for (int i = indx; i < vec_chatList.size() && i < indx + numDisplayVectors; i++)
	{
		DisplayText(k_chatX + 4, (k_TopRowY + 2 + fontH * row++), vec_chatList[i].c_str(), BLACK);
	}

	// input chat display
	if (inputSelected)
	{
		DisplayRect(k_chatX, k_TopRowY + k_scrollArrowH * k_chatMultiplier, k_chatX + k_chatW, k_TopRowY + k_scrollArrowH * (k_chatMultiplier+2), LTBLUE);
	
		DisplayText(k_chatX + 4, k_chatDispTextY, ChatBuffer, BLACK);
		
		if (buffer1Full)
			DisplayText(k_chatX + 4, k_chatDispTextY + fontH, InputBuffer, BLACK);
	}
	else
		DisplayRect(k_chatX, k_TopRowY + k_scrollArrowH * k_chatMultiplier, k_chatX + k_chatW, k_TopRowY + k_scrollArrowH * (k_chatMultiplier+2), CHAT_WIN);

	// send display
	DisplayText(k_windTextX, k_powTextY, "Send", CYAN);
	DisplayRect(k_OriginX +2, k_bottomRowY, k_OriginX+2 + k_angPowHW, k_bottomRowY + k_angPowHW, CYAN);

}

static void UIDisplayWind()
{
	DisplayText(k_windTextX, k_TopRowTextY, "Wind", YELLOW);

	// FIXME: uncomment if-else statement when wind element has been added
	//if (wind)
	//{
		/*POINT windptsL[8];
		
		windptsL[0].x= k_OriginX;
		windptsL[0].y= k_OriginY + k_base2;
		windptsL[1].x= k_OriginX + k_base1;
		windptsL[1].y= k_OriginY;
		windptsL[2].x= k_OriginX + k_base1;
		windptsL[2].y= k_OriginY + k_base1;
		windptsL[3].x= k_OriginX + k_base2;
		windptsL[3].y= k_OriginY + k_base1;
		windptsL[4].x= k_OriginX + k_base2;
		windptsL[4].y= k_OriginY + k_base3;
		windptsL[5].x= k_OriginX + k_base1;
		windptsL[5].y= k_OriginY + k_base3;
		windptsL[6].x= k_OriginX + k_base1;
		windptsL[6].y= k_OriginY + k_base4;
		windptsL[7].x= k_OriginX;
		windptsL[7].y= k_OriginY + k_base2;

		DisplayPoly(windptsL, 8, BLACK, YELLOW);*/
	//}
	//else
	//{
		POINT windptsR[8];
		
		windptsR[0].x= k_OriginX + k_base3;
		windptsR[0].y= k_OriginY + k_base2;
		windptsR[1].x= k_OriginX + k_base1;
		windptsR[1].y= k_OriginY;
		windptsR[2].x= k_OriginX + k_base1;
		windptsR[2].y= k_OriginY + k_base1;
		windptsR[3].x= k_OriginX + k_base0;
		windptsR[3].y= k_OriginY + k_base1;
		windptsR[4].x= k_OriginX + k_base0;
		windptsR[4].y= k_OriginY + k_base3;
		windptsR[5].x= k_OriginX + k_base1;
		windptsR[5].y= k_OriginY + k_base3;
		windptsR[6].x= k_OriginX + k_base1;
		windptsR[6].y= k_OriginY + k_base4;
		windptsR[7].x= k_OriginX + k_base3;
		windptsR[7].y= k_OriginY + k_base2;

		DisplayPoly(windptsR, 8, BLACK, YELLOW);
	//}
}

/*char* GetCurrentWeapon()
{
	return WeaponName[0];
}

char* GetCurrentItem()
{
	return ItemName[0];
}*/
