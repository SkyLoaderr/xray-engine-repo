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

u32					game_sv_GameState::get_it_2_id				(DWORD it)
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->client_Get		(it);
	if (0==C)			return 0;
	else				return C->ID;
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
s32					game_sv_GameState::get_option_i				(LPCSTR lst, LPCSTR name, s32 def)
{
	string64		op;
	strconcat		(op,"//",name,"=");
	if (strstr(lst,op))	return atoi	(strstr(lst,op)+strlen(op));
	else				return def;
}


// Network
void game_sv_GameState::net_Export_State						(NET_Packet& P)
{
	// Generic
	P.w_s32			(type);
	P.w_s32			(round);
	P.w_s32			(fraglimit);
	P.w_s32			(timelimit);

	// Teams
	P.w_u16			(u16(teams.size()));
	for (u32 t_it=0; t_it<teams.size(); t_it++)
	{
		P.w				(&teams[t_it],sizeof(game_TeamState));
	}

	// Players
	Lock			();
	u32	p_count		= get_count();
	P.w_u16			(u16(p_count));
	for (u32 p_it=0; p_it<p_count; p_it++)
	{
		string64*	p_name	=	get_name_it		(p_it);
		P.w_u32					(get_it_2_id	(p_it));
		P.w_string				(*p_name);
		game_PlayerState* A	=	get_it			(p_it);
		P.w						(A,sizeof(*A));
	}
	Unlock			();
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
