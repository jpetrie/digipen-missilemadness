//***********************************************************************//
//																		 //
//	Scott Smith															 //
//  Display module header												 //
//	1/14/03																 //
//																		 //
//***********************************************************************//
//#include "windows_init.h"

#pragma warning (disable:4786)

#ifndef __MOD_DISPLAYH__
#define __MOD_DISPLAYH__

#include "tank.h"

#define SCREEN_WIDTH  800									
#define SCREEN_HEIGHT 600									


typedef struct WINOBJ
{
	HWND		hwnd_local;
	HINSTANCE	hInstance_local;
}WINOBJ;

typedef struct BitMap
{
	const char *	file_name;
	short			width;
	short			height;

}BitMap;

const int BLACK		= 0x000000;		// black
const int BLUE		= 0xFF0000;		// blue
const int GREEN		= 0x00FF00;		// green
const int RED		= 0x0000FF;		// red
const int YELLOW	= 0x00FFFF;		// yellow
const int CYAN		= 0xFFFF00;		// cyan
const int MAGENTA	= 0xFF00FF;		// magenta
const int TOOLBAR	= 0x555555;		// grey
const int DKBLUE	= 0x300000;		// midnight
const int ORANGE	= 0x0958FF;
const int BACK_G	= 0x000030;
//const int BACK_G	= 0x003030;
//const int BACK_G	= 0x222222;
const int LTBLUE	= 0xFFDDDD;		// light blue
const int TERRAIN	= 0x007700;		// grass green
const int MISS_IN	= 0xFF0000;
const int MISS_OUT  = 0xAA0000;
const int CHAT_WIN  = 0xBBBBBB;

void ChangeToFullScreen(int width, 
						int height);

void DisplayCompRes(void);

void DisplayPoint(short x, 
				  short y, 
				  int color);

void DisplayRect(short start_x, 
				 short start_y, 
				 short end_x, 
				 short end_y, 
				 int fill_color, 
				 int outline_color = 0x000000);

void DisplayLine(short start_x, 
				 short start_y, 
				 short end_x, 
				 short end_y, 
				 int line_color);

void DisplayEllipse(short	top_left_bound_x, 
					short	top_left_bound_y, 
					short	bottom_right_bound_x,
					short	bottom_right_bound_y,
					int		fill_color);

void DisplayCircle(short	center_x,
				   short	center_y,
				   short	radius,
				   int		outline_color,
				   int		fill_color);

void DisplayPoly(POINT * point_list, short number_of_points, 
									 int outline_color, 
									 int fill_color);

void DisplayBitmap(BitMap stBitmap, short x, short y);

void DisplayBlast(short center_x, short center_y, short radius, int pallette);

WINOBJ WinRetrieve(void);

void DisplayText(short x, short y, const char * text_to_display, int color);

void DisplayStartup(HINSTANCE inst,HWND wind);


#endif

