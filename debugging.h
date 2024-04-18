// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================


// -----------------------------------------------------------------------------
//                                                                         guard
// -----------------------------------------------------------------------------
#pragma warning (disable:4786)

#ifndef _TANK_DEBUGGING_H_
#define _TANK_DEBUGGING_H_


// -----------------------------------------------------------------------------
//                                                                      includes
// -----------------------------------------------------------------------------

// tank
#include "tank.h"


// -----------------------------------------------------------------------------
//                                                                     constants
// -----------------------------------------------------------------------------

// command constants
enum
{
	kDbgCmd_NetHost  = 0,
	kDbgCmd_NetJoin,
	kDbgCmd_NetStart,
	kDbgCmd_NetHostList,
	kDbgCmd_SendChat,
	kDbgCmd_SendTurn,
	kDbgCmd_GameName
};


// -----------------------------------------------------------------------------
//                                                                       classes
// -----------------------------------------------------------------------------

// forward declarations
class CDebugConsoleS;
class CDebugConsoleI;


// _____________________________________________________________________________
class CDebugConsoleS
//
// Provides interface to debug console singleton.
//
{
	public:
		// ct and dt
		CDebugConsoleS();
		~CDebugConsoleS();

		// instance accessor
		static CDebugConsoleI& GetInstance(void);


	private:
		// disabled
		CDebugConsoleS(const CDebugConsoleS &s);
		CDebugConsoleS& operator=(const CDebugConsoleS &rhs);

		// data members
		static int             mRefCount;
		static CDebugConsoleI* mInstance;
};


// _____________________________________________________________________________
class CDebugConsoleI
//
// Implements a debugging console.
//
{
	public:
		// ct and dt
		CDebugConsoleI(void);
		~CDebugConsoleI(void);

		// show/hide
		static void Show(void);
		static void Hide(void);
		static void Toggle(void);

		// output
		static void Write(const char *fmt,...);

		// input
		static bool ProcessKey(WPARAM key);

	protected:
		// setup
		static void nCreate(void);
		static void nDestroy(void);

		// routers
		static unsigned long _stdcall nThreadRouter(void *param);
		static long          _stdcall nMessageRouter(HWND wind,UINT msg,WPARAM wParam,LPARAM lParam);
	
		// handlers
		static void nHandleRedraw(void);

		// input buffer
		static void nInputBufferFlush(void);
		static void nInputBufferTokenize(string *cmd,vector<string> *args);
		static void nInputBufferExecute(string &cm,vector<string> &args);

		// output buffer
		static void nOutputBufferFlush(void);

		// disabled
		CDebugConsoleI(const CDebugConsoleI &s);
		CDebugConsoleI& operator=(const CDebugConsoleI &rhs);


		// data members
		static HANDLE         mThreadH;    // Handle to console thread.
		static unsigned long  mThreadID;   // ID of console thread.
		static unsigned long  mPThreadID;  // ID of parent thread.

		static HWND  mWindH;  // Handle to our window.
		static HDC   mDCH;    // Handle to our DC.

		static int  mWindSizeX;  // Window width.
		static int  mWindSizeY;  // Window height.

		static int  mCurX;  // Current X position of text output.
		static int  mCurY;  // Current Y position of text output.

		static int   mCharW;
		static int   mCharH;

		static HFONT mFontH;
		static HPEN  mPenH;

		static string					mPrevBuffer;			// Previous command buffer.
		static string         mInputBuffer;     // Input buffer.
		static deque<string>  mOutputBuffer;    // Output buffer.
		static string         mDanglingBuffer;  // Unterminated output buffer.

		static bool  mVisible;
};


// -----------------------------------------------------------------------------
//                                                                    prototypes
// -----------------------------------------------------------------------------

// console stream functions
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const string &rhs)        { lhs.Write("%s",rhs.c_str()); return (lhs); };
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const char *rhs)          { lhs.Write("%s",rhs); return (lhs); };
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const char rhs)           { lhs.Write("%c",rhs); return (lhs); };
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const unsigned char rhs)  { lhs.Write("%u",rhs); return (lhs); };
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const int rhs)            { lhs.Write("%d",rhs); return (lhs); };
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const unsigned int rhs)   { lhs.Write("%u",rhs); return (lhs); };
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const float rhs)          { lhs.Write("%f",rhs); return (lhs); };
static CDebugConsoleI& operator<<(CDebugConsoleI &lhs,const double rhs)         { lhs.Write("%f",rhs); return (lhs); };

// internal debug functions
bool  dbg_dassert(const char *f,unsigned int l,bool c,const char *m);

// -----------------------------------------------------------------------------
//                                                                        macros
// -----------------------------------------------------------------------------

// assertation macros
#define dassert(c)  dbg_dassert(__FILE__,__LINE__,(c),#c)

// accessor macros
#define dcon  CDebugConsoleS::GetInstance()


// -----------------------------------------------------------------------------
//                                                           singleton reference
// -----------------------------------------------------------------------------

// CDebugConsole
namespace { static CDebugConsoleS  debug_console_reference; }


#endif

