#include "stdafx.h"
#include "xrServer.h"

// Main
void				game_sv_GameState::Lock						()
{
	xrServer*		S	= Level().Server;
	S->clients_Lock		();
}
void				game_sv_GameState::Unlock					()
{
	xrServer*		S	= Level().Server;
	S->clients_Unlock	();
}
game_PlayerState*	game_sv_GameState::get_it					(DWORD it)
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->client_Get		(it);
	if (0==C)			return 0;
	else				return &C->ps;
}
game_PlayerState*	game_sv_GameState::get_id					(DWORD id)								// DPNID
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->ID_to_client	(id);
	if (0==C)			return 0;
	else				return &C->ps;
}
string64*			game_sv_GameState::get_name_it				(DWORD it)
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->client_Get		(it);
	if (0==C)			return 0;
	else				return &C->Name;
}
string64*			game_sv_GameState::get_name_id				(DWORD id)								// DPNID
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->ID_to_client	(id);
	if (0==C)			return 0;
	else				return &C->Name;
}

u32					game_sv_GameState::get_count				()
{
	xrServer*		S	= Level().Server;
	return				S->client_Count();
}

// Utilities
u32					game_sv_GameState::get_alive_count			(u32 team)
{
	Device.Fatal	("Not implemented");
	return 0;
}

// Network
void game_sv_GameState::net_Export_State						(NET_Packet& P)
{
}

void game_sv_GameState::net_Export_Update						(NET_Packet& P, DWORD id)
{
	Lock		();
	game_PlayerState* A		= get_id		(id);
	if (A)
	{
		P.w_u32	(id);
		P.w		(A,sizeof(*A));
	}
	Unlock		();
}
