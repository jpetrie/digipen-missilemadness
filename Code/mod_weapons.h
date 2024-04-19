#pragma warning(disable: 4786)
#include <windows.h>
#include "mod_weapon_attributes.h"


#ifndef MOD_WEAPONS_H
#define MOD_WEAPONS_H

#define polygon3  0
#define polygon4  1
#define polygon5  2
#define polygon6  3

extern const double pi;
extern const double ratio;

void calcMissile(Weapon *m,int count,double force,double angle,int numFrame, HDC hdcMem);
void drawMissile(Weapon *w,POINT location,double polyangle,double xchange, double ychange,HDC hdcMem, int count);
bool AfterEffects(Weapon& weapon, int count);


#endif
