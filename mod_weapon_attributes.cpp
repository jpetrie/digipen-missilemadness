//***********************************************************************//
//																		 //
//	Eric Roberts														 //
//  Weapon effects module header										 //
//	3/18/03																 //
//																		 //
//***********************************************************************//
#pragma warning(disable : 4786)




#include "mod_weapon_attributes.h"
#include <fstream>
#include "tank.h"
#include "mod_display.h"
#include "mod_tank.h"
/*map<string, int> Weapon::FlagMap;
map<string, int> Weapon::EnumerationMap;
map<string, char> Weapon::WeaponNameMap;*/

extern const double pi;
extern const double ratio; //echan
extern Weapon m[NUMWEAPONS];//ECHAN
extern tank* iTank;
extern int iTankCount;
extern int iTurn;
static int tankIndex;
/******************************************************************************
Name
Function
******************************************************************************/
Weapon::Weapon() //Proper order?
: WeaponDamageRadius(0), WeaponCost(0), SubCount(0), SimulSubCount(0), RemainingBombs(0), 
	NextDrop(0), WeaponDirectDamage(0), WeaponClipSize(0), WeaponDamageFalloff(0), 
	WeaponTerrainEffect(0), EmptyHull(false), WeaponName(0), WeaponSub_Weapon(0), 
	WeaponDetonationType(0), WeaponSub_WeaponDetonationCondition(None), WeaponSub_WeaponDetonationMode(Fireworks), 
	WeaponEffectFlags(0)
{
	WeaponMissile.objColor[0] = YELLOW;
	missileInit(&WeaponMissile, 3);
	//static bool unset  = true;
	//(isMapSet)?0:WeaponSetMaps(), isMapSet = true;
	WeaponSetMaps();
	WeaponMissile.numofmissile = 5;
	/*ofstream blu("fi.txt");
	blu<<sizeof(*this)<<endl<<sizeof(Weapon);*/
}

/******************************************************************************
Name
Function
******************************************************************************/
Weapon::~Weapon()
{
/*	if (WeaponSub_Weapon)
	delete WeaponSub_Weapon;*/
}

/******************************************************************************
Name
Function
******************************************************************************/
const Weapon& Weapon::operator=(const Weapon& rhside)
{
	/*WeaponDamageRadius  = rhside.WeaponDamageRadius;
	int WeaponCost rhside.WeaponC;
	short SubCount;
	short SimulSubCount;
	short WeaponDirectDamage;
	short WeaponClipSize;
	float WeaponDamageFalloff;
	signed char WeaponTerrainEffect;
	
	bool EmptyHull;

	string WeaponDetonationSound; 
	
	string WeaponFiringSound;
	
	char WeaponName;
	
	long WeaponDetonationType;
	
	Weapon* WeaponSub_Weapon;
	
	Sub_WeaponDetonationConditionE WeaponSub_WeaponDetonationCondition;
	
	Sub_WeaponDetonationModeE WeaponSub_WeaponDetonationMode;
	
	Flags WeaponEffectFlags;
*/
	return (rhside);
}

/******************************************************************************
Name
Function
******************************************************************************/
void Weapon::Read(string filename, string name)
{
 
 //Weapon Name as enum
 char WNString[ItemNameLength];
 char FieldString[ItemNameLength];
 char trash[800];
 ifstream file( filename.c_str() );
 


 //Loop from here
 while( file.peek() != EOF )
 {
	 
	 //Character '|' means the rest of the line is the name of the Weapon 
	 if ( file.get() == '|' )
	 {
		file.getline(WNString, ItemNameLength);
		
		string NameCheck(WNString);

		if(name == NameCheck) //Found weapon
		{
			WeaponName = WeaponNameMap[NameCheck];
			WeaponNameString = NameCheck;

			//Each weapon's description is separated by a return
			while(file.peek() != '\n' && file.peek() != EOF) 
			{
				file.getline(FieldString, ItemNameLength, ':');
				
				FindMember(FieldString, file);

				file.getline(FieldString, ItemNameLength, '\n');
				//file.ignore(ItemNameLength, '\n');
			}
		}	
	 }
	 else
	 {
		 //Getline into some trash, or use ignore
		 file.getline(trash, 80);
	 }
 }//To here
 //Initialization belongs here?
	this->EmptyHull = false;
	this->RemainingBombs = SubCount;
 //
 file.close();
 ReInitWeapon();
 //Weapon Name as string

}

