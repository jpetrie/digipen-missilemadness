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

#include "error_handling.h"

#include "debugging.h"


// -----------------------------------------------------------------------------
//                                                                     functions
// -----------------------------------------------------------------------------

// _____________________________________________________________________________
void ThrowError(int code)
//
// Displays a fatal error message window. Fatal errors cannot be recovered from
// and only present the user with an 'exit' option.
//
// The message displayed is looked up from a string table based on the value of
// the error code passed to the function.
//
// --> code  Error code to throw.
//
{
char  TheBuffer[256];

	LoadString(gAppInstance,code,TheBuffer,256);
	MessageBox(gMainWindow,TheBuffer,"Fatal Error",(MB_OK | MB_ICONEXCLAMATION));

	exit(1);
}

