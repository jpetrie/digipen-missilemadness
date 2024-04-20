//***********************************************************************//
//																		 //
//	Scott Smith															 //
//  Display module														 //
//	1/15/03																 //
//	last update 1/30/03													 //
//***********************************************************************//

#include "tank.h"
							
#include "mod_display.h"

#include "error_handling.h"

#include "debugging.h"


static HWND hwnd;
HINSTANCE ghInstance;


void ChangeToFullScreen(int width, int height)
{
	DEVMODE dmSettings;									// Device Mode variable - Needed to change modes

	memset(&dmSettings,0,sizeof(dmSettings));			// Makes Sure Memory's Cleared

	// Get current settings -- This function fills our the settings
	// This makes sure NT and Win98 machines change correctly
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		// Display error message if we couldn't get display settings
		errorDisplayAndExit(ErrorCode::DisplayResolution);
		return;
	}

	dmSettings.dmPelsWidth	= width;					// Set the desired Screen Width
	dmSettings.dmPelsHeight	= height;					// Set the desired Screen Height
	dmSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;	// Set the flags saying we're changing the Screen Width and Height
	
	// This function actually changes the screen to full screen
	// CDS_FULLSCREEN Gets Rid Of Start Bar.
	// We always want to get a result from this function to check if we failed
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	// Check if we didn't recieved a good return message From the function
	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		// Display the error message and quit the program
		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}

void DisplayPoint(short x, short y, int color)
{
	//HDC		TheDC; 
	
	//TheDC = ghdcMem;
	SetPixel(ghdcMem, x, y, color);
	//ReleaseDC(gMainWindow, TheDC);
}

void DisplayRect(short start_x, short start_y, short end_x, short end_y, int fill_color, int outline_color)
{
	//HDC		TheDC; 
	HBRUSH brush = CreateSolidBrush(fill_color);
	HPEN	pen = CreatePen(PS_SOLID, 1, outline_color);
	
	//TheDC = ghdcMem;
	SelectObject(ghdcMem,brush );
	SelectObject(ghdcMem,pen);
	Rectangle(ghdcMem, start_x, start_y, end_x, end_y);
	//ReleaseDC(gMainWindow, TheDC);
	DeleteObject((HGDIOBJ)brush);
	DeleteObject((HGDIOBJ)pen);
}

void DisplayLine(short start_x, short start_y, short end_x, short end_y, int line_color)
{
	//HDC		TheDC; 
	
	HPEN	hpen;
	LPPOINT lp_point = NULL;
	
	//TheDC = ghdcMem;
	hpen = CreatePen(PS_SOLID, 1, line_color);
	SelectObject(ghdcMem, hpen);
	MoveToEx(ghdcMem, start_x, start_y, lp_point); 
	LineTo(ghdcMem, end_x, end_y);
	//ReleaseDC(gMainWindow, ghdcMem);
	DeleteObject((HGDIOBJ)hpen);
}

void DisplayEllipse(short	top_left_bound_x, 
					short	top_left_bound_y, 
					short	bottom_right_bound_x,
					short	bottom_right_bound_y,
					int		fill_color)
{
	
	//HDC		TheDC; 
	
	//TheDC = ghdcMem;
	
	SelectObject(ghdcMem, CreateSolidBrush(fill_color));
	Ellipse(ghdcMem, top_left_bound_x,
						top_left_bound_y, 
						bottom_right_bound_x, 
						bottom_right_bound_y);
	//ReleaseDC(gMainWindow, TheDC);
} 

void DisplayCircle(short	center_x,
				   short	center_y,
				   short	radius,
				   int		outline_color,
				   int		fill_color)
{

	//HDC		TheDC; 
	HPEN hpen;
	HBRUSH brush;

	
	
	//TheDC = ghdcMem;
	brush = CreateSolidBrush(fill_color);
	SelectObject(ghdcMem, brush);
	hpen = CreatePen(PS_SOLID, 1, outline_color);
	SelectObject(ghdcMem, hpen);
	Ellipse(ghdcMem, center_x - radius,
						center_y - radius, 
						center_x + radius, 
						center_y + radius);
	//ReleaseDC(gMainWindow, TheDC);
	DeleteObject((HGDIOBJ)hpen);
	DeleteObject((HGDIOBJ)brush);
}

