
//#include "weapons.h"
#include "mod_missiles.h"
#include "mod_weapons.h"



void missileInit(struct missile *m,int num)
{
	int scale = 1;
	//echan
	/*m[0].ptarray[0].x = -2 * scale;
	m[0].ptarray[0].y = -4 * scale;
	m[0].ptarray[1].x = 2 * scale;
	m[0].ptarray[1].y = -4 * scale;
	m[0].ptarray[2].x = 2 * scale;
	m[0].ptarray[2].y = 4 * scale;
	m[0].ptarray[3].x = -2 * scale;
	m[0].ptarray[3].y = 4 * scale;

	m[0].ptarray[4].x = -4 * scale;
	m[0].ptarray[4].y = -4 * scale;
	m[0].ptarray[5].x = 0 * scale;
	m[0].ptarray[5].y = -8 * scale;
	m[0].ptarray[6].x = 4 * scale;
	m[0].ptarray[6].y = -4 * scale;

	m[0].ptarray[7].x = -2 * scale;
	m[0].ptarray[7].y = 4 * scale;
	m[0].ptarray[8].x = 2 * scale;
	m[0].ptarray[8].y = 4 * scale;
	m[0].ptarray[9].x = 4 * scale;
	m[0].ptarray[9].y = 6 * scale;
	m[0].ptarray[10].x = -4 * scale;
	m[0].ptarray[10].y = 6 * scale;*/
	//echan

	m[0].numofmissile = 20;
	m[0].draw[0] = false;
	m[0].draw[1] = false;
	m[0].draw[2] = false;
	m[0].draw[3] = false;
	m[0].draw[4] = false;

	m[0].anglediff[0] = 0 * ratio;
	m[0].anglediff[1] = 0 * ratio;
	m[0].anglediff[2] = 0 * ratio;
	m[0].anglediff[3] = 0 * ratio;
	m[0].anglediff[4] = 0 * ratio;
				
	/*m[0].objIdentifier[0] = polygon4;
	m[0].objIdentifier[1] = polygon3;
	m[0].objIdentifier[2] = polygon4;
	m[0].numObjects = 3;*/

		
	
}


