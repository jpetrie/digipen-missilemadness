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

#include "mod_game.h"
#include "mod_network.h"

#include "debugging.h"

#include <mutex>

// -----------------------------------------------------------------------------
//                                                                static members
// -----------------------------------------------------------------------------

// CDebugConsoleS
int			        CDebugConsoleS::mRefCount = 0;
CDebugConsoleI* CDebugConsoleS::mInstance = 0;

// CDebugConsoleI
HANDLE         CDebugConsoleI::mThreadH;
unsigned long  CDebugConsoleI::mThreadID;
unsigned long  CDebugConsoleI::mPThreadID;

HWND  CDebugConsoleI::mWindH;
HDC   CDebugConsoleI::mDCH;

int  CDebugConsoleI::mWindSizeX;
int  CDebugConsoleI::mWindSizeY;

int  CDebugConsoleI::mCurX;
int  CDebugConsoleI::mCurY;

int   CDebugConsoleI::mCharH;
int   CDebugConsoleI::mCharW;

HFONT    CDebugConsoleI::mFontH;
HPEN     CDebugConsoleI::mPenH;

string         CDebugConsoleI::mPrevBuffer;
string         CDebugConsoleI::mInputBuffer;
deque<string>  CDebugConsoleI::mOutputBuffer;
string         CDebugConsoleI::mDanglingBuffer;

// Used to prevent concurrent access to the above buffers from multiple threads.
static std::mutex BufferMutex;

bool  CDebugConsoleI::mVisible;
extern bool first;


// -----------------------------------------------------------------------------
//                                                                     functions
// -----------------------------------------------------------------------------

// _____________________________________________________________________________
CDebugConsoleS::CDebugConsoleS(void)
//
// Default ct.
//
{
	// Reference the singleton.
	mRefCount++;
}


// _____________________________________________________________________________
CDebugConsoleS::~CDebugConsoleS(void)
//
// Destructor.
//
{
	// Unreference the singleton; delete if appropriate.
	if(0 == --mRefCount && mInstance)
	{
		delete mInstance;
	}
}


// _____________________________________________________________________________
CDebugConsoleI& CDebugConsoleS::GetInstance(void)
//
// Singleton instance accessor.
//
{
	if(!mInstance)
	{
		mInstance = new CDebugConsoleI();
	}

	return (*mInstance);
}


// _____________________________________________________________________________
CDebugConsoleI::CDebugConsoleI(void)
//
// Default ct.
//
{
	// Get the parent thread ID (current thread, at this point).
	mPThreadID = GetCurrentThreadId();

	// Spawn the handler thread and allow it to execute (to create its
	// message pump, the thread must call GetMessage() once).
	mThreadH = CreateThread(NULL,0,CDebugConsoleI::nThreadRouter,NULL,0,&mThreadID);

	// We probably managed to get lucky with this Sleep() call here; in practice, the first time
	// the dcon macro gets used we run this code to spawn the worker thread. Setting that thread up
	// involves manipulating the output buffer in the thread, but immediately after we return from
	// here we go into a Write() call via the operator<< overloads and also manipulate the output
	// buffer, leading to a crash. Sprinkling std::lock_guards around access to the input and output
	// buffers is a quick band-aid, but really this entire interface should get rewritten.
	Sleep(0);
}


// _____________________________________________________________________________
CDebugConsoleI::~CDebugConsoleI(void)
//
// Destructor.
//
{
	nDestroy();
}


// _____________________________________________________________________________
void CDebugConsoleI::Show(void)
//
// Shows the console; no effect if the console is already shown.
//
{
	if(!mVisible)
	{
		ShowWindow(mWindH,SHOW_OPENWINDOW);

		// If we're being debugged, don't float above everything because that's
		// really freakin' annoying.
		if(!IsDebuggerPresent())
		{
			SetWindowPos(mWindH,HWND_TOPMOST,0,0,mWindSizeX,mWindSizeY,0);
		}

		mVisible = true;
	}
}


// _____________________________________________________________________________
void CDebugConsoleI::Hide(void)
//
// Hides the console; no effect if the console is already hidden.
//
{
	if(mVisible)
	{
		ShowWindow(mWindH,HIDE_WINDOW);
		mVisible = false;
	}
}


