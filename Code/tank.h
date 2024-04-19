// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================


// -----------------------------------------------------------------------------
//                                                                         guard
// -----------------------------------------------------------------------------

#ifndef _TANK_H_
#define _TANK_H_

#pragma warning (disable:4786)

// -----------------------------------------------------------------------------
//                                                                      includes
// -----------------------------------------------------------------------------

#include <crtdbg.h>

// windows
#include <windows.h>

// standard C++
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// resources
#include "resource.h"


// -----------------------------------------------------------------------------
//                                                                       defines
// -----------------------------------------------------------------------------

// custom messages
#define WM_NETWORK   WM_USER + 1
#define WM_DEBUGGER  WM_USER + 2


// -----------------------------------------------------------------------------
//                                                              global variables
// -----------------------------------------------------------------------------

// application
extern HDC        gMainDC;
extern HDC        ghdcMem;
extern HWND       gMainWindow;
extern HBITMAP    ghBitmap;
extern HINSTANCE  gAppInstance;

// display resolution
extern int gresolutionX;
extern int gresolutionY;

enum Names
{
	Title,
	HostGameSetup,
	JoinGameSetup,
	PreGameSetup,
	PreBattle,
	Battle,
	PostMortem,
};

extern Names gCurrentScreen;
// -----------------------------------------------------------------------------
//                                                                    prototypes
// -----------------------------------------------------------------------------

// message routers
LRESULT CALLBACK TankRouter(HWND wind,UINT msg,WPARAM wParam,LPARAM lParam);
void             TankDebugRouter(WPARAM cmd,vector<string> *args);

// configs
void ReadCfgs(void);

// -----------------------------------------------------------------------------
//                                                                        macros
// -----------------------------------------------------------------------------

// version
#define TANK_VERSION  0x00


#endif

