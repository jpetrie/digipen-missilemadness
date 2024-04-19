#pragma warning(disable: 4786)
#include <windows.h>
#include <math.h>
#include "mod_weapons.h"
#include "mod_missiles.h"
#include "mod_terrain.h"
#include "mod_game.h"
#include "mod_ui.h"
#include "mod_tank.h"
#include "mod_weapon_attributes.h"
#include "debugging.h"

extern Weapon m[NUMWEAPONS]; //From mod_game.cpp //echan

double accCONST = -.13; //acceleration due to gravity constant


const double pi = 3.1415926535;//pi constant
const double ratio = pi / 180;//conversion to radians factor
const double DConstant = .08;

static void collideMissile(POINT ptnew[6], Weapon *w, int count, int polygon);

///////////////////////////////////////////////////////////////////////////////////////////////
/*This function is used to draw missiles in flight. A struct that holds the info for the 
particular missile needs to be passed. If also needs the location of the object which is 
passed as a point data type. The angle in which the missile is pointed is needed along with
the amount of distance the missile has moved in the x and y direction.*/
///////////////////////////////////////////////////////////////////////////////////////////////
void drawMissile(Weapon *w,POINT location,double polyangle,double xchange, double ychange,HDC hdcMem, int count)
{
	int i = 0;//keeps track of our position in ptarray of missile struct
	int j = 0;
	int currentObj = 0;//keeps track of what polygon of missile we are drawing

	double cosangle = cos(polyangle);//get cos of angle of slope of missile
	double sinangle = sin(polyangle);//get sin of angle of slope of missile

	int xintchange = (int)(xchange + .5);//round the change is position to an int
	int yintchange = (int)(ychange + .5);

	POINT ptnew[6];//place to store translated points

	//we repeat this loop until all polygons are drawn
	while(currentObj != w->WeaponMissile.numObjects)
	{
		//draw the polygon specified by the current position in objIdentifier
		switch(w->WeaponMissile.objIdentifier[currentObj])
		{
		/*The next four switch statements specify the polygon to draw. The points of the 
		polygon are then rotated and translated. The i variable is incremented by the number
		of points that make up the polygon. And the current object variable is incremented*/
		case polygon3:


			ptnew[0].x = (int)((cosangle * w->WeaponMissile.ptarray[i].x) - (sinangle * w->WeaponMissile.ptarray[i].y) + .5);
			ptnew[0].y = (int)((sinangle * w->WeaponMissile.ptarray[i].x) + (cosangle * w->WeaponMissile.ptarray[i].y) + .5);

			ptnew[1].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 1].x) - (sinangle * w->WeaponMissile.ptarray[i + 1].y) + .5);
			ptnew[1].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 1].x) + (cosangle * w->WeaponMissile.ptarray[i + 1].y) + .5);

			ptnew[2].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 2].x) - (sinangle * w->WeaponMissile.ptarray[i + 2].y) + .5);
			ptnew[2].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 2].x) + (cosangle * w->WeaponMissile.ptarray[i + 2].y) + .5);

			ptnew[0].x = ptnew[0].x + xintchange + location.x;
			ptnew[0].y = ptnew[0].y + yintchange + location.y;

			ptnew[1].x = ptnew[1].x + xintchange + location.x;
			ptnew[1].y = ptnew[1].y + yintchange + location.y;

			ptnew[2].x = ptnew[2].x + xintchange + location.x;
			ptnew[2].y = ptnew[2].y + yintchange + location.y;

			if(w->WeaponMissile.draw[count] == true)	
			collideMissile(ptnew, w, count, w->WeaponMissile.objIdentifier[currentObj]);
			
			if(w->WeaponMissile.draw[count] == true)
				DisplayPoly(ptnew, 3, w->WeaponMissile.objColor[currentObj], w->WeaponMissile.objColor[currentObj]);


		//	Polygon(hdcMem,ptnew,3);
			i = i + 3;
			currentObj++;
			break;


		case polygon4:

			ptnew[0].x = (int)((cosangle * w->WeaponMissile.ptarray[i].x) - (sinangle * w->WeaponMissile.ptarray[i].y) + .5);
			ptnew[0].y = (int)((sinangle * w->WeaponMissile.ptarray[i].x) + (cosangle * w->WeaponMissile.ptarray[i].y) + .5);

			ptnew[1].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 1].x) - (sinangle * w->WeaponMissile.ptarray[i + 1].y) + .5);
			ptnew[1].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 1].x) + (cosangle * w->WeaponMissile.ptarray[i + 1].y) + .5);

			ptnew[2].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 2].x) - (sinangle * w->WeaponMissile.ptarray[i + 2].y) + .5);
			ptnew[2].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 2].x) + (cosangle * w->WeaponMissile.ptarray[i + 2].y) + .5);

			ptnew[3].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 3].x) - (sinangle * w->WeaponMissile.ptarray[i + 3].y) + .5);
			ptnew[3].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 3].x) + (cosangle * w->WeaponMissile.ptarray[i + 3].y) + .5);

			ptnew[0].x = ptnew[0].x + xintchange + location.x;
			ptnew[0].y = ptnew[0].y + yintchange + location.y;

			ptnew[1].x = ptnew[1].x + xintchange + location.x;
			ptnew[1].y = ptnew[1].y + yintchange + location.y;

			ptnew[2].x = ptnew[2].x + xintchange + location.x;
			ptnew[2].y = ptnew[2].y + yintchange + location.y;

			ptnew[3].x = ptnew[3].x + xintchange + location.x;
			ptnew[3].y = ptnew[3].y + yintchange + location.y;

			if(w->WeaponMissile.draw[count] == true)
			collideMissile(ptnew, w, count, w->WeaponMissile.objIdentifier[currentObj]);

		//	Polygon(hdcMem,ptnew,4);
			if(w->WeaponMissile.draw[count] == true)
				DisplayPoly(ptnew, 4, w->WeaponMissile.objColor[currentObj], w->WeaponMissile.objColor[currentObj]);
			i = i + 4;
			currentObj++;
			break;

		case polygon5:

			ptnew[0].x = (int)((cosangle * w->WeaponMissile.ptarray[i].x) - (sinangle * w->WeaponMissile.ptarray[i].y) + .5);
			ptnew[0].y = (int)((sinangle * w->WeaponMissile.ptarray[i].x) + (cosangle * w->WeaponMissile.ptarray[i].y) + .5);

			ptnew[1].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 1].x) - (sinangle * w->WeaponMissile.ptarray[i + 1].y) + .5);
			ptnew[1].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 1].x) + (cosangle * w->WeaponMissile.ptarray[i + 1].y) + .5);

			ptnew[2].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 2].x) - (sinangle * w->WeaponMissile.ptarray[i + 2].y) + .5);
			ptnew[2].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 2].x) + (cosangle * w->WeaponMissile.ptarray[i + 2].y) + .5);
				
			ptnew[3].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 3].x) - (sinangle * w->WeaponMissile.ptarray[i + 3].y) + .5);
			ptnew[3].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 3].x) + (cosangle * w->WeaponMissile.ptarray[i + 3].y) + .5);

			ptnew[4].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 4].x) - (sinangle * w->WeaponMissile.ptarray[i + 4].y) + .5);
			ptnew[4].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 4].x) + (cosangle * w->WeaponMissile.ptarray[i + 4].y) + .5);
							
			ptnew[0].x = ptnew[0].x + xintchange + location.x;
			ptnew[0].y = ptnew[0].y + yintchange + location.y;

			ptnew[1].x = ptnew[1].x + xintchange + location.x;
			ptnew[1].y = ptnew[1].y + yintchange + location.y;

			ptnew[2].x = ptnew[2].x + xintchange + location.x;
			ptnew[2].y = ptnew[2].y + yintchange + location.y;

			ptnew[3].x = ptnew[3].x + xintchange + location.x;
			ptnew[3].y = ptnew[3].y + yintchange + location.y;

			ptnew[4].x = ptnew[4].x + xintchange + location.x;
			ptnew[4].y = ptnew[4].y + yintchange + location.y;

			if(w->WeaponMissile.draw[count] == true)
			collideMissile(ptnew, w, count, w->WeaponMissile.objIdentifier[currentObj]);

		//	Polygon(hdcMem,ptnew,5);
			if(w->WeaponMissile.draw[count] == true)
				DisplayPoly(ptnew, 5, w->WeaponMissile.objColor[currentObj], w->WeaponMissile.objColor[currentObj]);
			i = i + 5;
			currentObj++;
			break;

		case polygon6:

			ptnew[0].x = (int)((cosangle * w->WeaponMissile.ptarray[i].x) - (sinangle * w->WeaponMissile.ptarray[i].y) + .5);
			ptnew[0].y = (int)((sinangle * w->WeaponMissile.ptarray[i].x) + (cosangle * w->WeaponMissile.ptarray[i].y) + .5);

			ptnew[1].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 1].x) - (sinangle * w->WeaponMissile.ptarray[i + 1].y) + .5);
			ptnew[1].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 1].x) + (cosangle * w->WeaponMissile.ptarray[i + 1].y) + .5);

			ptnew[2].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 2].x) - (sinangle * w->WeaponMissile.ptarray[i + 2].y) + .5);
			ptnew[2].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 2].x) + (cosangle * w->WeaponMissile.ptarray[i + 2].y) + .5);

			ptnew[3].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 3].x) - (sinangle * w->WeaponMissile.ptarray[i + 3].y) + .5);
			ptnew[3].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 3].x) + (cosangle * w->WeaponMissile.ptarray[i + 3].y) + .5);

			ptnew[4].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 4].x) - (sinangle * w->WeaponMissile.ptarray[i + 4].y) + .5);
			ptnew[4].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 4].x) + (cosangle * w->WeaponMissile.ptarray[i + 4].y) + .5);

			ptnew[5].x = (int)((cosangle * w->WeaponMissile.ptarray[i + 5].x) - (sinangle * w->WeaponMissile.ptarray[i + 5].y) + .5);
			ptnew[5].y = (int)((sinangle * w->WeaponMissile.ptarray[i + 5].x) + (cosangle * w->WeaponMissile.ptarray[i + 5].y) + .5);
							
			ptnew[0].x = ptnew[0].x + xintchange + location.x;
			ptnew[0].y = ptnew[0].y + yintchange + location.y;

			ptnew[1].x = ptnew[1].x + xintchange + location.x;
			ptnew[1].y = ptnew[1].y + yintchange + location.y;

			ptnew[2].x = ptnew[2].x + xintchange + location.x;
			ptnew[2].y = ptnew[2].y + yintchange + location.y;

			ptnew[3].x = ptnew[3].x + xintchange + location.x;
			ptnew[3].y = ptnew[3].y + yintchange + location.y;

			ptnew[4].x = ptnew[4].x + xintchange + location.x;
			ptnew[4].y = ptnew[4].y + yintchange + location.y;

			ptnew[5].x = ptnew[5].x + xintchange + location.x;
			ptnew[5].y = ptnew[5].y + yintchange + location.y;

			if(w->WeaponMissile.draw[count] == true)
			collideMissile(ptnew, w, count, w->WeaponMissile.objIdentifier[currentObj]);

		//	Polygon(hdcMem,ptnew,6);
			if(w->WeaponMissile.draw[count] == true)
				DisplayPoly(ptnew, 6, w->WeaponMissile.objColor[currentObj],w->WeaponMissile.objColor[currentObj]);
			i = i + 6;
			currentObj++;
			break;

		}

	}
		
}
////////////////////////////////////////////////////////////////////////////////////
/*This function calculates the position of a missile during a specific time in flight
The count parameter is used to access individual missiles within one missile structure.
The force parameter is the initial velocity. The angle and device context of buffer is
also needed.*/
////////////////////////////////////////////////////////////////////////////////////
void calcMissile(Weapon *w,int count, double force,double angle,int numFrame,HDC hdcMem)
{

	double xforce = 0;//x component of force
	double yforce = 0;//y component of force
	double polyangle = 0;//angle of missile in flight
	double slope = 0;//slope of missiles body
	double acc = 0;//distance lost in y due to gravity

	//the force is divided according to the desired angle.                                                            
	//there is an x force and y force acting upon the missile
	xforce = cos(angle + w->WeaponMissile.anglediff[count]) * force;
	yforce = sin(angle + w->WeaponMissile.anglediff[count]) * force;
	
	//Frame adjust
	int numFrameOld = numFrame; //echan
	(numFrame -= w->WeaponMissile.frameAdj[count]);	//echan

	//F
	POINT position = {0,0}, origin = w->WeaponMissile.origin[count];

	//the amount of velocity due to acceleration constant of gravity is calculated
	acc = (w->WeaponFlagIsSet( "DoubleGravity" )?2:1)*( (w->WeaponFlagIsSet("DefiesGravity"))? 0 :accCONST * numFrame * numFrame);

	//x changes according to the amount of x force and wind resistance
	w->WeaponMissile.changex[count] = w->WeaponMissile.changex[count] + xforce; //xchange + xforce;
		
	//y changes according to the amount of y force and gravity
	w->WeaponMissile.changey[count] = (-yforce * numFrame) - acc;
		
	position.x = w->WeaponMissile.changex[count] + origin.x;
	position.y = w->WeaponMissile.changey[count] + origin.y;
	//if x has changed then we are not dealing with a vertical line
	if(w->WeaponMissile.changex[count] > 0)
	{
		//the tangent line of the parabola at this nuw->WeaponMissile.Fraw->WeaponMissile.e which gives us the angle
		slope = (-(w->WeaponMissile.changey[count] - w->WeaponMissile.prevChangey[count]) / (w->WeaponMissile.changex[count] - w->WeaponMissile.prevChangex[count]));
		polyangle = (pi / 2) - atan(slope);
	}
	else if(w->WeaponMissile.changex[count] < 0)//if x is negative 
	{
		slope = (-(w->WeaponMissile.changey[count] - w->WeaponMissile.prevChangey[count]) / (w->WeaponMissile.changex[count] - w->WeaponMissile.prevChangex[count]));
		polyangle =  ((3 * pi)/2) - atan(slope); //+ (pi / 2);
	}
	else
	{	//this is the vertical line value;
		polyangle = 0;
	}


	((!count) && w->WeaponShouldDeploySub(count, polyangle, position) && (!w->EmptyHull) )?w->WeaponDeploySub(position, polyangle, angle, numFrame):0;
	{
		(w->WeaponFlagIsSet("Drunk"))?(w->WeaponMissile.changey[count] += rand()%10,w->WeaponMissile.changex[count] += rand()%10):0;
		(w->WeaponName == Radar_Missile)? m[w->WeaponSub_Weapon].WeaponMissile.draw[0] = false: 0;//echan

		drawMissile( w, origin, polyangle,w->WeaponMissile.changex[count],w->WeaponMissile.changey[count],hdcMem, count);
		//echan if the draw member has been set to false by collide missile (has collided)
		( (!w->EmptyHull)&&(w->WeaponSub_WeaponDetonationCondition == Detonation) && ( w->WeaponMissile.draw[count] == false) )?w->WeaponDeploySub(position, polyangle, angle, numFrameOld):0;//echan
		w->WeaponMissile.prevChangex[count] = w->WeaponMissile.changex[count];//store prev x distance for slope calculation
	    w->WeaponMissile.prevChangey[count] = w->WeaponMissile.changey[count];//store prev y distance for slope calculation	
	}

}


