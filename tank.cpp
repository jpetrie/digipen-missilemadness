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

#include "handler_redraw.h"
#include "handler_network.h"
#include "handler_ui.h"

#include "mod_display.h"
#include "mod_game.h"
#include "mod_missiles.h"
#include "mod_network.h"
#include "mod_weapons.h"
#include "mod_ui.h"
#include "mod_game.h"
#include "error_handling.h"

#include "debugging.h"

#include "mod_sound.h"
BG_Music bgm;

string bgm_Title;
string bgm_Lobby;
string bgm_Battle;


// -----------------------------------------------------------------------------
//                                                              global variables
// -----------------------------------------------------------------------------

// application
HDC        gMainDC;
HDC		     ghdcMem;
HBITMAP    ghBitmap;
HWND       gMainWindow;
HINSTANCE  gAppInstance;



// display resolution
int gresolutionX = SCREEN_WIDTH;
int gresolutionY = SCREEN_HEIGHT;

// state information
bool gActive;

// ui information
Names gCurrentScreen  = Title;

// -----------------------------------------------------------------------------
//                                                                     functions
// -----------------------------------------------------------------------------


// _____________________________________________________________________________
int WINAPI WinMain(HINSTANCE thisInstance,HINSTANCE prevInstance,LPSTR cmdLine,int cmdShow)
//	
// Program entry point.
//
// --> thisInstance  Handle to this application instance.
// --> prevInstance  Handle to previous application instance.
// --> cmdLine       Specifies command-line arguments.
// --> cmdShow       Specifies how to invoke the application.
// -R-               Exit status code.
{
WNDCLASSEX  TheWindowClass; 
MSG         TheMessage;
HBRUSH      TheBackBrush;
bool        FlgDone;

	dcon << "Welcome to TANKGAME\n\n";

	// Register window class.
	TheBackBrush = CreateSolidBrush(BACK_G);
	TheWindowClass.cbSize        = sizeof(WNDCLASSEX);
	TheWindowClass.style         = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	TheWindowClass.lpfnWndProc   = TankRouter;
	TheWindowClass.cbClsExtra    = 0;
	TheWindowClass.cbWndExtra    = 0;
	TheWindowClass.hInstance     = thisInstance;
	TheWindowClass.hIcon         = LoadIcon(NULL,IDI_WINLOGO);
	TheWindowClass.hCursor       = LoadCursor(NULL,IDC_ARROW);
	TheWindowClass.hbrBackground = TheBackBrush;
	TheWindowClass.lpszMenuName  = NULL;
	TheWindowClass.lpszClassName = "TankWind";
	TheWindowClass.hIconSm       = LoadIcon(NULL,IDI_WINLOGO);
	RegisterClassEx(&TheWindowClass);

	// Create the window, store associated variables in global context.
	gMainWindow  = CreateWindowEx(NULL,"TankWind","Tank",(WS_POPUP | WS_VISIBLE),0,0,SCREEN_WIDTH,SCREEN_HEIGHT,NULL,NULL,thisInstance,NULL);
	gMainDC      = GetDC(gMainWindow);
	gAppInstance = thisInstance;

	// Capture the mouse.
	SetCapture(gMainWindow);;

	// Prevent bitdepths of < 32.
	DisplayCompRes();

	srand(clock());
	dcon << "[STARTUP]\tSeeding random number generator\n";
	dcon << "[STARTUP]\tEngaging full screen mode\n";
	dcon << "[STARTUP]\tReading configs from \"Game Data/config\"\n";
	ReadCfgs();
	

	ChangeToFullScreen(SCREEN_WIDTH,SCREEN_HEIGHT);

	
	DisplayStartup(gAppInstance,gMainWindow);
	NetStartup();
	GameStartup();

		bgm.Init();
		bgm.Play(bgm_Lobby.c_str(), true);

	// Enter the message pump.
	dcon << "\n";
	FlgDone = false;
	gActive = true;

DWORD a,b = 0;

	while(!FlgDone)
	{


		if(PeekMessage(&TheMessage,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&TheMessage,NULL,0,0))
			{
				FlgDone = true;
			}
			TranslateMessage(&TheMessage);
			DispatchMessage(&TheMessage);
		}

		// Process game loop.
		
		a = GetTickCount();
		if((b - a) >= 100)
		{
			GameLoop();
			NetIdle();
		}
		b = GetTickCount();
	}
	dcon << "\n\n";

	// Clean up and return.
	GameShutdown();
	NetShutdown();

	return (0);
}