void DisplayPoly(POINT * point_list, short number_of_points, 
									 int outline_color, 
									 int fill_color)
{
	//HDC		TheDC; 
	HPEN	hpen;
	HBRUSH brush = CreateSolidBrush(fill_color);
		
	//TheDC = ghdcMem;
	SelectObject(ghdcMem, brush);
	hpen = CreatePen(PS_SOLID, 1, outline_color);
	SelectObject(ghdcMem, hpen);
	

	Polygon(ghdcMem, point_list, number_of_points);
	//ReleaseDC(gMainWindow, TheDC);
	DeleteObject((HGDIOBJ)hpen);
	DeleteObject((HGDIOBJ)brush);
}

void DisplayBitmap(BitMap stBitmap, short x, short y)
{
              
	//HDC		TheDC; 
	
	HBITMAP hbitmap = (HBITMAP)LoadImage(gAppInstance, stBitmap.file_name, IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	//TheDC = ghdcMem;
	
	HDC image_dc = CreateCompatibleDC(ghdcMem);	
	
	HBITMAP old_hbitmap = (HBITMAP)SelectObject(image_dc,hbitmap);
	BitBlt(ghdcMem, x, y, stBitmap.width, stBitmap.height, image_dc, 0, 0, SRCCOPY);
	//ReleaseDC(gMainWindow, TheDC);
	DeleteObject(hbitmap);
	DeleteObject(image_dc);
	DeleteObject(old_hbitmap);
}

void DisplayCompRes(void)
{
	int bits, planes, comp;
	//HDC TheDC;
	//TheDC = ghdcMem;
	bits = GetDeviceCaps(ghdcMem, BITSPIXEL);
	planes = GetDeviceCaps(ghdcMem, PLANES);
	comp = bits * planes;
	if(comp < 32)
	{
		errorDisplayAndExit(ErrorCode::DisplayDepth);
	}
}


void DisplayBlast(short center_x, short center_y, short radius, int pallette)
{
    char color	= 255 / (1.25 * radius),
		red		= 0,
		blue	= 0,
		green	= 0;
	
	
	for (int i = 0; i <= radius; i++)
	{
		switch(pallette)
		{
		case RED:
			{
				red += color;
				break;
			}
		case BLUE:
			{
				blue += color;
				break;
			}
		case GREEN:
			{
				green += color;
				break;
			}
		case YELLOW:
			{
				red += color;
				green+= color;
				break;
			}
		case MAGENTA:
			{
				blue += color;
				red += color;
				break;
			}
		case CYAN:
			{
				blue += color;
				green += color;
				break;
			}
		}
		DisplayCircle(center_x, center_y, i, 0x000000, RGB(red, green, blue));
		Sleep(10);
		if(i > (radius / 50))
		{
			DisplayCircle(center_x, center_y, i - (radius / 50), RED, RED);
		}
		
	}
	
	
	DisplayCircle(center_x, center_y, radius + 1, BACK_G, BACK_G);
	
	
}

void DisplayText(short x, short y, const char * text_to_display, int color)
{
	
	//HDC		TheDC; 
	string in_text = text_to_display;
	
	//TheDC = ghdcMem;
	SetTextColor(ghdcMem, color);
	SetBkMode(ghdcMem, TRANSPARENT);		// make background transparent
	
	TextOut(ghdcMem, x, y, text_to_display, (int) in_text.length());
	//ReleaseDC(gMainWindow, TheDC);
}


WINOBJ WinRetrieve(void)
{
	WINOBJ w;

	w.hwnd_local = hwnd;
	w.hInstance_local = ghInstance;

	return w;
}

void DisplayStartup(HINSTANCE inst,HWND wind)
{
	hwnd = wind;
	ghInstance = inst;
}

