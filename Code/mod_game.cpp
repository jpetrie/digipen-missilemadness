
#include "tank.h"

#include "mod_game.h"
#include "mod_terrain.h"
#include "mod_display.h"
#include "mod_weapons.h"
#include "mod_missiles.h"
#include "mod_network.h"
#include "mod_ui.h"
#include "mod_tank.h"
#include "mod_items.h"
#include "windows.h"
#include "mod_weapon_attributes.h"
#include "debugging.h"
#include "mod_ai.h"
#include "handler_ui.h"

#include "mod_sound.h"
#include "mod_econ.h"
arrow arrow1;
#define NUMWEAPONS  22
int  iCoreSeed;
extern string bgm_Battle;
Map iSurfaceMap[SCREEN_WIDTH + 1];

bool iInPostMortem = false;

bool allowNextTurn = true;
bool sentReady;
bool showWaitingForSlowpokes = false;

static string        iName = "tankplayer";  // Player name default.
tank			*iTank;
int           iTankCount;
 int           iTurn;
 int           iTurnOrderID;

vector<int> gDeadList; // First death is first element;
bool goToPostMortemASAP = false;
bool iTankUsedItem = false;
int gShowHoverID = -1;
bool first = false;
bool started = false;
/////////////////////////////////////////////////////////////////////////////////
//MISSILE DATA																   
//missile launch variables                                                     
bool launch = false; //whether or not we are executing launch code             
int  animaCount = 0; //use to delay animation                                    
int  animation = 1; //amount of game loops to wait before moving missile
int  numFrame = 0;  //keeps track of current frame in missile animation
POINT locationM = {500,500};//original position of missile before flight

//user missile input variables
int forceInput = 50; //amount of force that player thinks he is using
double force = 0; //amount of force the player is actually using
double angle = 0;//current angle

//missile object variables
Weapon m[NUMWEAPONS]; //an array of missile types in user's arsenal //echan
int currentM = Artillery; //current missile that user has selected

// items
vector<StItem> iItemList;

//END OF MISSILE DATA
double compForce  = 0; //force of computer shot
bool singlePlayer = false; //are we playing single player?
bool thinking = false;
int thinkTime = 100;

bool mustSendReady = false;

extern int gItemCtr;
extern int gWeapCtr;

extern int gInitialItem[MAX_ITEMS];
extern int gInitialWeap[MAX_WEAPONS];


