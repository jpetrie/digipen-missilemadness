#pragma warning (disable:4786)

#ifndef _TANK_MOD_ECON_
#define _TANK_MOD_ECON_

#include "tank.h"

#include "mod_tank.h"

const int kInitialCash = 15000;

extern int gInitialCash;

void PurchaseItem(int idx);
void SellItem(int idx);

void PurchaseWeapon(int idx);
void SellWeapon(int idx);

void FireWeapon(tank *t,int idx);
void FireItem(tank *t,int idx);

#endif