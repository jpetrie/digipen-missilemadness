//***********************************************************************//
//																		 //
//	Eric Roberts														 //
//  Weapon effects header												 //
//	1/14/03																 //
//																		 //
//***********************************************************************//
#pragma warning(disable:4786)
#pragma warning(disable:4800)
#ifndef WEAPON_ATT_
#define WEAPON_ATT_
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <windows.h>
#include "mod_missiles.h"

#define NUMWEAPONS 22


using namespace std;
typedef long Flags;

	/*map<string, int> FlagMap;
	map<string, int> EnumerationMap;
	map<string, char> WeaponNameMap;*/

const int RANDOM_DETONATION_CONSTANT = 7;

enum Sub_WeaponE{};
enum Sub_WeaponDetonationConditionE{None, Normal, Random_Condition, Wide_Bombing, Tight_Bombing, At_Same_X, At_Same_Y, Apex, At_Acquisition, Floor, Ceiling, Original, Recurring, Detonation};//echan
enum Sub_WeaponDetonationModeE{Upward, Downward, Forward, Backward, Parallel, Random_Mode, Fireworks, Gatling, Trace, Chain, Shower};//echan
//enum WeaponNameE {StandardC, SuperC, UberC, FlakC, Radar_Missile, Thermal_Missile, Drunk_Missile, Sniper, Carpet, Flamethrower, Napalm, Firestorm, Earthmover, Dirtball, Mudslinger, Tracer, EMP, Artillery, ICBP};
//Weapon Names
const char StandardC		=1;
const char SuperC			=2;
const char UberC			=3;
const char FlakC			=4;
const char Radar_Missile	=9;
const char Thermal_Missile	=20;//echan
const char Jackhammer		=6; //echan
const char Drunk_Missile	=7;
const char Sniper			=19;
const char Carpet			=8;
const char Flamethrower		=10;//echan
const char Chain_Gun		=5; //echan
const char Napalm			=21;
const char Firestorm		=11;//echan
const char Earthmover		=12;
const char Dirtball			=13;
const char Mudslinger		=14;
const char Tracer			=15;
const char EMP				=16;	
const char Artillery		=17;
const char ICBP				=18;
//const char None				=0;

// Terrain Effect constants
const signed char Subtractive	= 0;
const signed char Additive		= 1;
const signed char NoEffect		= 2;

union EffectData //unimp
{
int ble;
float blw;
};

//Int
const int NO = 0;
const int YES = 1;

const int ItemNameLength =     80; 
//Weapon Effect Flags
const int IGNORE_OWNER   =      1;
const int DETONATE_X     =   1<<1;//Unimp
const int DETONATE_Y     =   1<<2;//Unimp
const int SHOCKS         =   1<<3;//Unimp
const int IGNORE_WIND    =   1<<4;//Unimp no wind yet
const int ARMOR_PIERCE   =   1<<5;//unimp
const int SHIELD_PIERCE  =   1<<6;//unimp
const int SHIELD_DISABLE =   1<<7;//unimp
const int IS_SUB_WEAPON  =   1<<8;
const int DRUNK          =   1<<9;
const int SMALL_THETA_ERR=  1<<10;
const int LARGE_THETA_ERR=  1<<11; 
const int DEFY_GRAVITY   =  1<<12;
const int DOUBLE_GRAVITY =  1<<13;
const int DECAY          =  1<<14; //unimp
const int ART_TARGET     =  1<<15; 
const int VECTORED       =  1<<16;//unimp
const int CONTRAIL       =  1<<17;//unimp
const int NAPALM		 =  1<<18;//unimp
const int TRACER		 =  1<<19;//echan
//End of Effect flags

/*/////////////////////////////////////////////////////
/////////////////////////////////////////////////////*/
typedef class Weapon
{
public:
	 Weapon();
	 ~Weapon();
	 
	 const Weapon& operator=(const Weapon& rhside); //Probably not needed
		
bool WeaponFlagIsSet(string flag);//overload for long int argument

void Read(string filename, string name);//Needs to be done for each missile at beginning of game
int  WeaponGetEffectFlags();
void WeaponSetEffectFlags(long flags);
void WeaponSetMaps();
bool WeaponPlayFiringSound();
bool WeaponPlayDetonationSound();
void WeaponSetSubOrigin(POINT origin, int count);
bool WeaponDetConIsMet(float polyangle, int count); 
bool WeaponShouldDeploySub(int count, float polyangle, POINT position);
bool WeaponDeploySub(POINT origin, float poylangle, float angle, int numFrame);
bool WeaponDetonate(int count);//Clear stuff in here
bool WeaponGetPoints(ifstream& file);
void ReInitWeapon();
void FindMember(string field, ifstream& file);
void WeaponDiscernFlags(ifstream& file);
//void WeaponGetSubWeapon(string name);

int totalPtCount;

//private:
	
	missile WeaponMissile;
    int WeaponDiscernEnum(ifstream& file);
	int WeaponDamageRadius;
	int WeaponCost;
	short SubCount;
	short SimulSubCount;
	short RemainingBombs; 
	short NextDrop;
	short WeaponDirectDamage;
	short WeaponClipSize;
	float WeaponDamageFalloff;
	signed char WeaponTerrainEffect;
	bool EmptyHull;
	char WeaponName;
	char WeaponSub_Weapon;//index into global array
	long WeaponDetonationType;
	string WeaponNameString;
	
	string WeaponDetonationSound; 
	string WeaponFiringSound;
	

	Sub_WeaponDetonationConditionE WeaponSub_WeaponDetonationCondition;//
	Sub_WeaponDetonationModeE WeaponSub_WeaponDetonationMode;
	Flags WeaponEffectFlags;

	
	map<string, long>FlagMap;
	map<string, int> EnumerationMap;
	map<string, char> WeaponNameMap;
	//Sub_WeaponE WeaponSub_Weapon; //Enum undefined
	//Static members 
	
	//map<string, int> FlagMap;
	//map<string, int> EnumerationMap;
	//map<string, char> WeaponNameMap;
	
	
} *WeaponPtr;



/*	map<string, int> FlagMap;
	map<string, int> EnumerationMap;
	map<string, char> WeaponNameMap;
	
	static map<string, int> FlagMap;
	static map<string, int> EnumerationMap;
	static map<string, char> WeaponNameMap;
*/
static bool isMapSet =false;


/*Weapon::map<string, int> FlagMap;
Weapon::map<string, int> EnumerationMap;
Weapon::map<string, char> WeaponNameMap;*/

#endif