void GameStart(char seed,vector<string> names,char id)
{

	/* BACKGROUND MUSIC PER LEVEL */
		extern BG_Music bgm;
		bgm.Stop();
		bgm.Play(bgm_Battle.c_str(), true);

	// Store the main seed globally; we must have
	// a synchonized random sequence between all
	// the clients or Very Bad Things will happen.
	iCoreSeed = seed;
	srand(iCoreSeed);

	// Turn order zero goes first.
	iTurn = 0;

	// Store where in the turn order we are allowed to go.
	iTurnOrderID = id;
	dcon << "I am going on turn: " << iTurnOrderID << "\n";

	// Create the tank array.
	if(!singlePlayer)
	{
		iTankCount = names.size();	
	}
	else
	{
		iTankCount = 2;
	}
	
	iTank = new tank[iTankCount];

	for(int i = 0; i < iTankCount - ((int)singlePlayer); ++i)
	{
		iTank[i].name = names[i];

bool	good_loc = true;
	do
	{
		iTank[i].location.x    = rand() % SCREEN_WIDTH;
		for(int tt = 0; tt < i; ++tt)
		{
			if(iTank[i].location.x <= iTank[tt].location.x - 100 &&
			   iTank[i].location.x >= iTank[tt].location.x + 100)
			{
				good_loc = false;			}
		}
	}while(!good_loc);

		iTank[i].location.y    = iSurfaceMap[iTank[i].location.x].yTop;
		initTank(&iTank[i]);//initialize tank structure
		
		// Set my cash and gear.
		if(i == iTurnOrderID)
		{
			iTank[i].cash = gInitialCash;

			dcon << "Items: ";
			for(int ii = 0; ii < gItemCtr; ++ii)
			{
				iTank[i].item[ii] = gInitialItem[ii];
				dcon << iTank[i].item[ii] << " ";
			}
			dcon << "\n";

			iTank[i].item_ctr = gItemCtr;
			dcon << "Weapons: ";
			for(int iw = 0; iw < gWeapCtr; ++iw)
			{
				iTank[i].weap[iw] = gInitialWeap[iw];
				dcon << iTank[i].weap[iw] << " ";
			}
			iTank[i].weap_ctr = gWeapCtr;
			dcon << "\n";
		}

		if(i == 0)
		{
			iTank[i].color1 = BLUE;
		}
		if(i == 1)
		{
			iTank[i].color1 = RED;
		}
		if(i == 2)
		{
			iTank[i].color1 = YELLOW;
		}
		if(i == 3)
		{
			iTank[i].color1 = MAGENTA;
		}
		if(i == 4)
		{
			iTank[i].color1 = CYAN;
		}
		if(i == 5)
		{
			iTank[i].color1 = GREEN;
		}
		if(i == 6)
		{
			iTank[i].color1 = LTBLUE;
		}
		if(i == 7)
		{
			iTank[i].color1 = TOOLBAR;
		}

		DisplayText(iTank[i].location.x,iTank[i].location.y - 20,const_cast<char *>(iTank[i].name.c_str()),RGB(0,0,0));

		if(singlePlayer)
		{
			iTank[1].name = "Computer";

			do
			{
				iTank[1].location.x    = rand() % SCREEN_WIDTH;
			}while(iTank[1].location.x >= iTank[0].location.x - 300 &&
				   iTank[1].location.x <= iTank[0].location.x + 300);

			iTank[1].location.y    = iSurfaceMap[iTank[1].location.x].yTop;
			initTank(&iTank[1]);//initialize tank structure
			
			iTank[1].color1 = RED;
			iTank[1].aiWeaponCount = 0;

			iTank[1].armor = 400;
			iTank[1].shield  = 300;
			
			for(int i = 0; i < MAX_WEAPONS;i++)
			{
				while(!(iTank[1].weap[i] = rand() % (NUMWEAPONS - 2)));
				
				//iTank[1].weap[i] = 1;
			}
			UIHandleChat("**** \'Computer\' has joined.");
		}
	}

}


string GameGetPlayerName(void)
{
	return (iName);
}


void GameSetPlayerName(string name)
{
	iName = name;
}





void GameProcessTurn(float pwr,int ang,int wep,int seed)
{
	if(pwr == kNet_TurnDead)
	{
		// Skip this tank's turn.
		iTank[iTurn].dead = true;
	//	if(!gDeadList.find(iTurn))
			gDeadList.push_back(iTurn);
		NetClientUpdate(static_cast<char>(iTurn),static_cast<char>(kNet_TurnKill));// angle of 255 kills the player.
		GameGotoNextTurn();
	}
	else if(ang == kNet_TurnItem)
	{
	StItem testIt;

		// Use the item.
		if(!iTankUsedItem)
		{
		dcon << "Tank at turn " << iTurn << " just used item " << wep << "\n";
		
	/*	testIt.flags = kItemEffect_Shield | kItemEffect_NinjaStealthTrick;
		testIt.power = 50 << 8;
		testIt.duration = 2;*/
		testIt = iItemList[wep];
		ItemApplyEffects(&iTank[iTurn],&testIt);
		iTankUsedItem = true;
		}
		else
		{
			dcon << "Tank at turn " << iTurn << " already used an item.\n";
		}
	}
	else
	{
		if(!singlePlayer || (iTurn == 0))
		{
			currentM = wep;
			force = (float)(pwr / 5.0f);
			angle = ratio * ang;
			launch = true;
			if(wep > 0 && wep <= 19)
				m[wep].WeaponPlayFiringSound();
			mustSendReady = true;
			allowNextTurn = false;
			dcon << "end of stuff in gpt\n";
		}
		else // cpu turn
		{
			currentM = wep;
			launch = true;
			if(wep > 0 && wep <= 19)
				m[wep].WeaponPlayFiringSound();

		}
	}
}


// -----------------------------------------------------------------------------
//																		gameloop
// -----------------------------------------------------------------------------
int numAnglen=1;
int numPowlen=1;

