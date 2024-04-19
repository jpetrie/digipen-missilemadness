#include "mod_tank.h"
#include <windows.h>
#include <math.h>
#include "mod_weapons.h"
#include "mod_display.h"
extern int iTurn;
extern int iTurnOrderID;
extern bool thinking;
extern bool singlePlayer;
extern arrow arrow1;





extern bool launch;
extern bool mustSendReady;

vector<StDamageFloater> dfloaters;
vector<StDamageFloater> trfloaters;

/*initTank initializes tank points for drawing*/
void initTank(struct tank *t)
{
	int scale = 2;//scale tank factor
	
	//0 to 3 are points for tank body
	t->ptarray[0].x = -3 * scale;
	t->ptarray[0].y = -2 * scale;

	t->ptarray[1].x = 3 * scale;
	t->ptarray[1].y = -2 * scale;

	t->ptarray[2].x = 5 * scale;
	t->ptarray[2].y = 2 * scale;

	t->ptarray[3].x = -5 * scale;
	t->ptarray[3].y = 2 * scale;
	
	//8 to 11 used for tank cannon
	t->ptarray[4].x = -1 * scale;
	t->ptarray[4].y = -5 * scale;

	t->ptarray[5].x = 1 * scale;
	t->ptarray[5].y = -5 * scale;

	t->ptarray[6].x = 1 * scale;
	t->ptarray[6].y = 1 * scale;

	t->ptarray[7].x = -1 * scale;
	t->ptarray[7].y = 1 * scale;

	t->vector[0] = 0;
	t->vector[1] = 1;
	t->state = 0;
	t->tankangle = 0;//angle of tank's body
	t->speed = 1;
	t->color2 = CYAN;

	
	// jpetrie: added
	t->armor = 400;
	t->shield = 0;
	t->rjam = 0;
	t->tjam = 0;
	t->cloaked = false;
	t->decoy = NULL;
	t->dead = false;
	t->angle = 90;
	t->totalAngle = 0;
	t->angleTime = 0;

	t->lastLoc.x = 0;
	t->lastLoc.y = 0;
	
}

void drawArrow(struct tank *t,POINT newOrigin, POINT location)
{
	if(launch || mustSendReady) return;

	POINT ptnew[7];
	if(arrow1.updown == true)
	{
		arrow1.arrowTime++;

		if( arrow1.arrowTime == 40)
		{
			arrow1.updown = false;
		}
	}
	else
	{
		arrow1.arrowTime--;
		if(arrow1.arrowTime == 20)
		{
			arrow1.updown = true;
		}
	}

	for(int i = 0; i <= 6; i++)
	{
		ptnew[i].x = arrow1.ptarray[i].x + location.x + newOrigin.x;
		ptnew[i].y = arrow1.ptarray[i].y + location.y + newOrigin.y + - arrow1.arrowTime;
	}
	DisplayPoly(ptnew, 7, t->color2, t->color1);
}

void drawTank(struct tank *t,POINT location,double angle,double tankangle,HDC hdcMem)
{
	POINT ptnew[8];//temporary array of points
	
	angle = -angle + (pi / 2);//angle of shot and cannon relative to ground
	tankangle = -tankangle;//angle of tank itself

	int i = 0;//used for loop iterations
	double cosangle = cos(angle);
	double sinangle = sin(angle);
	homoPoint hP;
	hP.x = 0;
	hP.y = -3;
	hP.z = 1;

	
	matrix transTemp;
	
	matrix rotateTemp;

	matrix tranInverse;
	
	if(t->transOffset)
	{
		
	
		makeTranslationMatrix(transTemp,-(t->transOffset));
	
		makeRotateMatrix(rotateTemp,tankangle);

		tranInverse = makeTranInverseMatrix(transTemp); //trans = -1Trans

		matrixMultiply(tranInverse,rotateTemp); //rotate = rotate * trans
	
		matrixMultiply(tranInverse,transTemp); //transTemp = transTemp * rotateTemp

		matrixMultiply(tranInverse,t->total); //transTemp = transTemp * total

		t->total = tranInverse;
	}
	
	t->newOrigin = matrixPoint(t->total,hP);
	hP.x = 0;
	hP.y = -15;
	hP.z = 1;

	t->originCannon = matrixPoint(t->total, hP);
	
	//rotate and translate tank's trapezoid
	for(i = 0;i < 4;i++)
	{
		//figure out rounding value
		ptnew[i] = matrixPoint(t->total,t->ptarray[i]);
		
		//translate
		ptnew[i].x = ptnew[i].x + location.x ;
		ptnew[i].y = ptnew[i].y + location.y ;
	
	}
	
	//rotate and translate cannon
	for(i = 4;i < 8;i++)
	{
		//rotate
		ptnew[i].x = (int)((cosangle * t->ptarray[i].x) - (sinangle * t->ptarray[i].y) );
		ptnew[i].y = (int)((sinangle * t->ptarray[i].x) + (cosangle * t->ptarray[i].y) );
		//translate
		ptnew[i].x = ptnew[i].x + location.x  + t->newOrigin.x;
		ptnew[i].y = ptnew[i].y + location.y  + t->newOrigin.y - 5;
	}
	HPEN	hpen;
	hpen = CreatePen(PS_SOLID, 1, t->color2);
	SelectObject(hdcMem, hpen);

	DisplayPoly((ptnew + 4), 4, t->color2,t->color1);//display cannon
	
	DisplayCircle(t->newOrigin.x + location.x,
				   t->newOrigin.y + location.y,
				   6,
				   t->color2,
				   t->color1);
	
	DisplayPoly(ptnew, 4, t->color2, t->color1); //display body
	DeleteObject((HGDIOBJ)hpen);

}