// _____________________________________________________________________________
void CDebugConsoleI::Toggle(void)
//
// Toggles the visibility of the console.
//
{
	if(!mVisible)
	{
		Show();
	}
	else
	{
		Hide();
	}
}


// _____________________________________________________________________________
void CDebugConsoleI::Write(const char *fmt,...)
//
// Writes formatted output to the console.
//
// --> fmt  Specifies the format of the output, as with printf().
// --> ...  Specifies additional format data.
//
{
	std::lock_guard<std::mutex> guard(BufferMutex);
	
	
char                   TheBuffer[256];
string                 TheResult;
string                 TheScratch;
std::string::size_type ThePos;
va_list                TheAP;

	// This provides us printf() style variable-argument functionality.
	va_start(TheAP,fmt);
	vsprintf(TheBuffer,fmt,TheAP);
	va_end(TheAP);

	// Postprocess the result.
	TheResult = TheBuffer;
	ThePos    = 0;
	while((ThePos = TheResult.find_first_of("\t",ThePos)) != string::npos)
	{
		// Crude four-space tab hack.
		TheResult[ThePos] = ' ';
		TheResult.insert(ThePos,"   ");
		ThePos++;
	}

	// Find the first occurance of a newline character in the result.
	do
	{
		ThePos = TheResult.find_first_of("\n");
		if(ThePos == string::npos)
		// If not found, we tack the result onto the dangling buffer.
		{
			mDanglingBuffer += TheResult;
		}
		else
		// If it was found, we push the substring into the output buffer.
		{
			mOutputBuffer.push_front(mDanglingBuffer + TheResult.substr(0,ThePos));
			TheResult = TheResult.substr(ThePos + 1);
			mDanglingBuffer = "";
		}
	}
	while(ThePos != string::npos && TheResult.length() > 0);

	// Force a console redraw.
	InvalidateRect(mWindH,NULL,true);
}