void GameLoop(void)
{
	if(first)
	{
		first = false;
		DisplayRect(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, BACK_G, BACK_G);
		TerrainGenerate(6,6);
		TerrainTrace(TERRAIN, iSurfaceMap);
		TerrainDraw(iSurfaceMap,TERRAIN);
		started = true;

		for(int ti2 = 0; ti2 < iTankCount; ++ti2)
		{
			iTank[ti2].location.y = iSurfaceMap[iTank[ti2].location.x].yTop - 10;
		}
	}

	HBRUSH      TheBackBrush;
	TheBackBrush = CreateSolidBrush(BACK_G);
	BitBlt(ghdcMem,0,0,gresolutionX,gresolutionY,NULL,0,0,BLACKNESS);
	SelectObject(ghdcMem,TheBackBrush);
	Rectangle(ghdcMem,0,0,gresolutionX,gresolutionY);
	DeleteObject(TheBackBrush);

	if(thinking)//is the computer pretending to think
	{
		if(thinkTime)
		{
			thinkTime--;
		}
		else
		{
			thinkTime = 100;
			thinking = false;
			computerAI();
		}
	}

	
	if(!iInPostMortem)
	{
		if(started)
		{	
			TerrainDraw(iSurfaceMap,TERRAIN);
			int index = 0;
			if(vBlast.size() > 0)
			{
				BlastIt = vBlast.begin();
				for(index = 0; index < vBlast.size(); ++index, ++BlastIt)
				{
					if(!TerrainFall(iSurfaceMap, vBlast[index].x_pos, vBlast[index].radius, index))
					{
						vBlast.erase(BlastIt);
						break;
					}
				}
				
			}

			

			TerrainDraw(iSurfaceMap,TERRAIN);

		}



		for(int i = 0; i < iTankCount; ++i)
		{
		char TheNameBuf[256];

			// Only the player sees her shield/armor.
		/*	if(i == iTurnOrderID)
				sprintf(TheNameBuf,"%s (%d %d)",iTank[i].name.c_str(),iTank[i].armor,iTank[i].shield);
			else
				sprintf(TheNameBuf,"%s",iTank[i].name.c_str());*/

			if(gShowHoverID >= 0)
			{
				if(gShowHoverID == iTurnOrderID && !iTank[iTurnOrderID].dead)
				{
					sprintf(TheNameBuf,"%s",iTank[gShowHoverID].name.c_str());
					DisplayText(iTank[gShowHoverID].location.x,iTank[gShowHoverID].location.y - 40,TheNameBuf,YELLOW);

					DisplayText(10,10,"Your Armor: ",YELLOW);
					DisplayText(10,30,"Your Shield: ",YELLOW);
					sprintf(TheNameBuf,"%d",iTank[gShowHoverID].armor);
					DisplayText(100,10,TheNameBuf,YELLOW);
					sprintf(TheNameBuf,"%d",iTank[gShowHoverID].shield);
					DisplayText(100,30,TheNameBuf,YELLOW);
				}
				else if(!iTank[gShowHoverID].dead)
				{
					sprintf(TheNameBuf,"%s",iTank[gShowHoverID].name.c_str());
					DisplayText(iTank[gShowHoverID].location.x,iTank[gShowHoverID].location.y - 40,TheNameBuf,YELLOW);
				}
			}
			/*
			if(iTank[i].cloaked && i != iTurnOrderID)
			{
				// Draw the name of a cloaked tank at the decoy.
			//	DisplayText(iTank[i].decoy->location.x,iTank[i].decoy->location.y - 40,TheNameBuf,YELLOW);
			}
			else
			{
				DisplayText(iTank[i].location.x,iTank[i].location.y - 40,TheNameBuf,YELLOW);
			}*/

			/*Tank Logic/////////////////////////////////////////////////
			///////////////////////////////////////////////////////////*/
			if(iTank[i].cloaked)
			{
				tankPhysics(&iTank[i],TERRAIN,ghdcMem);
				tankPhysics(iTank[i].decoy,TERRAIN,ghdcMem);
			}
			else
			{
				tankPhysics(&iTank[i],TERRAIN,ghdcMem);
			}
			double tangle = iTank[i].angle * ratio;

			HBRUSH      TankBrush;
			TankBrush = CreateSolidBrush(iTank[i].color1);
		
			SelectObject(ghdcMem,TankBrush);

			if(i ==  iTurnOrderID)
				iTank[i].color2 = iTank[i].shield > 0 ? RGB(255,0,0) : RGB(255, 255, 255);
			else
				iTank[i].color2 = iTank[i].shield > 0 ? RGB(255,0,0) : iTank[i].color1;

			if(!iTank[i].dead)
			{

				if(iTank[i].cloaked)//note: this SO does not work anymore.
				{
					// Draw the tank (only if WE'RE the cloaked tank) and the decoy tank.
					if(i == iTurnOrderID)
					{
						drawTank(&iTank[i],iTank[i].location,tangle,iTank[i].tankangle,ghdcMem);
					}
					drawTank(iTank[i].decoy,iTank[i].decoy->location,tangle,iTank[i].decoy->tankangle,ghdcMem);
				}
				else
				{
					drawTank(&iTank[i],iTank[i].location,tangle,iTank[i].tankangle,ghdcMem);
					if(!singlePlayer)
					{
						//if(iTurn == iTurnOrderID)
					//	{
							drawArrow(&iTank[iTurn],iTank[iTurn].newOrigin,iTank[iTurn].location);
					//	}
					}
					else
					{
						if(thinking) 
						{
							if(iTank[i].name == "Computer")
							{
								drawArrow(&iTank[i],iTank[i].newOrigin,iTank[i].location);
							}
						}
						else
						{
							if(iTank[i].name != "Computer")
							{
								drawArrow(&iTank[i],iTank[i].newOrigin,iTank[i].location);
							}
						}
					}
				}
			}
			DeleteObject(TankBrush);
		}
		

		if(launch)
		{
			showWaitingForSlowpokes = false;
			if(currentM > 22) currentM = 1;
			if(currentM == 0) currentM = 1;
			//allowNextTurn = false;

			animaCount++;
			m[currentM].WeaponMissile.origin[0].x = iTank[iTurn].location.x + iTank[iTurn].originCannon.x; //0 for original weapon
			m[currentM].WeaponMissile.origin[0].y = iTank[iTurn].location.y + iTank[iTurn].originCannon.y; //0 for original weapon
			if(animaCount == animation)
			{
				numFrame++;
				int count = 0;
			
				do
				{
					if(!singlePlayer || (iTurn == 0))
					{
						if(m[currentM].WeaponMissile.draw[count])
						{	
							calcMissile(&m[currentM],count,force,angle,numFrame,ghdcMem);
						}
						
						//echan
						if(m[currentM].WeaponName != m[currentM].WeaponSub_Weapon)//echan
						{
						if(m[m[currentM].WeaponSub_Weapon].WeaponMissile.draw[count])
						{
							calcMissile(&m[m[currentM].WeaponSub_Weapon],count,force,angle,numFrame,ghdcMem);
						}
						}

	//					count++;
					}
					else
					{

						if(m[currentM].WeaponMissile.draw[count])//this code is only for computer
						{		
								calcMissile(&m[currentM],count,compForce,iTank[1].angle * ratio,numFrame,ghdcMem);
						}

							if(m[m[currentM].WeaponSub_Weapon].WeaponMissile.draw[count])
							{
								calcMissile(&m[m[currentM].WeaponSub_Weapon],count,compForce,iTank[1].angle * ratio,numFrame,ghdcMem);
							}

					}

				count++;
				}while(count < m[currentM].WeaponMissile.numofmissile);

				int numFalse = 0;

				for(int p = 0; p < m[currentM].WeaponMissile.numofmissile; p++)
				{
					
					if(!m[currentM].WeaponMissile.draw[p])
					{
						numFalse++;
					}

				}




				
				for(int r = 0; r < m[m[currentM].WeaponSub_Weapon].WeaponMissile.numofmissile; r++)

				{
					
					if(!m[m[currentM].WeaponSub_Weapon].WeaponMissile.draw[r])
					{
						numFalse++;
					}
				}

				if( (numFalse == m[currentM].WeaponMissile.numofmissile + m[m[currentM].WeaponSub_Weapon].WeaponMissile.numofmissile)  &&  AfterEffects(m[currentM], 0)  )//echan
				{

							for(int i = 0; i < iTankCount; ++i)
							{
								if(iTank[i].armor <= 0)
								{
									iTank[i].dead = true;
								//	if(!gDeadList.find(iTurn))
										gDeadList.push_back(iTurn);
									dcon << "Killed tank " << i << "\n";
									NetClientUpdate(static_cast<char>(i),static_cast<char>(kNet_TurnKill));// angle of 255 kills the player.
								}
							}

					GameGotoNextTurn();
					dcon << "Current turn now: " << iTurn << "\n";
					launch = false;

					
					
					
					m[currentM].ReInitWeapon();

					(m[currentM].WeaponSub_Weapon != 0) ? m[m[currentM].WeaponSub_Weapon].ReInitWeapon(): 0;


					numFrame = 0;

				}
				animaCount = 0;
			}
		}
	}


	UI_DisplayCurrentScreen ();



	char numbuf[4];
	if (gCurrentScreen == Battle)
	{
		DisplayText(k_angPowTextX, k_TopRowY + k_textOffset10X_Y, itoa(Angle,numbuf,10), ORANGE);
		
		DisplayText(k_angPowTextX, k_bottomRowY + k_textOffset10X_Y, itoa(Power,numbuf,10), ORANGE);

		UpdateFloaters();
		TraceFloaters();
	}

//	if(showWaitingForSlowpokes && !singlePlayer)
//		DisplayText(575,10,"Waiting for other players...",RED);

	

	gMainDC = GetDC(gMainWindow);
	BitBlt(gMainDC,0,0,gresolutionX,gresolutionY,ghdcMem,0,0,SRCCOPY);
	ReleaseDC(gMainWindow,gMainDC);

bool tanksDone = true;

for(int tankdonei = 0; tankdonei < iTankCount; ++tankdonei)
{
		if(iTank[tankdonei].lastLoc.x != iTank[tankdonei].location.x ||
		   iTank[tankdonei].lastLoc.y != iTank[tankdonei].location.y)
		{
			tanksDone = false;
		}
}

		if(!launch && vBlast.size() <= 0 && tanksDone && mustSendReady)
		{
			dcon << "CLIENT HAS FINISHED TURN PROCESSING.\n";
			sentReady = false;
			NetClientIsReady();
			mustSendReady = false;
			showWaitingForSlowpokes = true;

				if(goToPostMortemASAP)
				{
					gCurrentScreen = Title;
					NetResetAllState();
				}
		}
		else
		{
			//dcon << "No: " << vBlast.size() << " " << (tanksDone ? "y" : "n") << " " << (mustSendReady ? "y" : "n") << "\n";
		}

	for(int tankll = 0; tankll < iTankCount; ++tankll)
	{
		iTank[tankll].lastLoc.x = iTank[tankll].location.x;
		iTank[tankll].lastLoc.y = iTank[tankll].location.y;
	}

	if(goToPostMortemASAP && singlePlayer)
	{
		gCurrentScreen = Title;
		NetResetAllState();
	}

}

