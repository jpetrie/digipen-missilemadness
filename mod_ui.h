// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// 
//	owned by: Jesica McCrory
//
// externs all variables shared between mod_ui and handler_ui and are defined
// in mod_ui.cpp
// defines all constants used in displaying the ui
// =============================================================================

#ifndef _TANK_MOD_UI_H_
#define _TANK_MOD_UI_H_
#pragma warning (disable:4786)

#include "tank.h"


void UI_DisplayCurrentScreen ();
char* GetCurrentWeapon();
char* GetCurrentItem();



extern char* ChatBuffer;
extern char* InputBuffer;
extern vector <string> vec_chatList;
extern int indx;
extern int ctr;
extern bool buffer1Full;

extern bool errorUI;
extern int portUI;
const int maxPort			= 5;

extern string defaultName;
extern string defaultGame;
extern int defaultPort;
const int maxPlayers		= 8;
const int defaultRounds		= 3;

extern string playName;
extern string nameUI;
extern int numPlayers;
extern int maxRounds;
extern string ipUI;

enum InputSource
{
	DEFAULT,
	IP,
	PORT,
	GAMENAME,
	PLAYERNAME,
	HOSTLIST,
	WEAPON,
	WEAPON_A,
	ITEM,
	ITEM_A
};

extern InputSource inputSource;
const int numDisplayVectors = 5;
const int numDispVecPreGame	= 16;
const int numDispVecPBattle = 10;
const int fontH				= 14;

const int chatMaxLetters = 25;

const int yTextSub			= 15;
const int yTextAdd			=  3;

// Battle button Coordinates
const int k_textOffset10X_Y	= 4;	//8;
const int k_textOffset1X	= 8;   //12;			// pixel offset to start text inside button

const int k_angPowDown10X	= 6;  //44;			// button for decreasing angle or power by 10			
const int k_angPowDown1X	= 40;  //86;			// button for decreasing angle or power by 1
const int k_angPowUp1X		= 124; //202;			// button for increasing angle or power by 1
const int k_angPowUp10X		= 154; //244;			// button for increasing angle or power by 10
const int k_angPowRectX		= 70; //128;			// rect for displaying value
const int k_angPowRectX2	= 118; //192;
const int k_angPowRadius	= 12;  //16;
const int k_angPowHW		= 24;  //32;			// height and width individual above buttons
const int k_angTextX		= 77; //142;

const int k_powTextX		= 75;
const int k_powTextY		= 536; //689;

const int k_angPowTextX		= 83;

const int k_fireX			= 214;

const int k_weaponX			= 264; //352;			// space for current selected weapon		
const int k_weaponW			= 192; //240;
const int k_weaponTextX		= 267;  //366;

const int k_itemX			= k_weaponX;
const int k_itemW			= k_weaponW;
const int k_itemTextX		= k_weaponTextX;
const int k_itemTextY		= k_powTextY;

const int k_useX			= k_fireX;

const int k_chatX			= 528; //704;			// space for chat message box
const int k_chatW			= 266;
const int k_chatTextX		= 615; //810;
const int k_chatMultiplier	= 5;

const int k_scrollArrowH	=  15;

const int k_TopRowY			= 492; //624;			// y coordinate for top row x coordinates
const int k_TopRowTextY		= 475; //608;
const int k_buttonsBattleW	= 168; //240;			// width of weapon rect, all angle buttons together

const int k_chatDispTextY	= k_TopRowY + k_scrollArrowH * k_chatMultiplier + 1;

const int k_bottomRowY		= 552; //706;


const float scalar = 0.75;

// Wind arrows bases
const int k_OriginX			=480; //632*scalar;
const int k_OriginY			=k_TopRowY; //684*scalar;
const int k_base0			=	8*scalar;
const int k_base1			=  16*scalar;
const int k_base2			=  24*scalar;
const int k_base3			=  32*scalar;
const int k_base4			=  48*scalar;
const int k_windTextX		=k_OriginX-3; //632*scalar;

//const int k_windTextY		= 644*scalar;