// _____________________________________________________________________________
bool CDebugConsoleI::ProcessKey(WPARAM key)
//
// Handles input.
//
{
RECT  TheRect;

	if(!mVisible && key == '`')
	// Only handle the tab key (to show console) if we're invisible.
	{
		Show();
		return (true);
	}
	else if(mVisible)
	{
		switch(key)
		{
			case '`':
			// Hide console.
			{
				Hide();
				return (true);
			}
			break;
			case VK_RETURN:
			// Process input.
			{
			string          TheCmd;
			vector<string>  TheArgs;
			std::lock_guard<std::mutex> guard(BufferMutex);

				nInputBufferTokenize(&TheCmd, &TheArgs);
				nInputBufferFlush();
				nInputBufferExecute(TheCmd, TheArgs);

				InvalidateRect(mWindH,NULL,true);
				return (true);
			}
			break;
			case VK_BACK:
			// Delete the last character typed.
			{
				std::lock_guard<std::mutex> guard(BufferMutex);

				if(mInputBuffer.length() > 0)
				{
				string::iterator  TheIt;
					
					TheIt = mInputBuffer.end();
					TheIt--;
					mInputBuffer.erase(TheIt);
				}

				TheRect.left   = 0;
				TheRect.top    = mWindSizeY - mCharH;
				TheRect.right  = mWindSizeX;
				TheRect.bottom = mWindSizeY;
				InvalidateRect(mWindH,&TheRect,true);
				return (true);
			}
			break;
			default:
			{
				std::lock_guard<std::mutex> guard(BufferMutex);

				// Add the key pressed to the buffer.
				mInputBuffer += key;

				TheRect.left   = 0;
				TheRect.top    = mWindSizeY - mCharH;
				TheRect.right  = mWindSizeX;
				TheRect.bottom = mWindSizeY;
				InvalidateRect(mWindH,&TheRect,true);
				return (true);
			}
		}
	}
	
	return (false);
}
	
	
// _____________________________________________________________________________
void CDebugConsoleI::nCreate(void)
//
// Performs set up.
//
{
WNDCLASSEX  TheWindowClass; 
HBITMAP     TheBitmap;

	mWindSizeX = 800;
	mWindSizeY = 600 / 2;

	{
		std::lock_guard<std::mutex> guard(BufferMutex);

		// Flush buffers.
		nInputBufferFlush();
		nOutputBufferFlush();

		// Fix a bug where the first line isn't pushed to the buffer.
		mOutputBuffer.push_front("\n");
	}

	// Create window class.
	TheWindowClass.cbSize        = sizeof(WNDCLASSEX);
	TheWindowClass.style         = CS_HREDRAW | CS_VREDRAW;
	TheWindowClass.lpfnWndProc   = CDebugConsoleI::nMessageRouter;
	TheWindowClass.cbClsExtra    = 0;
	TheWindowClass.cbWndExtra    = 0;
	TheWindowClass.hInstance     = GetModuleHandle(0);
	TheWindowClass.hIcon         = LoadIcon(NULL,IDI_WINLOGO);
	TheWindowClass.hCursor       = LoadCursor(NULL,IDC_ARROW);
	TheWindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	TheWindowClass.lpszMenuName  = NULL;
	TheWindowClass.lpszClassName = "CDebugConsoleClass";
	TheWindowClass.hIconSm       = LoadIcon(NULL,IDI_WINLOGO);
	RegisterClassEx(&TheWindowClass);
	mWindH = CreateWindowEx(NULL,"CDebugConsoleClass","Debug Console",(WS_POPUP),0,0,mWindSizeX,mWindSizeY,NULL,NULL,GetModuleHandle(0),NULL);

	// Create backbuffer.
	mDCH = CreateCompatibleDC(GetDC(mWindH));
	TheBitmap = CreateCompatibleBitmap(mDCH,mWindSizeX,mWindSizeY);
	SelectObject(mDCH,TheBitmap);
	DeleteObject(TheBitmap);

	// Backbuffer text options.
	mCurX  = 0;
	mCurY  = 0;
	mCharW = 10;
	mCharH = 15;
	mPenH  = CreatePen(PS_SOLID,1,RGB(255,255,255));
	mFontH = CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,"Courier New");
	SelectObject(mDCH,mPenH);
	SelectObject(mDCH,mFontH);
	SetBkMode(mDCH,TRANSPARENT);
	SetTextColor(mDCH,RGB(255,255,255));

	mVisible = false;
}


// _____________________________________________________________________________
void CDebugConsoleI::nDestroy(void)
//
// Performs clean up.
//
{
	std::lock_guard<std::mutex> guard(BufferMutex);

	// Release GDI objects.
	DeleteObject(mPenH);
	DeleteObject(mFontH);

	// Ensure buffer is flushed
	nOutputBufferFlush();

	_CrtDumpMemoryLeaks();
}


// _____________________________________________________________________________
DWORD WINAPI CDebugConsoleI::nThreadRouter(LPVOID param)
//
// Message pump for the debug console thread.
//
// --> param  Thread function parameters. Unused.
// -R-        Thread function return. Unused, return 0.
//
{
MSG TheMessage;

	// Construct the console.
	nCreate();

	// Enter the message pump.
	while(GetMessage(&TheMessage,NULL,0,0))
	{
		TranslateMessage(&TheMessage);
		DispatchMessage(&TheMessage);
	}

	// If our message pump exits, we terminate the parent thread
	// as well.
	PostThreadMessage(mPThreadID,WM_QUIT,0,0);
	return (0);
}