void tankPhysics(tank *t,COLORREF mycolor,HDC hdcMem)
{
	POINT dummy;
	dummy.y = t->location.y;
	t->vector[1] = 1;//put tank in falling state
	if(t->tankangle)
	{
		t->angleTime++;
	}
	else
	{
		t->angleTime = 0;
	}



	t->tankangle = 0;
	t->state = pixelTest(t,t->location,t->tankangle, mycolor, hdcMem);
	//Sleep(15);

	t->vector[0] = 0;
	homoPoint temp1;
	temp1.x = -10;
	temp1.y = 4;
	temp1.z = 1;
	homoPoint temp2;
	temp2.x = 10;
	temp2.y = 4;
	temp2.z = 1;

	POINT newPoint1 = matrixPoint(t->total,temp1);
	POINT newPoint2 = matrixPoint(t->total,temp2);
	double slope;
	temp1.x = newPoint1.x;
	temp1.y = newPoint1.y;
	temp2.x = newPoint2.x;
	temp2.y = newPoint2.y;
	if(newPoint2.x - newPoint1.x)
	{
		
		slope = ((temp2.y - temp1.y) / (temp2.x - temp1.x));
		t->totalAngle = atan(slope);
	}
		
	


	if(t->state == 0)//tank is falling
	{
		t->vector[0] = 0;
		t->vector[1] = 1;
	}
	if(t->state == 1)
	{	//unbalanced on the right
			
		
		dummy.x = t->location.x;
		if(!(pixelSideTest(t,dummy, 1,t->tankangle, mycolor, hdcMem)))
		{
			if((t->location.x + 10) < SCREEN_WIDTH - 1)
			{
				
				int angleChange = -1;
				if(t->angleTime > 4)
				{
					angleChange = -2;
				}
				if(t->angleTime > 7)
				{
					angleChange = -3;
				}	
				if(t->totalAngle < (pi / 4))
				{
					t->tankangle = -1 * ratio;
				}
				if(t->totalAngle > 0)
				{
					t->vector[0] = 1;
				}
				
			}

		}
		
		t->vector[1] = 0;
			
	}

	if(t->state == 2)//move to left and increase angle
	{
		
		dummy.x = t->location.x;
		if(!(pixelSideTest(t,dummy, 0,t->tankangle, mycolor, hdcMem)))
		{
			if((t->location.x - 10) > 0)
			{
			
				int angleChange = 1;
				if(t->angleTime > 4)
				{
					angleChange = 2;
				}
				if(t->angleTime > 7)
				{
					angleChange = 3;
				}	

				if(t->totalAngle > -(pi / 4))
				{
					t->tankangle = 1 * ratio;
				}
				if(t->totalAngle < 0)
				{
					t->vector[0] = -1;
				}
			}
		}

		t->vector[1] = 0;
			
	}

	if(t->state == 3)
	{
		t->vector[0] = 0;
		t->vector[1] = 0;
	}

	if(((t->location.x - 10) <= 0) && t->vector[0] < 0)
	{
		t->vector[0] = 0;
	}
	if(((t->location.x + 10) >= SCREEN_WIDTH - 1) && t->vector[0] > 0)
	{
		t->vector[0] = 0;
	}

	if(t->vector[0])
	{
		t->location.x = t->location.x + (t->vector[0] * t->speed);
	}
	if(t->vector[1])
	{
		t->location.y = t->location.y + (t->vector[1] * t->speed);
	}

	
}

