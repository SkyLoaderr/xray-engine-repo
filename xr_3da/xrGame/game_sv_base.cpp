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
game_PlayerState*	game_sv_GameState::get_it					(u32 it)
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->client_Get		(it);
	if (0==C)			return 0;
	else				return &C->ps;
}
game_PlayerState*	game_sv_GameState::get_id					(u32 id)								// DPNID
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->ID_to_client	(id);
	if (0==C)			return 0;
	else				return &C->ps;
}

u32					game_sv_GameState::get_it_2_id				(u32 it)
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->client_Get		(it);
	if (0==C)			return 0;
	else				return C->ID;
}

string64*			game_sv_GameState::get_name_it				(u32 it)
{
	xrServer*		S	= Level().Server;
	xrClientData*	C	= (xrClientData*)S->client_Get		(it);
	if (0==C)			return 0;
	else				return &C->Name;
}
string64*			game_sv_GameState::get_name_id				(u32 id)								// DPNID
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
	strconcat		(op,"/",name,"=");
	if (strstr(lst,op))	return atoi	(strstr(lst,op)+strlen(op));
	else				return def;
}
string64&			game_sv_GameState::get_option_s				(LPCSTR lst, LPCSTR name, LPCSTR def)
{
	static __declspec(thread) string64	ret;

	string64		op;
	strconcat		(op,"/",name,"=");
	LPCSTR			start	= strstr(lst,op);
	if (start)		
	{
		LPCSTR			begin	= start + strlen(op); 
		sscanf			(begin, "%[^/]",ret);
	}
	else			
	{
		if (def)	strcpy		(ret,def);
		else		ret[0]=0;
	}
	return ret;
}
void				game_sv_GameState::signal_Syncronize		()
{
	sv_force_sync	= TRUE;
}
void				game_sv_GameState::switch_Phase				(u32 new_phase)
{
	phase				= u16(new_phase);
	start_time			= Device.TimerAsync();
	signal_Syncronize	();
}

// Network
void game_sv_GameState::net_Export_State						(NET_Packet& P, u32 to)
{
	// Generic
	P.w_s32			(type);
	P.w_u16			(phase);
	P.w_s32			(round);
	P.w_u32			(start_time);
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
	game_PlayerState*	Base	= get_id(to);
	for (u32 p_it=0; p_it<p_count; p_it++)
	{
		string64*	p_name		=	get_name_it		(p_it);
		game_PlayerState* A		=	get_it			(p_it);
		game_PlayerState copy	=	*A;
		if (Base==A)	
		{
			copy.flags	|=		GAME_PLAYER_FLAG_LOCAL;
		}

		P.w_u32					(get_it_2_id	(p_it));
		P.w_string				(*p_name);
		P.w						(&copy,sizeof(game_PlayerState));
	}
	Unlock			();
}

void game_sv_GameState::net_Export_Update						(NET_Packet& P, u32 id_to, u32 id)
{
	Lock		();
	game_PlayerState* A		= get_id		(id);
	if (A)
	{
		game_PlayerState copy	=	*A;
		if (id==id_to)	
		{
			copy.flags	|=		GAME_PLAYER_FLAG_LOCAL;
		}

		P.w_u32	(id);
		P.w		(&copy,sizeof(game_PlayerState));
	}
	Unlock		();
}

void game_sv_GameState::OnRoundStart			()
{
	switch_Phase	(GAME_PHASE_INPROGRESS);
	round			++;

	// clear "ready" flag
	Lock	();
	u32		cnt		= get_count	();
	for		(u32 it=0; it<cnt; it++)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->flags				&=	~GAME_PLAYER_FLAG_READY;
	}
	Unlock	();
}

void game_sv_GameState::OnRoundEnd				(LPCSTR reason)
{
	switch_Phase		(GAME_PHASE_PENDING);
}

void game_sv_GameState::OnPlayerConnect			(u32 id_who)
{
	signal_Syncronize	();
}

void game_sv_GameState::OnPlayerDisconnect		(u32 id_who)
{
	signal_Syncronize	();
}