void GameStartup(void)
{
	// Read items.
	for(int iw = 0; iw < MAX_WEAPONS; ++iw)
	{
		gInitialWeap[iw] = -1;
	}
	for(int ii = 0; ii < MAX_ITEMS; ++ii)
	{
		gInitialItem[ii] = -1;
	}
	ItemRead(iItemList);

	missileInit( &(m[ICBP].WeaponMissile), 0);
	missileInit( &(m[Normal].WeaponMissile), 0);

	
	/*There needs to be a call to Read for each Weapon to fill
	array (to be a vector), check mod_weapon_attributes.h for constants	*/
	m[StandardC].Read("Data/weapons/Weaponfile.txt", "Standard_Cannon");
	m[SuperC].Read("Data/weapons/Weaponfile.txt", "Super_Cannon");
	m[UberC].Read("Data/weapons/Weaponfile.txt", "Uber_Cannon");
	m[FlakC].Read("Data/weapons/Weaponfile.txt", "Flak_Cannon");
	m[Radar_Missile].Read("Data/weapons/Weaponfile.txt", "Radar_Missile");
	m[Thermal_Missile].Read("Data/weapons/Weaponfile.txt", "Thermal_Missile");//echan
	m[Jackhammer].Read("Data/weapons/Weaponfile.txt", "Jackhammer");//echan
	m[Chain_Gun].Read("Data/weapons/Weaponfile.txt", "Chain_Gun");//echan
	m[Drunk_Missile].Read("Data/weapons/Weaponfile.txt", "Drunk_Missile");
	m[Sniper].Read("Data/weapons/Weaponfile.txt", "Sniper");
	m[Carpet].Read("Data/weapons/Weaponfile.txt", "Carpet_Bomb");
	m[Flamethrower].Read("Data/weapons/Weaponfile.txt", "Flamethrower"); //echan;
	m[Napalm].Read("Data/weapons/Weaponfile.txt", "Napalm");
	m[Firestorm].Read("Data/weapons/Weaponfile.txt", "Firestorm"); //echan	
	m[Earthmover].Read("Data/weapons/Weaponfile.txt", "Earthmover");
	m[Dirtball].Read("Data/weapons/Weaponfile.txt", "Dirtball");
	m[Mudslinger].Read("Data/weapons/Weaponfile.txt", "Mudslinger");
	m[Tracer].Read("Data/weapons/Weaponfile.txt", "Tracer");
	m[EMP].Read("Data/weapons/Weaponfile.txt", "EMP");
	m[Artillery].Read("Data/weapons/Weaponfile.txt", "Artillery");
	m[ICBP].Read("Data/weapons/Weaponfile.txt", "ICBP");	

	angle = 95 * ratio;//current angle
}