// _____________________________________________________________________________
LRESULT CALLBACK TankRouter(HWND wind,UINT msg,WPARAM wParam,LPARAM lParam)
//
// Message router for main window class.
//
// --> wind    Handle to the window object.
// --> msg     Message ID.
// --> wParam  Message parameter.
// --> lParam  Extended message parameters.
// -R-         Result code; True if the message was handled, false otherwise.
{
	switch(msg)
	{
		case WM_CREATE:
		// Handle creation message.
		{
			gMainDC = GetDC(gMainWindow);
			ghdcMem = CreateCompatibleDC(gMainDC);
			ghBitmap = CreateCompatibleBitmap(gMainDC,gresolutionX,gresolutionY);
			ReleaseDC(gMainWindow,gMainDC);
			SelectObject(ghdcMem,ghBitmap);
			return (true);
		}
		break;
		case WM_ACTIVATE:
		// Handle activation message.

		{
			if(LOWORD(wParam) == WA_INACTIVE)
			{
				gActive = false;
			}
			else
			{
				gActive = true;
			}
			return (true);
		}
		break;
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		// Handle mouse click.
		{
			HandleMouseUI(wind, wParam, lParam);
			return (true);
		}
		break;
		case WM_PAINT:
		// Handle paint message.
		{
			HandleRedraw(wind,wParam,lParam);
			return (true);
		}
		break;
		case WM_NETWORK:
		// Handle network message.
		{
			HandleNetwork(wind,wParam,lParam);
			return (true);
		}
		break;
		case WM_CHAR:
		{
			if(!dcon.ProcessKey(wParam))
			{
				GameHandleKey(wParam);
			}
		}
		break;
		case WM_MOUSEMOVE:
		// Handle mouse moved.
		{
			GameMouseHover(lParam);

			return 0;
		}
		break;
		case WM_CLOSE:
		// Handle quit.
		{
			PostQuitMessage(0);
			return (true);
		}
		break;
		case WM_DEBUGGER:
		// Handle debugger message.
		{
			TankDebugRouter(wParam,reinterpret_cast<vector<string> *>(lParam));

			// Delete the argument pointer.
			delete reinterpret_cast<vector<string> *>(lParam);
		}
		break;
		default:
		{
			// Do nothing.
		}
	}

	// Fail-safe; have Windows employ the
	// default message handler to handle the message.
	return (DefWindowProc(wind,msg,wParam,lParam));
}


void TankDebugRouter(WPARAM cmd,vector<string> *args)
{
	switch(cmd)
	{
		case kDbgCmd_NetHost:
		{
			NetHostGame(atoi(args->at(0).c_str()),args->at(1).c_str());
		}
		break;
		case kDbgCmd_NetJoin:
		{
			NetJoinGame(args->at(0).c_str(),atoi(args->at(1).c_str()));
		}
		break;
		case kDbgCmd_NetStart:
		{
			NetHostGameStart();
		}
		break;
		case kDbgCmd_NetHostList:
		{
			NetClientMSList();
		}
		break;
		case kDbgCmd_SendChat:
		{
		string  TheChat;

			for(int i = 0; i < args->size(); ++i)
			{
				TheChat += args->at(i);
				TheChat += " ";
			}
			NetClientChat(TheChat);
		}
		break;
		case kDbgCmd_SendTurn:
		{
			NetClientTurn(atoi(args->at(0).c_str()),atoi(args->at(1).c_str()),atoi(args->at(2).c_str()),atoi(args->at(3).c_str()));
		}
		break;
		case kDbgCmd_GameName:
		{
			GameSetPlayerName(args->at(0).c_str());
		}
		break;
	}
}


void ReadCfgs(void)
{
FILE *fp;
char buf[256];
string mname;
int mport;
	// Metaserver stuff.
	fp = fopen("GameData/config/metaserver.cfg","r");
	if(fp)
	{
		fgets(buf,256,fp);// port
		mport = atoi(buf);
		dcon << "[CONFIG]\tBinding metaserver port to: " << mport << "\n";
		fgets(buf,256,fp);// name
		mname = buf;
		dcon << "[CONFIG]\tBinding metaserver name to: " << mname << "\n";
		fclose(fp);
	}
	else
	{
		mname = "127.0.0.1";
		mport = 2807;
	}
	NetCfgMetaInfo(mname,mport);



	// Music stuff.
	fp = fopen("GameData/config/bgm.cfg","r");
	if(fp)
	{
	int i;

		fgets(buf,256,fp);
		for(i = 0; i < 256; ++i) { if(buf[i] =='\n') { buf[i] = '\0'; break; } }
		bgm_Title = buf;
		fgets(buf,256,fp);
		for(i = 0; i < 256; ++i) { if(buf[i] =='\n') { buf[i] = '\0'; break; } }
		bgm_Lobby = buf;
		fgets(buf,256,fp);
		for(i = 0; i < 256; ++i) { if(buf[i] =='\n') { buf[i] = '\0'; break; } }
		bgm_Battle = buf;
		fclose(fp);
	}
}


