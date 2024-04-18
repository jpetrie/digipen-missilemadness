// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================

// -----------------------------------------------------------------------------
//                                                                         guard
// -----------------------------------------------------------------------------
#pragma warning (disable:4786)

#ifndef _TANK_MOD_ITEMS_H_
#define _TANK_MOD_ITEMS_H_


// -----------------------------------------------------------------------------
//                                                                      includes
// -----------------------------------------------------------------------------

// tank
#include "tank.h"

#include "mod_tank.h"

// -----------------------------------------------------------------------------
//                                                                     constants
// -----------------------------------------------------------------------------


// effect flags
const int kItemEffect_Armor             = 1;
const int kItemEffect_Shield            = 2;
const int kItemEffect_RJam              = 4;
const int kItemEffect_TJam              = 8;
const int kItemEffect_Phase             = 16;
const int kItemEffect_NinjaStealthTrick = 32;

struct StItem
{
	string name;
	int		duration; // In turn cycles.
	int		cost;			// Cost to purchase.
	int		qty;			// Uses obtained per purchase.
	int		power;		// Power of effect; byte order (high to low): STEALTH,JAMMING,SHIELD,ARMOR
	int		flags;		// Effect flags.
};


/*
struct StItemFlags
{
	int s  : 1;
	int a  : 1;
	int rj : 1;
	int tj : 1;
	int c  : 1;
	int p  : 1;
	int d  : 3;
}
*/

void ItemRead(vector<StItem> &itemslist);


void ItemApplyEffects(tank *target,StItem *item);
void ItemApplyDurations(tank *target);


#endif