void GameShutdown()
{
	DeleteDC(ghdcMem);
	DeleteObject(ghBitmap);
	
}


bool GameCanPlayerFire(void)
{
	if(singlePlayer && (iTurn == 0))
		return true;
	else
		return (!launch && (iTurnOrderID == iTurn) && allowNextTurn);
}

bool GameCanPlayerUse(void)
{
	return !iTankUsedItem;
}


void GameHandleKey(WPARAM key)
{
	switch (key)
	{
		case VK_ESCAPE:
			PostQuitMessage(0);
		break;
		default:
		{
			if (inputSelected)
				UIHandleKey(key);
		}
	}	
}


void GameGotoNextTurn(void)
{
int ThisTurn = iTurn;

	iTankUsedItem = false;

	if(singlePlayer)
	{
		if(iTank[0].dead)
		{
			gDeadList.push_back(0);
			gDeadList.push_back(1);
			goToPostMortemASAP = true;
		}
		else if(iTank[1].dead)
		{
			gDeadList.push_back(1);
			gDeadList.push_back(0);
			goToPostMortemASAP = true;
		}
	}

	// Figure out number of tanks left alive.
	int alivectr = 0;
	for(int ijosh = 0; ijosh < iTankCount; ++ijosh)
	{
		if(!iTank[ijosh].dead) ++alivectr;
	}

	if(alivectr <= 1)
	{
	//	if(!gDeadList.find(ThisTurn))
			gDeadList.push_back(ThisTurn);
		
		goToPostMortemASAP = true;
	}
	else
	{
		do
		{
			if(++iTurn >= iTankCount) iTurn = 0;
		}while(iTank[iTurn].dead);
	}

	if(singlePlayer && iTurn)
	{
		thinking = true;
	}

// Handle item durations.
	for(int i = 0; i < iTankCount; ++i)
	{
		ItemApplyDurations(&iTank[i]);
	}

}




void GameUpdateTank(char id,char ang)
{
		if(ang == kNet_TurnDead)
		{
			dcon << "The update packet killed the tank; he must have pinged out.\n";
			iTank[id].dead = true;
		}
		else
		{
			iTank[id].angle = (unsigned char)ang;
		}
}


void GameMouseHover(LPARAM coords)
{
int X = coords & 0x0000FFFF;
int Y = (coords & 0xFFFF0000) >> 16;

	gShowHoverID = -1;
	// For all tanks....
	for(int i = 0; i < iTankCount; ++i)
	{

		if(X >= (iTank[i].location.x - 10) && X <= (iTank[i].location.x + 10) &&
		   Y >= (iTank[i].location.y - 10) && Y <= (iTank[i].location.y + 10))
		{
			gShowHoverID = i;
		}
	} 
}









