// =============================================================================
// TANKGAME
//
// (c) 2003 Team 1A, DigiPen Institute of Technology
// =============================================================================


// -----------------------------------------------------------------------------
//                                                                         guard
// -----------------------------------------------------------------------------

#ifndef _TANK_HANDLER_NETWORK_H_
#define _TANK_HANDLER_NETWORK_H_

#pragma warning (disable:4786)
// -----------------------------------------------------------------------------
//                                                                      includes
// -----------------------------------------------------------------------------

// tank
#include "tank.h"


// -----------------------------------------------------------------------------
//                                                                    prototypes
// -----------------------------------------------------------------------------

// handler
void HandleNetwork(HWND wind,WPARAM wParam,LPARAM lParam);

// packet handlers
void HandleNetworkIncoming(WPARAM sock,char *packet);

// action handlers
void HandleNetworkPingAction(WPARAM sock,char *packet);
void HandleNetworkJoinAction(WPARAM sock,char *packet);
void HandleNetworkChatAction(WPARAM sock,char *packet);
void HandleNetworkTurnAction(WPARAM sock,char *packet);
void HandleNetworkUpdateAction(WPARAM sock,char *packet);


#endif