// Start up button coordinates
/*const int k_bottomLeftX		= 104*scalar;			// button on left of screen
const int k_botLftTextX		= 110*scalar;
const int k_botRowTextY		= 722*scalar;

const int k_bottomRightX	= 716*scalar;			// button on right of screen
const int k_botRtTextX		= 722*scalar;

const int k_centerButtonX	= 410*scalar;			// x coord. for center buttons
const int k_cenColumnTextX	= 416*scalar;

const int k_topButtonY		= 674*scalar;			// y coord. for top button
const int k_topCenTextY		= 678*scalar;

const int k_bottomButtonY	= 718*scalar;			// y coord. for bottom buttons
*/


const int k_buttonTitleW	= 204*scalar;			// width of title screen buttons
const int k_buttonTitleH	=  32*scalar;			// height of title screen buttons

const int textoffsetX		= 30;
const int textoffsetY		= 4;

const int baseX				= 24;
const int baseY				= 12;
const int k_TitleX			= baseX; 
const int k_TitleX2			= k_TitleX + k_buttonTitleW;
const int k_TitleQuitY2		= SCREEN_HEIGHT-baseY;
const int k_TitleQuitY		= k_TitleQuitY2 - k_buttonTitleH;
const int k_TitleOptionsY2	= k_TitleQuitY - baseY;
const int k_TitleOptionsY	= k_TitleOptionsY2 - k_buttonTitleH;
const int k_TitleJoinY2		= k_TitleOptionsY - baseY;
const int k_TitleJoinY		= k_TitleJoinY2 - k_buttonTitleH;
const int k_TitleHostY2		= k_TitleJoinY - baseY;
const int k_TitleHostY		= k_TitleHostY2 - k_buttonTitleH;
const int k_Title1PlayerY2	= k_TitleHostY - baseY;
const int k_Title1PlayerY	= k_Title1PlayerY2 - k_buttonTitleH;

const int k_GeneralRtX		= SCREEN_WIDTH - k_TitleX2;
const int k_GeneralRtX2		= SCREEN_WIDTH - baseX;
const int k_GeneralRTextX	= k_GeneralRtX + textoffsetX;

const int k_GeneralLftX2	= k_GeneralRtX - baseX; 
const int k_GeneralLftX		= k_GeneralLftX2 - k_buttonTitleW;
const int k_GeneralLTextX	= k_GeneralLftX + textoffsetX;

const int k_GeneralY		= k_TitleQuitY;
const int k_GeneralY2		= k_TitleQuitY2;
const int k_GeneralTextY	= k_GeneralY + textoffsetY;



// game setup constants
const int k_errorX1			= k_buttonTitleW/2;
const int k_errorY1			= k_GeneralTextY;



const int k_portX1			= k_buttonTitleW;
const int k_portX2			= k_portX1 + k_buttonTitleW/2;
const int k_portY1			= 200;
const int k_portY2			= k_portY1 + k_buttonTitleH;
const int k_portTextX		= k_portX1 + 4;

const int k_gameNameX1		= k_portX1;
const int k_gameNameX2		= k_portX1 + k_buttonTitleW;
const int k_gameNameY1		= k_portY2 + k_buttonTitleH;
const int k_gameNameY2		= k_gameNameY1 + k_buttonTitleH;
const int k_gameNameTextX	= k_gameNameX1 + 4;

const int k_maxPlayX1		= k_portX1;
const int k_maxPlayX2		= k_portX2;
const int k_maxPlayY1		= k_gameNameY2 + k_buttonTitleH;
const int k_maxPlayY2		= k_maxPlayY1 + k_buttonTitleH;
const int k_maxPlayTextX	= k_maxPlayX1 + 4;

const int k_maxRoundX1		= k_portX1;
const int k_maxRoundX2		= k_portX1 + k_buttonTitleW/4;
const int k_maxRoundY1		= k_maxPlayY2 + k_buttonTitleH;
const int k_maxRoundY2		= k_maxRoundY1 + k_buttonTitleH;
const int k_maxRoundTextX	= k_maxRoundX1 + 4;

const int k_playerNameX1	= k_portX1 + 300;
const int k_playerNameX2	= k_playerNameX1 + k_buttonTitleW;
const int k_playerNameY1	= k_portY1;	
const int k_playerNameY2	= k_portY2;
const int k_playerNameTextX	= k_playerNameX1 + 4;

