#include "stdafx.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "UIGameCustom.h"
#include "script_engine.h"
#include "script_space.h"

game_cl_GameState::game_cl_GameState()
{
	local_player				= 0;
	m_game_type_name[0]			= 0;
}

game_cl_GameState::~game_cl_GameState()
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
//	P.r_s32			(fraglimit);
//	P.r_s32			(timelimit);
//	P.r_u32			(buy_time);

//	// Teams
//	u16				t_count;
//	P.r_u16			(t_count);
//	teams.clear		();
//	for (u16 t_it=0; t_it<t_count; ++t_it)
//	{
//		game_TeamState	ts;
//		P.r				(&ts,sizeof(game_TeamState));
//		teams.push_back	(ts);
//	}

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
		P.r_stringZ		(IP.name);
//		P.r				(&IP,sizeof(game_PlayerState));
		IP.net_Import	(P);
		I				= players.insert(mk_pair(ID,IP));
		if (IP.flags&GAME_PLAYER_FLAG_LOCAL) local_player = &I.first->second;
	}
	
}

void	game_cl_GameState::net_import_update(NET_Packet& P)
{
	// Read
	u32					ID;
	P.r_u32				(ID);

	// Update
	xr_map<u32,Player>::iterator I	= players.find(ID);
	if (players.end()!=I)
	{
		Player& IP		= I->second;
//		Memory.mem_copy	(&IP,&PS,sizeof(PS));
		IP.net_Import		(P);
	}
	else
	{
		game_PlayerState	PS;
		PS.net_Import		(P);
	};

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

void game_cl_GameState::TranslateGameMessage	(u32 msg, NET_Packet& P)
{

	string512 Text;
	char	Color_Main[]	= "%c192,192,192";
	LPSTR	Color_Teams[3]	= {"%c255,255,255", "%c64,255,64", "%c64,64,255"};

	switch (msg)
	{
	case GMSG_PLAYER_CONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);
			
			sprintf(Text, "%sPlayer %s%s %sconnected",Color_Main,Color_Teams[0],PlayerName,Color_Main);
			UIMessageOut(Text);
		}break;
	case GMSG_PLAYER_DISCONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);

			sprintf(Text, "%sPlayer %s%s %sdisconnected",Color_Main,Color_Teams[0],PlayerName,Color_Main);
			UIMessageOut(Text);
		}break;
	default:
		{
			R_ASSERT2(0,"Unknown Game Message");
		}break;
	};

}

void	game_cl_GameState::OnGameMessage	(NET_Packet& P)
{
	if (!HUD().GetUI()) return;
	u32 msg;
	P.r_u32(msg);

	TranslateGameMessage(msg, P);
};

game_cl_GameState::Player* game_cl_GameState::GetPlayerByGameID(u32 GameID)
{
	xr_map<u32,game_cl_GameState::Player>::iterator I=players.begin();
	xr_map<u32,game_cl_GameState::Player>::iterator E=players.end();

	for (;I!=E;++I)
	{
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)(&I->second);
		if (P->GameID == GameID) return P;
	};
	return NULL;
};

char*	game_cl_GameState::getTeamSection(int Team)
{
	switch (type)
	{
	case GAME_SINGLE:
		{
			return NULL;
		}break;
	case GAME_DEATHMATCH:
		{
			return "deathmatch_team0";
		}break;
	case GAME_TEAMDEATHMATCH:
		{
			switch (Team)
			{
			case 1:
				{
					return "teamdeathmatch_team1";
				}break;
			case 2:
				{
					return "teamdeathmatch_team2";
				}break;
			};
		}break;
	case GAME_ARTEFACTHUNT:
		{
			switch (Team)
			{
			case 1:
				{
					return "artefacthunt_team1";
				}break;
			case 2:
				{
					return "artefacthunt_team2";
				}break;
			};
		}break;
	}
	return NULL;
};


void game_cl_GameState::UIMessageOut (LPCSTR msg)
{
		HUD().GetUI()->UIMainIngameWnd.AddGameMessage(NULL, msg);
}

/*
CLASS_ID game_cl_GameState::getGameUICLASS_ID(LPCSTR game_type_name)
{
	string256		S;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);

	string256				I;
	strcpy(I,l_tpIniFile->r_string("common","ui_type_clsid_factory"));

	luabind::functor<LPCSTR>	result;
	R_ASSERT					(ai().script_engine().functor(I,result));
	ref_str clsid = result		(game_type_name);

	xr_delete			(l_tpIniFile);
	
	return TEXT2CLSID(*clsid);

}

CUIGameCustom*		game_cl_GameState::createGameUI	()
{
	return 0;
	CLASS_ID clsid			= getGameUICLASS_ID( type_name() );
	CUIGameCustom*			pUIGame	= dynamic_cast<CUIGameCustom*> ( NEW_INSTANCE ( clsid ) );

	pUIGame->SetClGame(this);
	pUIGame->Init();
	return pUIGame;
}
*/