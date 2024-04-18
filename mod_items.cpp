

#include "mod_terrain.h"
#include "mod_items.h"
#include "debugging.h"
#include "utilities.h"
#include "mod_game.h"

extern Map iSurfaceMap[SCREEN_WIDTH + 1];
extern int iCoreSeed;

void ItemApplyEffects(tank *target,StItem *item)
// Applies the effects of `item` to the tank specified by `target`.
{
int  TheShieldDecay;

	dassert(target != NULL);
	dassert(item != NULL);

	// Unpack durations.
	TheShieldDecay = (0xFF & target->durations);

	// Handle armor bonus effect.
	if(item->flags & kItemEffect_Armor)
	{
		target->armor += (item->power & 0xFF);
	}

	// Handle shield bonus effect.
	if(item->flags & kItemEffect_Shield)
	{
		target->shield += (10 * (item->power & 0xFF00) >> 8);
		TheShieldDecay = (item->duration & 0xFF) * iTankCount;
	}

	// Handle phasing effect.
	if(item->flags & kItemEffect_Phase)
	{
		target->location.x = rand() % SCREEN_WIDTH;
		target->location.y = iSurfaceMap[target->location.x].yTop - 20;
	}

	// Handle cloak. Cloak is nonissue.
	if(item->flags & kItemEffect_NinjaStealthTrick)
	{
		if(target->decoy)
		{
			delete target->decoy;
		}

		target->cloaked = true;
		
		// Create decoy tank.
		target->decoy = new tank;
		initTank(target->decoy);
		target->decoy->color1 = target->color1;
		target->decoy->color2 = RED;
		target->decoy->location.x = rand() % SCREEN_WIDTH;
	//	dcon << "Seed is: " << iCoreSeed << "\n";
	//	dcon << "The decoy is at X: " << target->decoy->location.x << "\n";

		target->decoy->location.y = iSurfaceMap[target->decoy->location.x].yTop - 20;
		target->decoy->armor = (item->power & 0xFF000000) >> 24;
		
		// Move tank to decoy location.
		target->location.x = rand() % SCREEN_WIDTH;
		//dcon << "The real tank is at X: " << target->location.x << "\n";

		target->location.y = iSurfaceMap[target->location.x].yTop - 20;
	}

	
	// Repack durations.
	target->durations = TheShieldDecay;
}


void ItemApplyDurations(tank *target)
// Call once PER TURN to handle item duration decays.f
{
int  TheShieldDecay;

	// Dumbass proofing.
	dassert(target != NULL);

	TheShieldDecay = target->durations;

	// Check shield decay.
	--TheShieldDecay;
	if(TheShieldDecay <= 0)
	{
		target->shield = 0;
	}

	// Repack durations.
	target->durations = (TheShieldDecay);
}


void ItemRead(vector<StItem> &itemslist)
// reads item data from Game Data/items/
// each file is parsed as an item and interpreted. malformed files are aborted with no errors
{
HANDLE           TheFindHand;
WIN32_FIND_DATA  TheFindData;
string TheBuffer;
string TheFN;
vector<string> TheTokens;
StItem TheItem;
char fbuf[256];

	TheFindHand = FindFirstFile("GameData/items/*",&TheFindData);
	if(TheFindHand != INVALID_HANDLE_VALUE)
	{
		do
		// For each file.
		{
		TheFN = "GameData/items/";
		TheFN += TheFindData.cFileName;
		FILE *fp = fopen(TheFN.c_str(),"r");

			TheBuffer = "";
			while(fp && !feof(fp))
			{

				fgets(fbuf,256,fp);
				TheBuffer += fbuf;
			}

			sprintf(fbuf,"");

			if(fp)
			{
				// defaults
				TheItem.flags = 0;
				TheItem.cost = 0;
				TheItem.duration = 0;
				TheItem.name = "INVALID";
				TheItem.power = 0;
				TheItem.qty = 0;

				// now find the item stuff.
				TheTokens = strexplode(TheBuffer,"\n");
				for(int i = 0; i < TheTokens.size(); ++i)
				{
				vector<string> Parts = strexplode(TheTokens[i],":");

					if(Parts.size() >= 2)
					{
						// For each token....
						if(Parts[0].find("name") != string::npos)
							TheItem.name = Parts[1];
						else if(Parts[0].find("cost") != string::npos)
							TheItem.cost = atoi(Parts[1].c_str());
						else if(Parts[0].find("qty") != string::npos)
							TheItem.qty = atoi(Parts[1].c_str());
						else if(Parts[0].find("armor") != string::npos)
						{
							TheItem.flags |= kItemEffect_Armor;
							TheItem.power &= 0xFFFFFF00;
							TheItem.power |= (atoi(Parts[1].c_str()));
						}
						else if(Parts[0].find("shield") != string::npos)
						{
							TheItem.flags |= kItemEffect_Shield;
							TheItem.power &= 0xFFFF00FF;
							TheItem.power |= (atoi(Parts[1].c_str()) << 8);

							TheItem.duration &= 0xFFFFFF00;
							TheItem.duration |= (atoi(Parts[2].c_str()));
						}
						else if(Parts[0].find("rjam") != string::npos)
						{
							TheItem.flags |= kItemEffect_RJam;
							TheItem.power &= 0xFF00FFFF;
							TheItem.power |= (atoi(Parts[1].c_str()) << 16);

							TheItem.duration &= 0xFF00FFFF;
							TheItem.duration |= (atoi(Parts[2].c_str()) << 24);
						}
						else if(Parts[0].find("tjam") != string::npos)
						{
							TheItem.flags |= kItemEffect_TJam;
							TheItem.power &= 0xFF00FFFF;
							TheItem.power |= (atoi(Parts[1].c_str()) << 16);

							TheItem.duration &= 0xFFFF00FF;
							TheItem.duration |= (atoi(Parts[2].c_str()) << 16);
						}
						else if(Parts[0].find("ninjitsu") != string::npos)
						{
							if(Parts[1].find("stealth-dash") != string::npos)
								TheItem.flags |= kItemEffect_Phase;
							else if(Parts[1].find("tricky-fuckin-ninja") != string::npos)
								TheItem.flags |= kItemEffect_NinjaStealthTrick;
						}
					}
				}

				if(TheItem.name != "INVALID")
					itemslist.push_back(TheItem);

			}
			
			if(fp)
				fclose(fp);
			
		}while(FindNextFile(TheFindHand,&TheFindData));
	}

	FindClose(TheFindHand);
}