/******************************************************************************
Name
Function: Ugly
******************************************************************************/
void Weapon::FindMember(string field, ifstream& file)
{
	//Take value from file and assign to appropriate field then return

	if(field == "DirectDamage")
	{
	file>>this->WeaponDirectDamage;
	return;
	}

	if(field == "DamageRadius")
	{
	file>>this->WeaponDamageRadius;
	return;
	}

	if(field == "DamageFalloff")
	{
	file>>this->WeaponDamageFalloff;
	return;
	}

	if(field == "Cost")
	{
	file>>this->WeaponCost;
	return;
	}

	if(field == "ClipSize")
	{
	file>>this->WeaponClipSize;
	return;
	}

	if(field == "DetonationSound")
	{
	file>>this->WeaponDetonationSound;
	return;
	}
	if(field == "FiringSound")
	{
	file>>this->WeaponFiringSound;
	return;
	}	
	if(field == "TerrainEffect")
	{
		string temp;
		file>>temp;
		if(temp == "Subtractive")
		{
			this->WeaponTerrainEffect = Subtractive;
			return;
		}
		if(temp == "Additive")
		{
			this->WeaponTerrainEffect = Additive;
			return;
		}
		if(temp == "NoEffect")
		{
			this->WeaponTerrainEffect = NoEffect;
		}
			return;
	}
	
	if(field == "SubCount")
	{
	file>>this->SubCount;
	return;
	}

	if(field == "SimulSubCount")
	{
	file>>this->SimulSubCount;
	return;
	}
	
	if(field == "PointList")
	{
	WeaponGetPoints(file);
	return;
	}


	//Beginning of enumeration type fields, have to be converted

	if(field == "DetonationType")
	{
	file>>this->WeaponDetonationType;
	return;
	}

	if(field == "Sub_Weapon")
	{
		string temp;
		file>>temp;
		this->WeaponSub_Weapon = WeaponNameMap[temp];
		return;
	}

	if(field == "Sub_WeaponDetonationCondition")
	{
	WeaponSub_WeaponDetonationCondition = (Sub_WeaponDetonationConditionE)WeaponDiscernEnum(file);
	return;
	}

	if(field == "Sub_WeaponDetonationMode")
	{
	WeaponSub_WeaponDetonationMode = (Sub_WeaponDetonationModeE) WeaponDiscernEnum(file);
	return;	
	}

	if(field == "Effects")
	{
	WeaponDiscernFlags(file);
	}

return;
}
/******************************************************************************
Name
Function
******************************************************************************/
bool Weapon::WeaponFlagIsSet(string flag)
{
		//Returns 0 if the flag is not set
return ( (FlagMap[flag]) & (WeaponEffectFlags) );
}

/******************************************************************************
Name
Function
******************************************************************************/
int  Weapon::WeaponGetEffectFlags()
{
return WeaponEffectFlags;
}

/******************************************************************************
Name
Function
******************************************************************************/
void Weapon::WeaponDiscernFlags(ifstream& file)
{
	string buffer;
	while( '\n'  != file.peek() )
	{
	file>>buffer;
	WeaponEffectFlags |= FlagMap[buffer.c_str()];//Flip bit that corresponds to 
	}											 //the flag, if already on, stays on
}

/******************************************************************************
Name
Function
******************************************************************************/
int Weapon::WeaponDiscernEnum(ifstream& file)
{
	string buffer;
	file>>buffer;
	return ( (int)EnumerationMap[buffer.c_str()]);
}

