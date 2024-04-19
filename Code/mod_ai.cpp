

#include "mod_weapons.h"
#include "mod_missiles.h"
#include "mod_tank.h"
#include "windows.h"
#include "math.h"
#include "mod_network.h"
#include "mod_weapon_attributes.h"
#include "debugging.h"
extern double accCONST;
extern double compForce;
extern tank	*iTank;
extern const double pi;
extern Weapon m[10];

int difficulty = 5;
void computerAI()
{

	//computer AI for turn duration
	iTank[1].angle = 0;
	
	double xforce = 3;//x component of force is always 3
	double yforce = 0;//y component of force
	static int previousX = 0;

	//d = distance = forceX * time
	double d = iTank[0].location.x - iTank[1].location.x;

	
	
	if(abs(d) > 200)
	{
		xforce = 4;
	}
	if(abs(d) > 400)
	{
		xforce = 6;
	}

	if(abs(d) > 700)
	{
		xforce = 7;
	}


	double t = abs((d / xforce));//calculate time needed to reach tank

	//calculate the y force needed to stay in the air for needed time
	yforce = ((iTank[0].location.y - iTank[1].location.y + (accCONST * pow(t,2))) / t);
	//calulate the total velocity
	compForce = sqrt(pow(xforce,2) + pow(yforce,2));

	//calculate the appropriate angle
	if(d <= 0)
	{
		iTank[1].angle = (acos((-xforce / compForce)));
	}
	else
	{
		iTank[1].angle = acos((xforce / compForce));
	}
	
	//pick a random number
	double  error = rand() % difficulty;
	int LorR = rand() % 2;//pick another random number
	//if the player's tank has not moved since last turn, decrease error 
	if(previousX == iTank[0].location.x)
	{
		if(error <= 2)
		{
			error = 0;
		}
		else
		{
			error = error - 2;
		}

	}
	//divide error by a constant
	error = error / 100;
	//if Left or Right is 1 then add error
	//if Left or Right is 0 then subtract error
	if(LorR)
	{
		iTank[1].angle = iTank[1].angle + error;
	}
	else
	{
		iTank[1].angle = iTank[1].angle - error;
	}
	iTank[1].angle  = iTank[1].angle * (180 / pi);
	

	if(m[iTank[1].weap[iTank[1].aiWeaponCount]].WeaponName == Tracer)
	{
		iTank[1].weap[iTank[1].aiWeaponCount] = FlakC;
	}

	if(m[iTank[1].weap[iTank[1].aiWeaponCount]].WeaponName == Carpet)
	{
		compForce = compForce * 2;
		dcon << "Carpet";
	}

	if(m[iTank[1].weap[iTank[1].aiWeaponCount]].WeaponName == Jackhammer)
	{
		compForce = compForce * .71;
		dcon << "JackHammer";
	}

	if(m[iTank[1].weap[iTank[1].aiWeaponCount]].WeaponName == FlakC)
	{
		compForce = compForce * .71;
		dcon << "FlakC";
	}

	if(m[iTank[1].weap[iTank[1].aiWeaponCount]].WeaponName == Chain_Gun)
	{
		compForce = compForce * .71;
		dcon << "Chain_Gun";
	}

	

	if(m[iTank[1].weap[iTank[1].aiWeaponCount]].WeaponName == EMP)
	{
		iTank[1].weap[iTank[1].aiWeaponCount] = UberC;
	}

	if(iTank[1].aiWeaponCount == (MAX_WEAPONS - 1))//did you run out of weapons
	{
		NetClientTurn(compForce, iTank[1].angle,1,0);//shoot standard cannon
	}
	else
	{
		//shoot next availible weapon
		NetClientTurn(compForce, iTank[1].angle,iTank[1].weap[iTank[1].aiWeaponCount],0);
	}

	if(iTank[1].aiWeaponCount < (MAX_WEAPONS - 1))
	{
		iTank[1].aiWeaponCount++;
	}
	//store the previous position of players tank
	previousX = iTank[0].location.x;

}