int pixelTest(tank *t,POINT location,double angle, COLORREF color, HDC hdc)
{
	int state = 0;//records the state of the tank
	
	
	homoPoint temp;
	POINT tempPoint;
	
	int tanklength = 10;//length of tank from center
	int tankheight = 4;//height of tank from center
	t->transOffset = 0;
	

	//run through the bottom left to bottom center points
	//and check for collision
	for(int i = -tanklength;i < 0;i++)
	{
		temp.x = i;
		temp.y = tankheight + 1;
		temp.z = 1;
		
		tempPoint = matrixPoint(t->total,temp );
		
		if(GetPixel(hdc,location.x + tempPoint.x,location.y + tempPoint.y) == color)
		{
			t->transOffset = i;
			state = 1;//tank has collision on left side
			
		}
		
		
	}
	//run through the bottom center to bottom right points
	//and check for collision
	for(int i = tanklength;i >= 0;i--)
	{

		
		temp.x = i;
		temp.y = tankheight + 1;
		temp.z = 1;
		tempPoint = matrixPoint(t->total,temp);


		if(GetPixel(hdc,location.x + tempPoint.x,location.y + tempPoint.y) == color)
		{
			if(state == 1)
			{
				t->transOffset = i;
				state = 3;//collsion on left and right sides
				return state;
			}
			else
			{
				t->transOffset = i;
				state = 2;//collision on right side
				return state;
			}
		}
		
	
	}
	//if state is 0 the tank is falling
	return state;
}



POINT matrixPoint(matrix &m2,homoPoint &pt)
{
	POINT temp;

	temp.x = m2.a[0][0] * pt.x + m2.a[0][1] * pt.y + m2.a[0][2] * pt.z;
	temp.y = m2.a[1][0] * pt.x + m2.a[1][1] * pt.y + m2.a[1][2] * pt.z;

	return temp;
}

void makeTranslationMatrix(matrix &T,int x)
{
	T.a[0][0] = 1;
	T.a[0][1] = 0;
	T.a[0][2] = x;

	T.a[1][0] = 0;
	T.a[1][1] = 1;
	T.a[1][2] = -4;//-4

	T.a[2][0] = 0;
	T.a[2][1] = 0;
	T.a[2][2] = 1;
}

matrix makeTranInverseMatrix(matrix &T)
{
	
	matrix temp;
	//matrix Temp2;
	temp.a[0][0] = T.a[0][0]; temp.a[0][1] = T.a[0][1]; temp.a[0][2] = -T.a[0][2];
	temp.a[1][0] = T.a[1][0]; temp.a[1][1] = T.a[1][1]; temp.a[1][2] = -T.a[1][2];
	temp.a[2][0] = T.a[2][0]; temp.a[2][1] = T.a[2][1]; temp.a[2][2] = T.a[2][2];

	return temp;
}

void makeRotateMatrix(matrix &R,double &angle)
{
	R.a[0][0] = cos(angle);
	R.a[0][1] = -sin(angle);
	R.a[0][2] = 0;

	R.a[1][0] = sin(angle);
	R.a[1][1] = cos(angle);
	R.a[1][2] = 0;

	R.a[2][0] = 0;
	R.a[2][1] = 0;
	R.a[2][2] = 1;
}