const int k_joinNameX1		= k_buttonTitleW/2;
const int k_joinNameX2		= k_joinNameX1 + k_buttonTitleW;
const int k_joinNameTextX	= k_joinNameX1 + 4;

const int k_manualIPX1		= k_portX1 + 300;
const int k_manualIPX2		= k_manualIPX1 + k_buttonTitleW;
const int k_manualIPY1		= 450;
const int k_manualIPY2		= k_manualIPY1 + k_buttonTitleH;
const int k_manualIPTextX	= k_manualIPX1 + 4;

const int k_manualPortX1	= k_manualIPX1;
const int k_manualPortX2	= k_manualPortX1 + k_buttonTitleW/2;
const int k_manualPortY1	= k_manualIPY2 + k_buttonTitleH;
const int k_manualPortY2	= k_manualPortY1 + k_buttonTitleH;
const int k_manualPortTextX	= k_manualPortX1 + 4;

const int k_jInfoX1			= k_manualIPX1;	
const int k_jInfoX2			= k_jInfoX1 + k_buttonTitleW * 2;
const int k_jInfoY1			= 200;
const int k_jInfoY2			= k_jInfoY1 + 200;
const int k_jInfoTextX		= k_jInfoX1 + 4;

const int k_preChatX		= k_jInfoX1;
const int k_preChatY1		= k_jInfoY1;
const int k_preChatY2		= k_jInfoY2;

const int k_preChatInY1		= k_manualIPY1;
const int k_preChatInY2		= k_preChatInY1 + 30;

const int k_preGNameX		= k_buttonTitleW/2;
const int k_preGNameY		= k_preChatInY1 - yTextSub;
const int k_preGName2X		= k_preGNameX + 90;

const int k_preGIPX			= k_preGNameX;
const int k_preGIPY			= k_preGNameY + yTextSub;
const int k_preGIP2X		= k_preGName2X;

const int k_preGPortY		= k_preGIPY + yTextSub;

const int k_weapAvailX1		= k_buttonTitleW/2;
const int k_weapAvailX2		= k_weapAvailX1 + k_buttonTitleW * 2;
const int k_weapAvailY1		= 150;
const int k_weapAvailY2		= k_weapAvailY1 + 150;

const int k_weapHaveX2		= SCREEN_WIDTH - k_buttonTitleW/2;
const int k_weapHaveX1		= k_weapHaveX2 - k_buttonTitleW * 2;
const int k_weapHaveY1		= k_weapAvailY1;
const int k_weapHaveY2		= k_weapAvailY2;

const int k_itemAvailX1		= k_weapAvailX1;
const int k_itemAvailX2		= k_weapAvailX2;
const int k_itemAvailY1		= k_weapAvailY2 + k_buttonTitleH*2;
const int k_itemAvailY2		= k_itemAvailY1 + 150;

const int k_itemHaveX2		= k_weapHaveX2;
const int k_itemHaveX1		= k_weapHaveX1;
const int k_itemHaveY1		= k_weapHaveY2 + k_buttonTitleH*2;
const int k_itemHaveY2		= k_itemHaveY1 + 150;

const int k_cashX			= k_cashX + k_buttonTitleW/2;
const int k_cashY			= k_itemHaveY2 + k_buttonTitleH;
const int k_cash2X			= k_cashX + 125;

const int k_rankX			= SCREEN_WIDTH/3;
const int k_resultsX		= k_rankX + 15;
const int k_startResults	= 150;

const int k_instructXY		= 50;
const int k_waitingX		= SCREEN_WIDTH/2;

const int k_startCredits	= 450;


const int k_toolbarH		= SCREEN_HEIGHT-126;//168;	// battle toolbar

const int numElements = 3;
extern int volatile weapIndx;
extern int weapAIndx;
extern int itemIndx;
extern int itemAIndx;
extern int joinIndx;

const int numWeapons = 4;
//extern char *WeaponName[];

//const int numItems = 2;
//extern char *ItemName[];


extern int Angle;
extern int Power;

#endif