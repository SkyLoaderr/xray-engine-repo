#include "stdafx.h"
#include "ai_script_processor.h"

game_cl_GameState::game_cl_GameState()
{
	local_player				= 0;
	m_tpScriptProcessor			= 0;
}

game_cl_GameState::~game_cl_GameState()
{
	xr_delete					(m_tpScriptProcessor);
}

void	game_cl_GameState::Create			(LPSTR &options)
{
	// loading scripts
	xr_delete					(m_tpScriptProcessor);
	LPCSTR						caSection = "";
	switch (type) {
		case GAME_ANY			: {
			Debug.fatal			("Game type is not specified!");
		}
		case GAME_SINGLE		: {
			caSection			= "single";
			break;
		}
		case GAME_DEATHMATCH	: {
			caSection			= "deathmatch";
			break;
		}
		case GAME_CTF			: {
			caSection			= "ctf";
			break;
		}
		case GAME_ASSAULT		: {
			caSection			= "assault";
			break;
		}
		case GAME_CS			: {
			caSection			= "cs";
			break;
		}
		default					: NODEFAULT;
	}
	CInifile					*l_tpIniFile = xr_new<CInifile>("script.ltx");
	R_ASSERT					(l_tpIniFile);
	if (l_tpIniFile->r_string(caSection,"script"))
		m_tpScriptProcessor		= xr_new<CScriptProcessor>("Game",l_tpIniFile->r_string(caSection,"script"));
}

void	game_cl_GameState::net_import_state	(NET_Packet& P)
{
	// Generic
	P.r_s32			(type);
	P.r_u16			(phase);
	P.r_s32			(round);
	P.r_u32			(start_time);
	P.r_s32			(fraglimit);
	P.r_s32			(timelimit);
	P.r_u32			(buy_time);

	// Teams
	u16 t_count;
	P.r_u16			(t_count);
	teams.clear		();
	for (u16 t_it=0; t_it<t_count; t_it++)
	{
		game_TeamState	ts;
		P.r				(&ts,sizeof(game_TeamState));
		teams.push_back	(ts);
	}

	// Players
	u16	p_count;
	P.r_u16			(p_count);
	players.clear	();
	std::pair <xr_map<u32,Player>::iterator, bool> I;
	for (u16 p_it=0; p_it<p_count; p_it++)
	{
		u32				ID;
		Player			IP;
		P.r_u32			(ID);
		P.r_string		(IP.name);
		P.r				(&IP,sizeof(game_PlayerState));
		I				= players.insert(mk_pair(ID,IP));
		if (IP.flags&GAME_PLAYER_FLAG_LOCAL) local_player = &I.first->second;
	}
	R_ASSERT(local_player);
}

void	game_cl_GameState::net_import_update(NET_Packet& P)
{
	// Read
	u32					ID;
	game_PlayerState	PS;
	P.r_u32				(ID);
	P.r					(&PS,sizeof(game_PlayerState));

	// Update
	xr_map<u32,Player>::iterator I	= players.find(ID);
	if (I!=players.end())
	{
		Player& IP		= I->second;
		Memory.mem_copy	(&IP,&PS,sizeof(PS));
	}
}

void	game_cl_GameState::net_signal		(NET_Packet& P)
{
}
