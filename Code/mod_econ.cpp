
#include "tank.h"

#include "mod_game.h"
#include "mod_econ.h"

#include "mod_weapons.h"
#include "mod_weapon_attributes.h"
#include "debugging.h"

extern Weapon m[20];

int gInitialCash = kInitialCash;

int gInitialWeap[MAX_WEAPONS];
int gInitialItem[MAX_ITEMS];
int gItemCtr = 0;
int gWeapCtr = 0;


void PurchaseItem(int idx)
{
	if(gInitialCash - iItemList[idx].cost >= 0 && gItemCtr < MAX_ITEMS)
	{
		gInitialCash -= iItemList[idx].cost;
		gInitialItem[gItemCtr++] = idx;
	}
}


void SellItem(int idx)
{
	if(gItemCtr > 0)
	{
		gInitialCash += iItemList[gInitialItem[idx]].cost;

		// Copy items down.
		for(int i = idx; i < MAX_ITEMS; ++i)
		{
			gInitialItem[i] = (i == MAX_ITEMS ? -1 : gInitialItem[i + 1]);
		}

		gItemCtr--;
	}
}


void PurchaseWeapon(int idx)
{
	if(gInitialCash - m[idx].WeaponCost >= 0 && gWeapCtr < MAX_WEAPONS)
	{
		gInitialCash -= m[idx].WeaponCost;
		gInitialWeap[gWeapCtr++] = idx;
	}
}


void SellWeapon(int idx)
{
	if(idx == 0) return;
	
	if(gWeapCtr > 0)
	{
	//	gInitialCash += m[idx].WeaponCost;
		gInitialCash += m[gInitialWeap[idx]].WeaponCost;

		// Copy items down.
		for(int i = idx; i < MAX_WEAPONS; ++i)
		{
			gInitialWeap[i] = gInitialWeap[i + 1];
		}

		gWeapCtr--;
	}
}


void FireWeapon(tank *t,int idx)
{
	// Weapon in spot 0 is infinite.
	if(idx == 0) return;
	if(idx >= t->weap_ctr) return;

	// Copy items down.
	for(int i = idx; i < t->weap_ctr; ++i)
	{
		//t->weap[i] = (i == t->weap_ctr ? -1 : t->weap[i + 1]);
		t->weap[i] = t->weap[i + 1];
	}

	t->weap_ctr--;
}



void FireItem(tank *t,int idx)
{
	// Copy items down.
	for(int i = idx; i < t->item_ctr; ++i)
	{
		t->item[i] = (i == t->item_ctr ? -1 : t->item[i + 1]);
	}

	t->item_ctr--;
}