static void collideMissile(POINT ptnew[6], Weapon *w, int count, int polygon)
{ 
	if((ptnew[0].x > (SCREEN_WIDTH - 1)) || (ptnew[0].x < 0))
	{
	//	for(int hacky = 0; hacky < 20; hacky++)
		w->WeaponMissile.draw[count] = false;
		//echanVV
		/*w->WeaponMissile.prevChangex[count] = 0;//reset variables in missile struct
		w->WeaponMissile.prevChangey[count] = 0;
		w->WeaponMissile.changex[count] = 0;
		w->WeaponMissile.changey[count] = 0;*/
		//echan^^
	}
	else
	{
		for(int j = 0; j < (polygon + 3); ++j)
		{
			bool collide = false;
			
			POINT temp;
			
			for(int i = 0; i < iTankCount; ++i)
			{
				temp.x = iTank[i].newOrigin.x + iTank[i].location.x;
				temp.y = iTank[i].newOrigin.y + iTank[i].location.y;
				if( (ptnew[j].x > (temp.x - 10)) &&
					(ptnew[j].x < (temp.x + 10)) &&
					(ptnew[j].y > (temp.y - 10))  &&
					(ptnew[j].y < (temp.y + 10)))
				{
					collide = true;
					break;
				}
			}

			if(GetPixel(ghdcMem, ptnew[j].x, ptnew[j].y) == TERRAIN || ptnew[j].y >= k_toolbarH)
				collide = true;
			
			
			if(collide)
			{
			//	dcon << "Collision!\n";
			//	for(int hacky = 0; hacky < 20; hacky++)
				w->WeaponMissile.draw[count] = false;
				//echanVV
				/*w->WeaponMissile.prevChangex[count] = 0;//reset variables in missile struct
				w->WeaponMissile.prevChangey[count] = 0;
				w->WeaponMissile.changex[count] = 0;
				w->WeaponMissile.changey[count] = 0;*/
				//echan^^
			
				
				Blast b;
				b.damage = true;
				if(w->WeaponTerrainEffect == Additive)
					b.color = TERRAIN;
				else 
					b.color = RED;

				b.radius = w->WeaponDamageRadius;
				b.x_pos = ptnew[j].x;
				b.y_pos = ptnew[j].y;
				vBlast.push_back(b);
				w->WeaponPlayDetonationSound();
				//echan VV
				//( (w->Sub_WeaponDetonationCondition == Detonation) && (!w->EmptyHull) )?WeaponDeploySub(POINT(0,0),0,PI/2,0):0;
				
				//echan ^^

				
				if(w->WeaponFlagIsSet("Vectored"))
				{
					POINT pa[3];
					pa[0].x = ptnew[j].x;
					pa[0].y = ptnew[j].y + 20;
					pa[1].x = ptnew[j].x + 120;
					pa[1].y = ptnew[j].y - 60;
					pa[2].x = ptnew[j].x - 120;
					pa[2].y = ptnew[j].y - 60;
					DisplayPoly(pa, 3, BLUE, LTBLUE);
					DisplayCircle(temp.x, temp.y, 6, LTBLUE, LTBLUE);
				}
				else if(w->WeaponTerrainEffect != NoEffect)
					DisplayCircle(b.x_pos, b.y_pos, b.radius, b.color, b.color); 
				
				// if dirtball
				if(w->WeaponTerrainEffect == Additive)
					TerrainTrace(TERRAIN, iSurfaceMap, ptnew[j].x - b.radius, b.x_pos + b.radius);
				
				StDamageFloater ang;
			//	dcon << w->WeaponNameString.c_str() << "\n";
						if(w->WeaponName == Tracer && (j == 0)  )
						{
							
							ang.origx = b.x_pos;
							ang.x = ang.origx;
							ang.y = b.y_pos - 30;
							ang.delta = 0;
							ang.val = ((w->WeaponMissile.anglediff[count]) * (180 / pi));
							ang.lifetime = 50;
							ang.color = LTBLUE;
							trfloaters.push_back(ang);
						//	dcon << "tracer\n";
							
						}

			
				for(int i = 0; i < iTankCount; ++i)
				{
				StDamageFloater df;
			
					temp.x = iTank[i].newOrigin.x + iTank[i].location.x;
					temp.y = iTank[i].newOrigin.y + iTank[i].location.y;
					if( (temp.x > (b.x_pos - b.radius) && 
						temp.x < (b.x_pos + b.radius)  ) &&
						temp.y > (b.y_pos - b.radius) &&
						temp.y < (b.y_pos + b.radius))
					{
						int damage = w->WeaponDirectDamage;
						if(w->WeaponDamageFalloff > 0)
						{
							damage *= (b.radius - (sqrt(( (abs( b.x_pos - temp.x) ) * \
								(abs ( b.x_pos - temp.x) ) ) + (abs(b.y_pos - temp.y) \
									* abs(b.y_pos - temp.y))))) / b.radius;
							damage /= w->WeaponDamageFalloff;
						}
					
						//damage /= (w->totalPtCount);
						df.origx = iTank[i].location.x;
						df.x = df.origx;
						df.y = iTank[i].location.y - 20;
						df.delta = 1;
						df.val = damage; //* (w->totalPtCount);// * w->WeaponMissile.numObjects * w->WeaponMissile.numObjects;
						df.lifetime = 50;

						if(w->WeaponFlagIsSet("Shield-Piercing"))
						{
							iTank[i].armor -= damage;
							break;
						}
						
						
						
						if((iTank[i].shield - damage) > 0)
						{
							iTank[i].shield -= damage;
						}
						else
						{
							iTank[i].armor -= (damage - iTank[i].shield);
							iTank[i].shield = 0;
						}

						if(iTank[i].armor <= 0)
						{
							iTank[i].armor = 0;
							df.color = RED;
						}
						else
							df.color = 0xFFFFFF;

						if(df.val != 0)
							dfloaters.push_back(df);
					}
				}

				TerrainFall(iSurfaceMap, ptnew[j].x, b.radius, vBlast.size() - 1);
				
			}
			break;
		}
	}

}

