// =============================================================================
// Missile Madness
//
// (c) 2003 DigiPen Institute of Technology
// =============================================================================

#include "error_handling.h"

#include <string>

#include <Windows.h>

extern HWND gMainWindow;

// _____________________________________________________________________________
void errorDisplayAndExit(ErrorCode code)
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
	std::string message = "An unknown fatal error has occured. Whatever you just did, it wasn't such a hot idea.";

	switch (code) {
		case ErrorCode::DisplayResolution:
			message = "Could not switch resolutions; it's probably time to upgrade that 13\" monitor.";
			break;
		case ErrorCode::DisplayDepth:
			message = "Please set your monitor to 24-bit color or higher. Anything less is for wimps. You're not a wimp are you?";
			break;
		case ErrorCode::FailedToJoin:
			message = "Unable to join the specified game. It's probably full, or they've decided to start without you. They obviously don't like you.";
			break;
		case ErrorCode::Unknown:
			break;
	}

	MessageBox(gMainWindow, message.c_str(), "Fatal Error", (MB_OK | MB_ICONEXCLAMATION));
	std::exit(EXIT_FAILURE);
}
