#include "stdafx.h"
#include "hudmanager.h"

game_cl_GameState::game_cl_GameState()
{
	local_player				= 0;
}

game_cl_GameState::~game_cl_GameState()
{
}

void	game_cl_GameState::Create(LPSTR &options)
{
}

void	game_cl_GameState::net_import_state	(NET_Packet& P)
{
	// Generic
	P.r_u32			(local_svdpnid);
	P.r_s32			(type);
	P.r_u16			(phase);
	P.r_s32			(round);
	P.r_u32			(start_time);
	P.r_s32			(fraglimit);
	P.r_s32			(timelimit);
	P.r_u32			(buy_time);

	// Teams
	u16				t_count;
	P.r_u16			(t_count);
	teams.clear		();
	for (u16 t_it=0; t_it<t_count; ++t_it)
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
	for (u16 p_it=0; p_it<p_count; ++p_it)
	{
		u32				ID;
		Player			IP;
		P.r_u32			(ID);
		P.r_string		(IP.name);
//		P.r				(&IP,sizeof(game_PlayerState));
		IP.net_Import	(P);
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
//	P.r					(&PS,sizeof(game_PlayerState));
	PS.net_Import		(P);

	// Update
	xr_map<u32,Player>::iterator I	= players.find(ID);
	if (players.end()!=I)
	{
		Player& IP		= I->second;
		Memory.mem_copy	(&IP,&PS,sizeof(PS));
	}

	if (OnServer())		return;

	//Syncronize GameTime
	u64				GameTime;
	P.r_u64			(GameTime);
	float			TimeFactor;
	P.r_float		(TimeFactor);

	Level().SetGameTime			(GameTime);
	Level().SetGameTimeFactor	(TimeFactor);
}

void	game_cl_GameState::net_signal		(NET_Packet& P)
{
}

void	game_cl_GameState::OnGameMessage	(NET_Packet& P)
{
	if (!HUD().GetUI()) return;
	u32 Message;
	P.r_u32(Message);

	switch (Message)
	{
	case GMSG_PLAYER_CONNECTED:
		{
			string64 PlayerName;
			P.r_string(PlayerName);
			HUD().outMessage			(0xffffffff,"","Player %s connected",PlayerName);
		}break;
	case GMSG_PLAYER_DISCONNECTED:
		{
			string64 PlayerName;
			P.r_string(PlayerName);
			HUD().outMessage			(0xffffffff,"","Player %s disconnected",PlayerName);
		}break;
	case GMSG_PLAYER_KILLED:
		{
			u16 PlayerID, KillerID, WeaponID;
			P.r_u16 (PlayerID);
			P.r_u16 (KillerID);
			P.r_u16 (WeaponID);

//			CObject* pPlayer = Level().Objects.net_Find(PlayerID);
//			CObject* pKiller = Level().Objects.net_Find(KillerID);
			CObject* pWeapon = Level().Objects.net_Find(WeaponID);

			if (pWeapon)
				HUD().outMessage			(0xffffffff,"","%d killed from %d by %d",PlayerID, WeaponID, KillerID);
//			HUD().outMessage			(0xffffffff,"","%s killed from %s by %s",pPlayer->cName(), pWeapon->cName(), pKiller->cName());
			else
				HUD().outMessage			(0xffffffff,"","%d killed by %d",PlayerID, KillerID);
//			HUD().outMessage			(0xffffffff,"","%s killed by %s",pPlayer->cName(), pKiller->cName());
			
		}break;
	default:
		{
			R_ASSERT2(0,"Unknown Game Message");
		}break;
	};
};