bool AfterEffects(Weapon& weapon, int count)
{
	//static POINT position;
	POINT toPoint;
	const  int spread = 1;
	static int distance = 20; 
	
	Blast b;

	if( !( weapon.WeaponFlagIsSet("ArtilleryTarget") ) )
	{
		int i = 0;
		
		weapon.WeaponMissile.prevChangex[count] = 0;//reset variables in missile struct
		weapon.WeaponMissile.prevChangey[count] = 0;
		weapon.WeaponMissile.changex[count] = 0;
		weapon.WeaponMissile.changey[count] = 0;
		
		return true;
	}
	else
	{
		toPoint.x = weapon.WeaponMissile.origin[count].x + weapon.WeaponMissile.changex[count];  
		toPoint.y = weapon.WeaponMissile.origin[count].y + weapon.WeaponMissile.changex[count]; 
		
		for(int i = distance; i < 20; i++)
		{
		DisplayLine(SCREEN_WIDTH/2, 0, toPoint.x + i * spread, iSurfaceMap[toPoint.x].yTop + 10, RGB(distance *30, distance , distance * 4) );
		DisplayLine(SCREEN_WIDTH/2, 0, toPoint.x - i * spread, iSurfaceMap[toPoint.x].yTop + 10, RGB(distance  *30, distance , distance * 4) );
		}
		
		if(!(distance % 5) || (distance == 1) )
		{
			DisplayCircle(toPoint.x + distance*spread, iSurfaceMap[toPoint.x].yTop + 10, 7, BACK_G, BACK_G);
			
				b.damage = true;
				b.color = BLUE;
				b.radius = 5;
				b.x_pos = toPoint.x + distance*spread;
				vBlast.push_back(b);

				DisplayCircle(toPoint.x - distance*spread, iSurfaceMap[toPoint.x].yTop + 10, 7, BACK_G, BACK_G);

				b.x_pos = toPoint.x - distance*spread;
				vBlast.push_back(b);
		
				TerrainFall(iSurfaceMap, toPoint.x - distance*spread, 7, vBlast.size() - 1);				
		}

		distance--;
		
		if(distance <= 0)
		{
		distance = 20;
		weapon.WeaponMissile.prevChangex[count] = 0;//reset variables in missile struct
		weapon.WeaponMissile.prevChangey[count] = 0;
		weapon.WeaponMissile.changex[count] = 0;
		weapon.WeaponMissile.changey[count] = 0;
		return true;
		
		}

	
	}
return false;
	
}