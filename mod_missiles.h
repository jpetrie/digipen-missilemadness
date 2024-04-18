#ifndef MOD_MISSILE_H
#define MOD_MISSILE_H
#pragma warning (disable:4786)
#include <windows.h>


//extern struct missile missile1;
void missileInit(struct missile *m,int num);

struct missile
{
	POINT ptarray[30];
	int numofmissile;//number of missiles fired
	double anglediff[20];
	bool draw[20];
	
	//Echan  vv
	POINT origin[20];
	int frameAdj[20];//for adjusting the frames of "spawned" missiles

	double changex[20];
	double changey[20];
	double prevChangex[20];
	double prevChangey[20];
	
	COLORREF objColor[20];
	int scale;
	int objIdentifier[20];
	int numObjects;
};

#endif