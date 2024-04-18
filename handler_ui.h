// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// 
//	owned by: Jesica McCrory
//
// variables and protoypes for functions used by more than handler_ui and mod_ui 
// =============================================================================


#ifndef _TANK_HANDLER_UI_H
#define _TANK_HANDLER_UI_H
#pragma warning (disable:4786)
#include "tank.h"

 
void HandleMouseUI(HWND wind, WPARAM wParam,LPARAM lParam);

void UIHandleKey (WPARAM wParam);

void UIHandleChat(string ThisChat);

void UIHandleAddHostList(int players, int port, string address, string name);

void ClearPreGameChat();

extern bool inputSelected;

struct HostInfo
{
	int plyrs;
	int port;
	string addrs;	
	string name;
};

extern vector<HostInfo> vec_hostList;

#endif