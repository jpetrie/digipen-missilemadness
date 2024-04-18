
#ifndef _TANKGAME_MOD_TANK_H_
#define _TANKGAME_MOD_TANK_H_
#pragma warning (disable:4786)
#include <windows.h>
#include <string>
#include <vector>
using namespace std;


#define MAX_ITEMS    5
#define MAX_WEAPONS  10

void initTank(struct tank *t);

void drawTank(struct tank *t,POINT location,double angle,double tankangle,HDC hdc);

int pixelTest(tank *t,POINT location,double angle, COLORREF color, HDC hdc);

bool pixelSideTest(tank *t,POINT location, int testside,double angle, COLORREF color, HDC hdc);

void tankPhysics(tank *t,COLORREF mycolor, HDC hdcMem);

struct homoPoint
{
	double x;
	double y;
	double z;
	homoPoint()
	{
		x = 0;
		y = 0;
		z = 1;
	}
	homoPoint(const homoPoint &r)
	{
		x = r.x;
		y = r.y;
		z = r.z;
	}

};
struct matrix
{
	double a[3][3];

	matrix()
	{
		a[0][0] = 1; a[0][1] = 0; a[0][2] = 0;

		a[1][0] = 0; a[1][1] = 1; a[1][2] = 0;

		a[2][0] = 0; a[2][1] = 0; a[2][2] = 1;
	}

	matrix(const matrix &r)
	{
		a[0][0] = r.a[0][0]; a[0][1] = r.a[0][1]; a[0][2] = r.a[0][2];
		a[1][0] = r.a[1][0]; a[1][1] = r.a[1][1]; a[1][2] = r.a[1][2];
		a[2][0] = r.a[2][0]; a[2][1] = r.a[2][1]; a[2][2] = r.a[2][2];
	}



};



struct tank
{
	homoPoint ptarray[8];
	std::string  name;
	POINT location;
	int vector[2];
	int state;
	double tankangle;//angle of tank's body
	int speed;
	COLORREF color1;
	COLORREF color2;
	POINT originCannon;
	POINT newOrigin; // top middle of tank
	POINT lastLoc;

	// jpetrie: added for gameplay purposes
	int armor;
	int shield;
	int rjam;
	int tjam;
	int durations;	// Bytepacked, high -> low: ignored, radar jam, thermal jam, shield.
	int cash;
	bool cloaked;
	tank* decoy;
	double angle;
	bool dead;
	matrix total;
	matrix justTranslate;
	int transOffset; //point of impact
	double totalAngle; //angle of tank's body at any given point in time
	int angleTime;
	int item[MAX_ITEMS];    // -1 is an empty slot.
	int weap[MAX_WEAPONS];  // -1 is an empty slot.
	int item_ctr;
	int weap_ctr;

	int aiWeaponCount;
};

struct arrow
{
	arrow()
	{
		ptarray[0].x = -4;
		ptarray[0].y = 0;

		ptarray[1].x = -2;
		ptarray[1].y = 0;

		ptarray[2].x = -2;
		ptarray[2].y = -8;

		ptarray[3].x = 2;
		ptarray[3].y = -8;

		ptarray[4].x = 2;
		ptarray[4].y = 0;

		ptarray[5].x = 4;
		ptarray[5].y = 0;

		ptarray[6].x = 0;
		ptarray[6].y = 4;

		arrowTime = 20;
		updown = true;
	}

	bool updown;
	int arrowTime;
	POINT ptarray[7];
};


void matrixMultiply(matrix &m1,matrix &m2);

void makeRotateMatrix(matrix &R,double &angle);
matrix makeTranInverseMatrix(matrix &T);
void makeTranslationMatrix(matrix &T,int x);

POINT matrixPoint(matrix &m2,homoPoint &pt);



struct StDamageFloater
{
	int delta;
	int x;
	int y;
	int val;
	int lifetime;
	int origx;
	int color;
};

void drawArrow(struct tank *t,POINT newOrigin, POINT location);
void UpdateFloaters(void);

void TraceFloaters(void);

extern vector<StDamageFloater> dfloaters;
extern vector<StDamageFloater> trfloaters;

#endif


