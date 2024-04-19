//***********************************************************************//
//																		 //
//	Scott Smith															 //
//  Terrain generator header											 //
//	1/16/03																 //
//																		 //
//***********************************************************************//

#pragma warning (disable:4786)
#include <windows.h>										
#include <stdio.h>	
#include <stdlib.h>	
#include <vector>
#include "mod_display.h"


#ifndef __TERRAINH__
#define __TERRAINH__



#define TERRAIN_DEF (int)((SCREEN_HEIGHT * 6) / 10)

extern int gDamage;


struct Blast
{
	short radius;
	int	color;
	int damage;
	int x_pos;
	int y_pos;
};

struct Map
{
	bool isBroken;
	int yTop;
	int yMid;
	int yLow;
};

extern vector<Blast> vBlast;
extern vector<Blast>::iterator BlastIt;

long TerrainDeviate(int amplitude);
void TerrainGenerate(int amplitude, unsigned char smooth);
void TerrainTrace(int terrain_color, Map * surface_map, int left = 0, int right = SCREEN_WIDTH);
void TerrainDraw(Map * surface_map, int color);
int TerrainFall(Map * surface_map, int blast_x, int damage_radius, int index);



#endif
