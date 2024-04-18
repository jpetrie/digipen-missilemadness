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
#include "mod_terrain.h"

#include "handler_redraw.h"

#include "error_handling.h"

#include "debugging.h"


// -----------------------------------------------------------------------------
//                                                                     functions
// -----------------------------------------------------------------------------

// _____________________________________________________________________________
void HandleRedraw(HWND wind,WPARAM wParam,LPARAM lParam)
//
// Handler for redraw (WM_PAINT) messages.
//
// --> wind    Handle to the window that caught the message.
// --> wParam  Message parameters.
// --> lParam  Message parameters.
//
{
PAINTSTRUCT  ThePS;
HDC          TheDC;

	TheDC = BeginPaint(wind,&ThePS);

	EndPaint(wind,&ThePS);
}