// _____________________________________________________________________________
LRESULT _stdcall CDebugConsoleI::nMessageRouter(HWND wind,UINT msg,WPARAM wParam,LPARAM lParam)
//
// Message router for debug console window class.
//
// --> wind    Handle to the window object.
// --> msg     Message ID.
// --> wParam  Message parameter.
// --> lParam  Extended message parameters.
// -R-         Result code; True if the message was handled, false otherwise.
{
PAINTSTRUCT       ThePS;
HDC               TheDC;

	switch(msg)
	{
		case WM_PAINT:
		// Update the console. Blit from the backbuffer to the console.
		{
			nHandleRedraw();

			TheDC = BeginPaint(wind,&ThePS);
			BitBlt(TheDC,ThePS.rcPaint.left,ThePS.rcPaint.top,
						 (ThePS.rcPaint.right - ThePS.rcPaint.left),
						 (ThePS.rcPaint.bottom - ThePS.rcPaint.top), 
             mDCH,ThePS.rcPaint.left,ThePS.rcPaint.top,SRCCOPY);
			EndPaint(wind,&ThePS);
			return (true);
		}
		break;
		case WM_KEYDOWN:
		{
			if(wParam == VK_UP)
			{
				mInputBuffer = mPrevBuffer;
				InvalidateRect(mWindH,NULL,true);
			}
		}
		break;
		case WM_CHAR:
		// Handle keyboard input.
		{
			ProcessKey(wParam);
			return (true);
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


// _____________________________________________________________________________
void CDebugConsoleI::nHandleRedraw(void)
//
// Redraws the console window to the internal DC.
//
{
int   TheLineCount;
char  TheBuffer[256];

	// Flush the entire backbuffer.
	SelectObject(mDCH,GetStockObject(BLACK_BRUSH));
	Rectangle(mDCH,0,0,mWindSizeX,mWindSizeY);

	MoveToEx(mDCH,0,(mWindSizeY - mCharH - 4),NULL);
	LineTo(mDCH,mWindSizeX,(mWindSizeY - mCharH - 4));

	// Figure the visible line count of the output buffer.
	TheLineCount = (mWindSizeY - (2 * mCharH)) / mCharH;
	if(TheLineCount >= mOutputBuffer.size())
	{
		TheLineCount = mOutputBuffer.size() - 1;
	}
	TheLineCount--;

	// Then draw all the lines of the output buffer.
	for(int i = 0; i <= (TheLineCount); ++i)
	{
		strcpy(TheBuffer,mOutputBuffer[(TheLineCount - i)].c_str());
		TextOut(mDCH,2,(i * mCharH),TheBuffer,strlen(TheBuffer));
	}
	strcpy(TheBuffer,mDanglingBuffer.c_str());
	TextOut(mDCH,2,(abs(TheLineCount) * mCharH + mCharH),TheBuffer,strlen(TheBuffer));

	strcpy(TheBuffer,mInputBuffer.c_str());
	TextOut(mDCH,2,(mWindSizeY - mCharH - 2),TheBuffer,strlen(TheBuffer));
}


// _____________________________________________________________________________
void CDebugConsoleI::nInputBufferFlush(void)
//
// Flushes the line input buffer.
//
{
	mInputBuffer.erase(mInputBuffer.begin(),mInputBuffer.end());
}


// _____________________________________________________________________________
void CDebugConsoleI::nInputBufferTokenize(string *command,vector<string> *args)
//
// Splits the input buffer into tokens to be interpreted by the CDebugConsoleI::
// nInputBufferExecute() function.
//
// <-- command  The string containing the command; cannot be NULL.
// <-- args     A vector of strings specifing arguments; cannot be NULL.
//
{
int             ThePos;
string          TheBuffer;
string          TheScratch;
vector<string>  TheTokens;

	// Stupid-proofing.
	dassert(command != NULL);
	dassert(args != NULL);

	// Work on a scratch copy of the buffer (we're going to be modifying it).
	TheBuffer = mInputBuffer;

	// Split the buffer based on whitespace.
	do
	{
		ThePos = TheBuffer.find_first_of(" ");
		TheScratch = TheBuffer.substr(0,ThePos);
		TheBuffer  = TheBuffer.substr(ThePos + 1);
		TheTokens.push_back(TheScratch);
	}
	while(ThePos != string::npos);

	// The first token is considered the command.
	*command = TheTokens[0];
	TheTokens.erase(TheTokens.begin());

	// The rest are the arguments.
	*args = TheTokens;
}


// _____________________________________________________________________________
void CDebugConsoleI::nInputBufferExecute(string &command,vector<string> &args)
//
// Executes the command specified by `command`, optionally using the arguments
// passed in `args`. If the command is invalid, nothing happens.
//
// --> command  The string containing the command.
// --> args     A vector of strings specifing arguments.
//
{
vector<string> *TheArgs;
int             i;

	// Store the command.
	mPrevBuffer = command;
	for(i = 0; i < args.size(); ++i)
	{	
		mPrevBuffer += " ";
		mPrevBuffer += args[i];
	}

	if(command == "quit")
	{
		
		if(args.size() > 0 && args[0] == "-f")
		// Force quit.
		{
			exit(0);
		}
		else
		// Quit gracefully.
		{
			PostQuitMessage(0);
			return;
		}
	}
	
	// The remaining commands must be handled by the main thread; if they are called from within
	// this function they will execute in the debugger's thread, which not only defeats the purpose
	// of the multi-threading, but also introduces some serious potential for errors.
	// The handler is responsible for deleting the vector<string>* passed as the message LPARAM.
	TheArgs = new vector<string>(args);
	if(command == "host")
	{
		if(args.size() < 2)
		{
			Write("  usage: host <port> <game name>\n");
			return;
		}
		PostMessage(gMainWindow,WM_DEBUGGER,kDbgCmd_NetHost,reinterpret_cast<LPARAM>(TheArgs));
		return;
	}
	if(command == "join")
	{
		if(args.size() < 2)
		{
			Write("  usage: host <address> <port>\n");
			return;
		}
		PostMessage(gMainWindow,WM_DEBUGGER,kDbgCmd_NetJoin,reinterpret_cast<LPARAM>(TheArgs));
		return;
	}
	if(command == "start")
	{
		if(!NetHosting())
		{
			Write("  error: you're not the game host\n");
			return;
		}
		PostMessage(gMainWindow,WM_DEBUGGER,kDbgCmd_NetStart,reinterpret_cast<LPARAM>(TheArgs));
		return;
	}
	if(command == "games")
	{
		PostMessage(gMainWindow,WM_DEBUGGER,kDbgCmd_NetHostList,reinterpret_cast<LPARAM>(TheArgs));
		return;
	}
	if(command == "chat")
	{
		if(args.size() < 1)
		{
			Write("  usage: chat <player | *> <message>\n");
			return;
		}
		PostMessage(gMainWindow,WM_DEBUGGER,kDbgCmd_SendChat,reinterpret_cast<LPARAM>(TheArgs));
		return;
	}
	if(command == "turn")
	{
		if(args.size() < 4)
		{
			Write("  usage: turn <power> <angle> <speed> <magic>\n");
			return;
		}
		Hide();
		PostMessage(gMainWindow,WM_DEBUGGER,kDbgCmd_SendTurn,reinterpret_cast<LPARAM>(TheArgs));
		return;
	}
	if(command == "name")
	{
		if(args.size() < 1)
		{
			Write("  usage: name <player name>\n");
			return;
		}
		else if(NetHosting() || NetJoined())
		{
			Write("  error: you cannot change your name during a game\n");
			return;
		}
		PostMessage(gMainWindow,WM_DEBUGGER,kDbgCmd_GameName,reinterpret_cast<LPARAM>(TheArgs));
		return;
	}
}


// _____________________________________________________________________________
void CDebugConsoleI::nOutputBufferFlush(void)
//
// Flushes the output buffer.
//
{
	mOutputBuffer.erase(mOutputBuffer.begin(),mOutputBuffer.end());
}


// _____________________________________________________________________________
bool dbg_dassert(const char *f,unsigned int l,bool c,const char *m)
//
// Internal debugging function; do not call directly.
//
// Handles an assertation.
//
// --> f  Specifies the file that is asserting.
// --> l  Specifies the line that is asserting.
// --> c  Specifies the assertation condition.
// --> m  Specifies the assertation condition as a string.
// -R-    True if the assert was triggered, false otherwise.
//
{
char    TheMessage[256];
string  TheBuffer;
string  TheFilename;

	// Assert if the condition is false.
	if(!c)
	{
		// Hide the console.
		dcon.Hide();

		// Unmangle file name.
		TheBuffer = f;
		TheFilename = TheBuffer.substr(TheBuffer.find_last_of("\\") + 1,32);

		// Format message.
		sprintf(TheMessage,"%s %u: %s",TheFilename.c_str(),l,m);
		MessageBox(NULL,TheMessage,"Assertation Failed",(MB_OK | MB_ICONHAND));
		return (true);
	}
	return (false);
}