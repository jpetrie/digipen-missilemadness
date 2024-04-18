// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================


// -----------------------------------------------------------------------------
//                                                                         guard
// -----------------------------------------------------------------------------
#pragma warning (disable:4786)
#ifndef _TANK_MOD_GAME_H_
#define _TANK_MOD_GAME_H_



#include "tank.h"
#include "mod_terrain.h"
#include "mod_display.h"
#include "mod_items.h"
#include "mod_tank.h"

extern int numAnglen;
extern int numPowlen;
extern int iTankCount;


void GameStartup(void);
void GameShutdown(void);

void GameProcessTurn(float pwr,int ang,int wep,int seed);

void GameStart(char seed,vector<string> names,char id);

void GameLoop(void);

void GameHandleKey(WPARAM key);

bool GameCanPlayerFire(void);
void GameGotoNextTurn(void);
string GameGetPlayerName(void);
void   GameSetPlayerName(string name);

extern Map iSurfaceMap[SCREEN_WIDTH + 1];
extern bool first;

void GameUpdateTank(char id,char ang);

void GameMouseHover(LPARAM coords);

extern vector<StItem> iItemList;
extern tank			*iTank;
extern int       iTankCount;
extern int iTurnOrderID;
extern int iTurn;
#endif