/******************************************************************************
Name
Function
******************************************************************************/
bool Weapon::WeaponPlayFiringSound()
{	
	PlaySound(WeaponFiringSound.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	return false;
}

/******************************************************************************
Name
Function
******************************************************************************/
bool Weapon::WeaponPlayDetonationSound()
{
	PlaySound(WeaponDetonationSound.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	return false;
}



/******************************************************************************
Name 
Function

******************************************************************************/
void Weapon::WeaponSetMaps()
{
	//Put in Missile Init
	//Set Map for Effect Flag
	FlagMap["IgnoreOwner"] = IGNORE_OWNER;
	FlagMap["DetonateAtTargetX"] = DETONATE_X;
	FlagMap["DetonateAtTargetY"] = DETONATE_Y;
	FlagMap["Shocks"] = SHOCKS;
	FlagMap["IgnoreWind"] = IGNORE_WIND;
	FlagMap["Armor-Piercing"] = ARMOR_PIERCE;
	FlagMap["Shield-Piercing"] = SHIELD_PIERCE;
	FlagMap["Shield-Disabling"] = SHIELD_DISABLE;
	FlagMap["IsSub-Weapon"] = IS_SUB_WEAPON;
	FlagMap["Drunk"] = DRUNK;
	FlagMap["SmallThetaError"] = SMALL_THETA_ERR;
	FlagMap["LargeThetaError"] = LARGE_THETA_ERR;
	FlagMap["DefiesGravity"] = DEFY_GRAVITY;
	FlagMap["DoubleGravity"] = DOUBLE_GRAVITY;
	FlagMap["Decays"] = DECAY;
	FlagMap["ArtilleryTarget"] = ART_TARGET;
	FlagMap["Vectored"] = VECTORED;
	FlagMap["Contrail"] = CONTRAIL;
	FlagMap["Napalm"] = NAPALM;

	//Set Map for Enumerations
	EnumerationMap["Normal"] = (int)Normal;
	EnumerationMap["Random_Condition"] = (int)Random_Condition;
	EnumerationMap["Wide_Bombing"] = (int)Wide_Bombing;
	EnumerationMap["Tight_Bombing"] = (int)Tight_Bombing;
	EnumerationMap["At_Same_X"] = (int)At_Same_X;
	EnumerationMap["At_Same_Y"] = (int)At_Same_Y;
	EnumerationMap["Apex"] = (int)Apex;
	EnumerationMap["Acquisition"] = (int)At_Acquisition;
	EnumerationMap["Floor"] = (int)Floor;
	EnumerationMap["Ceiling"] = (int)Ceiling;	
	EnumerationMap["Upward"] = (int)Upward;
	EnumerationMap["Downward"] = (int)Downward;
	EnumerationMap["Forward"] = (int)Forward;
	EnumerationMap["Backward"] = (int)Backward;
	EnumerationMap["Parallel"] = (int)Parallel;
	EnumerationMap["Random_Mode"] = (int)Random_Mode;
	EnumerationMap["Fireworks"] = (int)Fireworks;
	EnumerationMap["Gatling"] = (int)Gatling;
	EnumerationMap["Original"] = (int)Original;
	EnumerationMap["Trace"] = (int)Trace;//echan
	EnumerationMap["Chain"] = (int)Chain;//echan
	EnumerationMap["Recurring"] = (int)Recurring;//echan
	EnumerationMap["Detonation"] = (int)Detonation;
	EnumerationMap["Shower"] = (int)Shower;

	WeaponNameMap["Standard_Cannon"] = StandardC;
	WeaponNameMap["Super_Cannon"] = SuperC;
	WeaponNameMap["Uber_Cannon"] = UberC;
	WeaponNameMap["Flak_Cannon"] = FlakC;
	WeaponNameMap["Radar_Missile"] = Radar_Missile;
	WeaponNameMap["Thermal_Missile"] = Thermal_Missile;//echan
	WeaponNameMap["Jackhammer"] = Jackhammer;//echan
	//WeaponNameMap["Radar_Missile"] = Radar_Missile;//echan
	WeaponNameMap["Drunk_Missile"] = Drunk_Missile;
	WeaponNameMap["Sniper"] = Sniper;
	WeaponNameMap["Carpet_Bomb"] = Carpet;
	WeaponNameMap["Flamethrower"] = Flamethrower;
	WeaponNameMap["Napalm"] = Napalm;
	WeaponNameMap["Firestorm"] = Firestorm;
	WeaponNameMap["Earthmover"] = Earthmover;
	WeaponNameMap["Dirtball"] = Dirtball;
	WeaponNameMap["Mudslinger"] = Mudslinger;
	WeaponNameMap["Tracer"] = Tracer;
	WeaponNameMap["EMP"] = EMP;
	WeaponNameMap["Artillery"] = Artillery;
	WeaponNameMap["ICBP"] = ICBP;
	WeaponNameMap["Tracer"] = Tracer;
	WeaponNameMap["Chain_Gun"]= Chain_Gun;
	//WeaponNameMap["None"] = None;
}
/******************************************************************************
Name 
Function

******************************************************************************/
void Weapon::WeaponSetSubOrigin(POINT origin, int count)
{
this->WeaponMissile.origin[count].x = origin.x;
this->WeaponMissile.origin[count].x = origin.y;
}

/******************************************************************************
Name 
Function

******************************************************************************/
bool Weapon::WeaponDetConIsMet(float polyangle, int count)//unimplemented, checks to see whether or not to detonate
{
	bool retval = false;
	switch (this->WeaponDetonationType)
	{

	case Random_Condition:
		{
			int i = rand();
			retval = (i%RANDOM_DETONATION_CONSTANT)?true:false;
			break;
		} 

	case At_Same_X:
		{
		
			break;
		} 
	case At_Same_Y:
		{	//Get info on targets

			break;
		} 
	case Apex:
		{

			break;
		} 
	case At_Acquisition:
		{

			break;
		} 
	case Floor:
		{
			retval = ( (WeaponMissile.origin[count].y + WeaponMissile.changey[count]) <= 0)?true:false;
			break;
		} 
	case Ceiling:
		{
			retval = ( (WeaponMissile.origin[count].y + WeaponMissile.changey[count]) >= gresolutionY)?true:false;
			break;
		}
	default:
		return false;
	}
return true;
}
/***************************************************
***************************************************/
bool Weapon::WeaponShouldDeploySub(int count, float polyangle, POINT position)
{

	bool retval = false;
	static short i = 1;
	static int delay = 10;//echan
	
	//Hack?
	
	Sub_WeaponDetonationConditionE check = (!count)?m[WeaponName].WeaponSub_WeaponDetonationCondition:m[WeaponSub_Weapon].WeaponSub_WeaponDetonationCondition;
	
	switch(check)
	{
		case Random_Condition:
		{
			int i = rand();
			retval = (rand()%RANDOM_DETONATION_CONSTANT)?false:true;
			break;
		}

	case At_Same_X:
		{
			//
			tankIndex = iTankCount;
			
			while( !retval && (--tankIndex > -1) )
			{
				retval = (abs(position.x - iTank[tankIndex].location.x) < (10 +rand()%3) )? true:false;
			}
			
			(WeaponFlagIsSet("IgnoreOwner")  && (tankIndex == iTurn) )?(retval = false): 0;
			break;
		} 
	case At_Same_Y:
		{			
			while( ( --tankIndex > -1) && !retval)
			{
				retval = (abs(position.y - iTank[tankIndex].location.y) < (5 + rand()%3) )? true:false;
			}

			break;
		}
	case Wide_Bombing:
		{
			retval = ( (abs((position.x - (WeaponMissile.origin[count].x ))) / (110*NextDrop) ) >= 1 )? NextDrop++, true:false;
			break;
		}

	case Tight_Bombing:
		{
			retval = ( (abs((position.x - (WeaponMissile.origin[count].x ))) / (50*NextDrop) ) >= 1 )? NextDrop++, true:false;
			break;
		}
	case Apex:
		{
			retval =  ( (cos(polyangle) - .05f ) <= 0);
			break;
		} 
	case At_Acquisition:
		{
			/*
			tankIndex = iTankCount;
			int x = position.x;
			int y  = position.y;
			float distance, closest = 700;
			int closestTank = -1;
			while( !retval && ( --tankIndex > -1) )
			{
			distance = pow( pow( (position.x - iTank[tankIndex].location.x), 2) + pow( (position.y - iTank[tankIndex].location.y), 2), .5);
			(distance < closest)? closestTank = tankIndex : 0;
			}
			POINT newOrigin = position;
			int i = position.x - iTank[tankIndex].location.x;
			int j = position.y - iTank[tankIndex].location.y;

			for(; !retval && (j>0) && (i>0); i--, j-- )
			{
			retval = (TERRAIN == GetPixel(ghdcMem, position.x -= (position.x/i), position.y += (position.y/j)));
			}
			*/
			
			break;
		} 
	case Floor:
		{
			retval = ( (position.y) >= gresolutionY )?true:false;
			break;
		} 
	//echanVV
	case Original:
		{
			retval = (count)?false:true;
			break;
		}
	//echanVV
	case Recurring:
		{

			retval = (!count && ( !(delay--)))?true:false;
			(delay < 0)?delay = 10:0;
			break;
		}
	//echan^^

		
	//echan^^
	case Ceiling:
		{
			retval = ( (position.y) <= 0)?true:false;
			break;
		}
	default:
		retval = false;
		if(WeaponSub_Weapon != WeaponName)//echan
		{
		m[WeaponSub_Weapon].WeaponMissile.draw[0] = false; //echan
	}
	}
	

	return retval;
	//Spawns from spawned, should check for is subweapon or something, add a None case
}
/***************************************************
***************************************************/

/***************************************************
***************************************************/
bool Weapon::WeaponDeploySub(POINT origin, float polyangle, float angle, int numFrame)//Including angle, etc
{
	

short i = RemainingBombs;
short LoadCount = SimulSubCount;

//No need for Remaining and what not, bombs will be spawned whenever.  Just something for 
//number of bombs to be dropped at a time.
missile* SubWeapon = &(m[this->WeaponSub_Weapon].WeaponMissile);

//SubWeapon->draw[0] = ( (WeaponSub_Weapon == Tracer) || (WeaponSub_Weapon == Chain_Gun) )?true:false; //echan 
SubWeapon->draw[0] =  ( (WeaponSub_Weapon == WeaponName) && (WeaponSub_WeaponDetonationCondition != Detonation) )?true:false; //echan 


//i is now at first index of undrawn missile try one for now
for(;LoadCount >= 1 ; i--, RemainingBombs--, LoadCount--)
{
SubWeapon->draw[i]  = true;
SubWeapon->origin[i].x = origin.x;
SubWeapon->origin[i].y = origin.y;
SubWeapon->changex[i]= SubWeapon->changey[i] = SubWeapon->prevChangex[i] = SubWeapon->prevChangey[i] = 0;
SubWeapon->frameAdj[i] = numFrame;

//Should use pointlist from sub weapon Toi
	switch(this->WeaponSub_WeaponDetonationMode)
	{
		//make original missile have origin of tank place
		//adjust angle with angle difference anglediff
		case Upward:
			{	
				SubWeapon->anglediff[i] = ( (sin(angle) > 0 )? pi/2.0f - angle: pi/1.5f - angle);
				continue;//
				break;
			} 
		case Downward:
			{
				SubWeapon->anglediff[i] = ( (sin(angle) > 0 )? pi/2.0f - angle: pi/1.5f - angle) + pi;
				break;
			} 
		case Forward:
			{
				SubWeapon->anglediff[i] = ( (cos(angle) > 0 )? -1*angle: pi - angle);
				break;
			} 
		case Backward:
			{
				SubWeapon->anglediff[i] = ( (cos(angle) > 0 )? -1*angle: pi - angle) + pi;
				break;
			} 
		case Parallel:
			{
				SubWeapon->anglediff[i] = ( polyangle - angle);
				break;
			} 
		case Random_Mode:
			{
				SubWeapon->anglediff[i] = ( rand()*.5  );
				break;
			} 
		case Fireworks:
			{
				SubWeapon->anglediff[i] = ( ( (rand()%255)>128)?1:-1)*( rand()* .5 );
				break;
			} 
			//echan v 
		case Gatling:
			{
				SubWeapon->anglediff[i] =(  ( (rand()%255)<128) ?1:-1)*(rand()%15 * ratio);
				break;
			}
		case Trace:
			{
				//echan
				SubWeapon->anglediff[1] = (10)*ratio;//echan
				SubWeapon->anglediff[2] = (5)*ratio;//echan
				SubWeapon->anglediff[3] = (-5)*ratio;//echan
				SubWeapon->anglediff[4] = (-10)*ratio;//echan
				//echan horrible hack
				continue;
				break;
			
			}
		case Chain://changed weapon file, got rid of flamethrower, now chain
			{
				SubWeapon->anglediff[i] = 0;
				SubWeapon->origin[i] = WeaponMissile.origin[0];
				break;
			}

		case Shower:
			{
				SubWeapon->anglediff[i] = ( (sin(angle) > 0 )? pi/2.0f - angle: pi/1.5f - angle);
				break;
			}
		/*case Detonation:
			{
				continue;
				break;
				//break;
			}*/
			//echan^
	}
	SubWeapon->anglediff[i] += (( (rand()%255)> 127 )?-1:1)*static_cast<float>(rand()%10)*.02f;
	
}


(RemainingBombs <= 0)?EmptyHull = true:0;//echan

return false;
}
/***************************************************
***************************************************/
bool Weapon::WeaponGetPoints(ifstream& file)
{

	file>>WeaponMissile.numObjects;
	file.ignore(20, '\n');
	file>>WeaponMissile.scale;
	file.ignore(20, '\n');

	int numberOfPoints = 0;
	for(int i = 0; i < WeaponMissile.numObjects ; i++)
	{	
		file>>WeaponMissile.objIdentifier[i];	
		numberOfPoints += (WeaponMissile.objIdentifier[i] + 3); //3 is difference between #defined alias 
		file.ignore(20, '\n');									//and actual number of points
	}
	int j = -1, m = WeaponMissile.numObjects;

	while(m--)
	{
		file>>WeaponMissile.objColor[m];
		file.ignore(20, '\n');
	}

	totalPtCount = numberOfPoints;
	while(++j < numberOfPoints)//Maybe optimize later
	{
		file>>WeaponMissile.ptarray[j].x;
		
		WeaponMissile.ptarray[j].x *= WeaponMissile.scale;

		file.ignore(1);
		
		file>>WeaponMissile.ptarray[j].y;

		WeaponMissile.ptarray[j].y *= WeaponMissile.scale;

		file.ignore(20, '\n');
	}
return false;
}

/***************************************************
***************************************************/
bool Weapon::WeaponDetonate(int count)
{
//Check Detonation types
//DisplayBlast(this->WeaponMissile.origin[count].x + this->WeaponMissile.changex[count], this->WeaponMissile.origin[count].y + this->WeaponMissile.changey[count], 20, BLUE);


if(this->WeaponFlagIsSet("DefiesGravity"))
{
	POINT position = {(this->WeaponMissile.origin[count].x + this->WeaponMissile.changex[count]),(this->WeaponMissile.origin[count].y + this->WeaponMissile.changey[count])};
	if( position.x	< -10.0f || position.x > SCREEN_WIDTH + 10 )//the tens are for "clipping"
	{
	this->WeaponMissile.draw[count] = false;
	}

	if( position.y	< -10.0f || position.y > SCREEN_HEIGHT + 10 )//the tens are for "clipping"
	{
	this->WeaponMissile.draw[count] = false;
	}
}

//
if(WeaponName == Artillery)
{
  DisplayLine(SCREEN_WIDTH/2, 0, WeaponMissile.origin[count].x + WeaponMissile.changex[count], WeaponMissile.origin[count].y + WeaponMissile.changey[count], rand() ); 
}

return true;
}

/***************************************************
***************************************************/
void Weapon::ReInitWeapon()
{
		for(int i = 1; i < WeaponMissile.numofmissile; i++)
			{
				WeaponMissile.draw[i] = false;
				WeaponMissile.frameAdj[i] = 0;
				WeaponMissile.changex[i] = WeaponMissile.changey[i] = WeaponMissile.prevChangex[i] = WeaponMissile.prevChangey[i] = 0;
			}
			
		missileInit(&WeaponMissile, 5);	
		WeaponMissile.draw[0] = (!WeaponFlagIsSet("IsSub-Weapon") )? true: false;
		m[WeaponSub_Weapon].WeaponMissile.draw[0] = true; //echan
		
			EmptyHull = false;
			RemainingBombs = SubCount;
			NextDrop = 1;
}
