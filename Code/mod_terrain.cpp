// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================

#include "tank.h"

#include "handler_redraw.h"
#include "handler_network.h"

#include "mod_display.h"
#include "mod_network.h"
#include "mod_ui.h"
#include "error_handling.h"
#include "debugging.h"
#include "mod_terrain.h"

int gDamage = 0;
vector<Blast> vBlast;
vector<Blast>::iterator BlastIt;

void TerrainGenerate(int amplitude, unsigned char smooth)
{	
	
	//DisplayRect(0,0,1023, 767, BACK_G);
	//DisplayRect(0, k_toolbarH, SCREEN_WIDTH, SCREEN_HEIGHT, TOOLBAR);
	//	local variable declarations/initializations
	POINT left, right, 
		left_corner, right_corner, 
		far_left,
		current;
	POINT * p_list;
	short div = SCREEN_WIDTH - 1;
	short it = 1;
	short times;

	//	local variable assignments
	times = (short) pow(2, (double) smooth);
	p_list = new POINT[times * 2];
	left.x = 0;
	left.y = TERRAIN_DEF;
	right.x = div;
	right.y = TERRAIN_DEF;
	left.y += TerrainDeviate(50);
	right.y += TerrainDeviate(50);
	far_left = left;
	left_corner.x = 0;
	left_corner.y = k_toolbarH;
	right_corner.x = div;
	right_corner.y = k_toolbarH;
	
	for (int loop = 0; loop < (times + 3); ++loop)
	{
		p_list[loop].x = loop;
		p_list[loop].y = TERRAIN_DEF;
	}
	
	p_list[0] = left;
	p_list[times] = right;
	p_list[times + 1] = right_corner;
	p_list[times + 2] = left_corner;
	int curr_it = times;

	int j = 1;
	for (it = 1; it <= smooth; it++)
	{
		left = far_left;
		
		right = p_list[curr_it];

		short part = (short) pow(2, (double) it);
		
		short diff = times / part;
		short index = diff;
		int right_index_dx = (curr_it);
		int right_index = right_index_dx;

		for (j = 1; j <= (part / 2); j++)
		{
			
			current.x = (left.x + right.x) / 2;
			current.y = (left.y + right.y) / 2;
			current.y += TerrainDeviate(amplitude);
			
			p_list[index] = current;
			

			left = p_list[right_index];
			
			right_index += right_index_dx;
			right = p_list[right_index];
			
			index += (times / (pow((double)2, (double)(it - 1))));
		}
		curr_it /= 2;

	}
	DisplayPoly(p_list, times + 3, TERRAIN, TERRAIN);
	
	delete[] p_list;
	dcon << "TerrainGenerate . done\n" ;
}


long TerrainDeviate(int amplitude)
{
	
	float div;
	float num = (float)rand();	// use to determine up or down from current position
	float sign;
	if (num > 16383.0f)
	{
		sign = 1.0f;
		div = 32767.0f;
	}
	else
	{
		sign = (-1.0f);
		div = 4096.0f;
	}
	
	float dev = sign * (((float)num / div) * (float)amplitude);	// deviate;
	
	return (long) (dev/2);
}

void TerrainTrace(int terrain_color, Map * surface_map, int left, int right)
{
	//WINOBJ w;
	//w = WinRetrieve();
	//HDC TheDC;
	//TheDC = ghdcMem;
	if(left < 0)
		left = 0;
	if(right > SCREEN_WIDTH)
		right = SCREEN_WIDTH;


	for (int x = left; x < right; ++x)
	{
		int y = 0;
		while(GetPixel(ghdcMem, x, y) != terrain_color)
		{
			++y;
		}
		surface_map[x].yTop = y;
	}
	//ReleaseDC(gMainWindow, TheDC);
	dcon << "TerrainTrace . done\n";
}

void TerrainDraw(Map * surface_map, int color)
{
	//HPEN hpen = CreatePen(PS_SOLID, 1, color);
	//SelectObject(ghdcMem, hpen);
	for(int x = 0; x < SCREEN_WIDTH; ++x)
	{
		if(surface_map[x].isBroken == true)
		{
			
			DisplayLine(x, k_toolbarH, x, surface_map[x].yLow, TERRAIN);

			DisplayLine(x, surface_map[x].yMid, x, surface_map[x].yTop, TERRAIN);
		}
		if(surface_map[x].isBroken == false)
		{
			DisplayLine(x, k_toolbarH, x, surface_map[x].yTop, TERRAIN);
		}
	}
	//DeleteObject((HGDIOBJ)hpen);
	//dcon << "TerrainDraw . done\n";
}



int TerrainFall(Map * surface_map, int blast_x, int damage_radius, int index)
{
	int damage = 0;
	HDC TheDC;
	TheDC = ghdcMem;
	damage_radius += 10;
	int start_x, end_x;

	if((blast_x - damage_radius) < 0)
	{
		start_x = 0;
	}
	else
	{
		start_x = (blast_x - damage_radius);
	}

	if((blast_x + damage_radius) > SCREEN_WIDTH)
	{
		end_x = SCREEN_WIDTH;
	}
	else
	{
		end_x = (blast_x + damage_radius);
	}
/*	char buff[15];
	char buff1[15];
	char buff2[15];
	char buff3[15];
*/
	int x = start_x;
	damage = 0;
/*	DisplayText(x, surface_map[x].yTop - 70, itoa(index, buff, 10), 0x000000);
	DisplayText(x, surface_map[x].yTop - 50, itoa(surface_map[x].yTop, buff1, 10), 0x000000);
	DisplayText(x, surface_map[x].yMid - 30, itoa(surface_map[x].yMid, buff2, 10), 0x000000);
	DisplayText(x, 200, itoa(surface_map[x].yLow, buff3, 10), 0x000000);
*/
	if(vBlast[index].damage)
	{
		for (x = start_x; x <= end_x; ++x)
		{
			short count = 0;
			int y = surface_map[x].yTop;
			surface_map[x].yMid = y;
			surface_map[x].yLow = y;
			if (y < k_toolbarH)
			{
				
				
				y++;
				while ((GetPixel(TheDC, x, y) == TERRAIN) && y < (k_toolbarH))
				{
					surface_map[x].yMid = y;
					++y;
				}
				
				if(y <= (k_toolbarH))
				{
					surface_map[x].isBroken = true;
					
					(vBlast[index].damage)++;
					while((GetPixel(TheDC, x, y) != TERRAIN) && y < k_toolbarH)
					{
						++count;
						++y;
					}
					if(y < k_toolbarH)
					{
						surface_map[x].yLow = (y - 1);
					}
					else
					{
						surface_map[x].yLow = (k_toolbarH - 1);
					}
				}
				
			}
			
			if(count)
			{
				surface_map[x].yMid += 1;
				surface_map[x].yTop += 1;
				if(surface_map[x].yMid >= k_toolbarH)
					surface_map[x].yMid = k_toolbarH - 1;
				if(surface_map[x].yTop >= k_toolbarH)
					surface_map[x].yTop = k_toolbarH - 1;

				++damage;				

			}
			else
			{
				surface_map[x].isBroken = false;
			}
		}
		if(damage == 0)
		{
			vBlast[index].damage = false;
		}
		
	}
	TerrainDraw(surface_map, TERRAIN);
	return vBlast[index].damage;
	
}