void matrixMultiply(matrix &m1,matrix &m2)
{
	matrix temp;
	temp.a[0][0] = m1.a[0][0]; temp.a[0][1] = m1.a[0][1]; temp.a[0][2] = m1.a[0][2];
	temp.a[1][0] = m1.a[1][0]; temp.a[1][1] = m1.a[1][1]; temp.a[1][2] = m1.a[1][2];
	temp.a[2][0] = m1.a[2][0]; temp.a[2][1] = m1.a[2][1]; temp.a[2][2] = m1.a[2][2];


	m1.a[0][0] = temp.a[0][0] * m2.a[0][0] + temp.a[0][1] * m2.a[1][0] + temp.a[0][2] * m2.a[2][0];
	m1.a[0][1] = temp.a[0][0] * m2.a[0][1] + temp.a[0][1] * m2.a[1][1] + temp.a[0][2] * m2.a[2][1];
	m1.a[0][2] = temp.a[0][0] * m2.a[0][2] + temp.a[0][1] * m2.a[1][2] + temp.a[0][2] * m2.a[2][2];

	m1.a[1][0] = temp.a[1][0] * m2.a[0][0] + temp.a[1][1] * m2.a[1][0] + temp.a[1][2] * m2.a[2][0];
	m1.a[1][1] = temp.a[1][0] * m2.a[0][1] + temp.a[1][1] * m2.a[1][1] + temp.a[1][2] * m2.a[2][1];
	m1.a[1][2] = temp.a[1][0] * m2.a[0][2] + temp.a[1][1] * m2.a[1][2] + temp.a[1][2] * m2.a[2][2];

	m1.a[2][0] = temp.a[2][0] * m2.a[0][0] + temp.a[2][1] * m2.a[1][0] + temp.a[2][2] * m2.a[2][0];
	m1.a[2][1] = temp.a[2][0] * m2.a[0][1] + temp.a[2][1] * m2.a[1][1] + temp.a[2][2] * m2.a[2][1];
	m1.a[2][2] = temp.a[2][0] * m2.a[0][2] + temp.a[2][1] * m2.a[1][2] + temp.a[2][2] * m2.a[2][2];

}


bool pixelSideTest(tank *t,POINT location, int testside,double angle, COLORREF color, HDC hdc)
{

	int tanklength = 10;//amount of pixels from tank center
	int tankheight = 4;//amount of pixels from tank center to ground
	int state = 0;//the state of the tank
	
	homoPoint temp;
	POINT tempPt;
	int i = 0;//i is the x value we are working with
	//if testside is zero, the left side of the tank is being tested
	//i is set to the far left extreme of the tank - 1. The point that
	//represents the bottom left extreme is rotated and translated so
	//that it can be tested with the terrain color.
	if(testside == 0)
	{
		//used to subtract 1
		i = -tanklength-1;
		temp.x = i;
		temp.y = 4;
		temp.z = 1;

		tempPt = matrixPoint(t->total,temp);
		
		if(GetPixel(hdc,location.x + tempPt.x,location.y + tempPt.y) == color)
		{
				return true;
		}
	}
	else
	{
		//get far right 
		i = tanklength + 1 ;//used to add 1
		temp.x = i;
		temp.y = 4;
		temp.z = 1;

		tempPt = matrixPoint(t->total,temp);
	
		if(GetPixel(hdc,location.x + tempPt.x,location.y + tempPt.y) == color)
		{
				return true;
		}
	}

	return false;
}



void UpdateFloaters(void)
{
char buf[256];

	for(int i = 0; i < dfloaters.size(); ++i)
	{
		// Fake sine wave kinda stuff.
		dfloaters[i].x += dfloaters[i].delta;
		if(abs(dfloaters[i].x - dfloaters[i].origx) >= 7)
			dfloaters[i].delta *= -1;

		sprintf(buf,"-%d",dfloaters[i].val);
		DisplayText(dfloaters[i].x,dfloaters[i].y,buf,dfloaters[i].color);
		
		--dfloaters[i].y;
		if(--dfloaters[i].lifetime <= 0)
		{
		vector<StDamageFloater>::iterator it = dfloaters.begin();

			for(int j = 0; j < i; ++j)
				it++;

			dfloaters.erase(it);
		}

	}
}

void TraceFloaters(void)
{
char buf[256];

	for(int i = 0; i < trfloaters.size(); ++i)
	{
		// Fake sine wave kinda stuff.
		trfloaters[i].x += trfloaters[i].delta;
		if(abs(trfloaters[i].x - trfloaters[i].origx) >= 7)
			trfloaters[i].delta *= -1;

		sprintf(buf,"%d",-1 * trfloaters[i].val);
		DisplayText(trfloaters[i].x,trfloaters[i].y,buf,trfloaters[i].color);
		
		//--dfloaters[i].y;
		if(--trfloaters[i].lifetime <= 0)
		{
		vector<StDamageFloater>::iterator it = trfloaters.begin();

			for(int j = 0; j < i; ++j)
				it++;

			trfloaters.erase(it);
		}

	}
}

