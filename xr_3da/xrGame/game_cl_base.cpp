#include "stdafx.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "UIGameCustom.h"
#include "script_engine.h"
#include "script_space.h"
#include "xr_Level_controller.h"
#include "ui/UIMainIngameWnd.h"

game_cl_GameState::game_cl_GameState()
{
	local_player				= 0;
	m_game_type_name[0]			= 0;

	shedule.t_min				= 5;
	shedule.t_max				= 20;
	m_game_ui_custom			= NULL;
	shedule_register			();

	m_bVotingEnabled			= false;
	m_bServerControlHits		= true;
}

game_cl_GameState::~game_cl_GameState()
{
	shedule_unregister();
}

void	game_cl_GameState::net_import_GameTime		(NET_Packet& P)
{
	//time
	u64				GameTime;
	P.r_u64			(GameTime);
	float			TimeFactor;
	P.r_float		(TimeFactor);

	Level().SetGameTimeFactor	(GameTime,TimeFactor);

	u64				GameEnvironmentTime;
	P.r_u64			(GameEnvironmentTime);
	float			EnvironmentTimeFactor;
	P.r_float		(EnvironmentTimeFactor);

	Level().SetEnvironmentGameTimeFactor	(GameEnvironmentTime,EnvironmentTimeFactor);
}

void	game_cl_GameState::net_import_state	(NET_Packet& P)
{
	// Generic
	P.r_clientID	(local_svdpnid);
	P.r_s32			(type);
	
	u16 ph;
	P.r_u16			(ph);
	
	if(Phase()!=ph)
		switch_Phase(ph);

	P.r_s32			(round);
	P.r_u32			(start_time);
	m_bVotingEnabled = !!P.r_u8();
	m_bServerControlHits = !!P.r_u8();	
	Game().m_WeaponUsageStatistic.SetCollectData(!!P.r_u8());

	// Players
	u16	p_count;
	P.r_u16			(p_count);
	
	PLAYERS_MAP players_new;

/*
	players.clear	();
*/
	PLAYERS_MAP_IT I;
	for (u16 p_it=0; p_it<p_count; ++p_it)
	{
		ClientID			ID;
		P.r_clientID		(ID);
		game_PlayerState*   IP;
		I = players.find(ID);
		if( I!=players.end() ){
			IP = I->second;
			IP->net_Import(P);
			players_new.insert(mk_pair(ID,IP));
			players.erase(I);
		}else{
			IP = createPlayerState();
			IP->net_Import		(P);
			players_new.insert(mk_pair(ID,IP));
		}
		if (IP->testFlag(GAME_PLAYER_FLAG_LOCAL) ) local_player = IP;


/*		game_PlayerState*	IP = createPlayerState();
		IP->net_Import		(P);
		players.insert(mk_pair(ID,IP));
		if (IP->testFlag(GAME_PLAYER_FLAG_LOCAL) ) local_player = IP;
*/
	}

	I	= players.begin();
	for(;I!=players.end(); ++I)
		xr_delete(I->second);
	players.clear();
	
	players = players_new;

	net_import_GameTime(P);
}

void	game_cl_GameState::net_import_update(NET_Packet& P)
{
	// Read
	ClientID			ID;
	P.r_clientID		(ID);

	// Update
	PLAYERS_MAP_IT I	= players.find(ID);
	if (players.end()!=I)
	{
		game_PlayerState* IP		= I->second;
//		Memory.mem_copy	(&IP,&PS,sizeof(PS));
		IP->net_Import		(P);
	}
	else
	{
		game_PlayerState*	PS = createPlayerState();
		PS->net_Import		(P);
		xr_delete(PS);
	};

//	if (OnServer())		return;

	//Syncronize GameTime
	net_import_GameTime (P);
}

void	game_cl_GameState::net_signal		(NET_Packet& P)
{
}

void game_cl_GameState::TranslateGameMessage	(u32 msg, NET_Packet& P)
{

	string512 Text;
	char	Color_Main[]	= "%c192,192,192";
	LPSTR	Color_Teams[3]	= {"%c255,240,190", "%c64,255,64", "%c64,64,255"};

	switch (msg)
	{
	case GAME_EVENT_PLAYER_CONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);
			
			sprintf(Text, "%s%s %sconnected",Color_Teams[0],PlayerName,Color_Main);
			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_PLAYER_DISCONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);

			sprintf(Text, "%s%s %sdisconnected",Color_Teams[0],PlayerName,Color_Main);
			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_PLAYER_ENTERED_GAME:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);

			sprintf(Text, "%s%s %sentered the game",Color_Teams[0],PlayerName,Color_Main);
			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_ROUND_STARTED:
		{
			sprintf(Text, "%sRound started !!!",Color_Main);
			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_ROUND_END:
		{
			string64 reason;
			P.r_stringZ(reason);

			sprintf(Text, "%sRound Over (%s)", Color_Main, reason);
			CommonMessageOut(Text);
		}break;




	default:
		{
			R_ASSERT2(0,"Unknown Game Message");
		}break;
	};

}

void	game_cl_GameState::OnGameMessage	(NET_Packet& P)
{
	VERIFY	(this && &P);
	u32 msg	;
	P.r_u32	(msg);

	TranslateGameMessage(msg, P);
};

game_PlayerState* game_cl_GameState::GetPlayerByGameID(u32 GameID)
{
	PLAYERS_MAP_IT I=players.begin();
	PLAYERS_MAP_IT E=players.end();

	for (;I!=E;++I)
	{
		game_PlayerState* P = I->second;
		if (P->GameID == GameID) return P;
	};
	return NULL;
};

game_PlayerState* game_cl_GameState::GetPlayerByOrderID		(u32 idx)
{
	PLAYERS_MAP_IT I = players.begin();
	std::advance(I,idx);
	game_PlayerState* ps = I->second;
	return ps;
}

ClientID game_cl_GameState::GetClientIDByOrderID	(u32 idx)
{
	PLAYERS_MAP_IT I = players.begin();
	std::advance(I,idx);
	return I->first;
}



void game_cl_GameState::CommonMessageOut (LPCSTR msg)
{
	if (!HUD().GetUI()) return;
	HUD().GetUI()->UIMainIngameWnd->AddGameMessage(msg, -1, 60000);
}

float game_cl_GameState::shedule_Scale		()
{
	return 1.0f;
}

void game_cl_GameState::shedule_Update		(u32 dt)
{
	ISheduled::shedule_Update	(dt);
	if(!m_game_ui_custom){
		if( HUD().GetUI() )
			m_game_ui_custom = HUD().GetUI()->UIGame();
	} 
	//---------------------------------------
	switch (Phase())
	{
	case GAME_PHASE_INPROGRESS:
		{
			if (GameID() != GAME_SINGLE)
				m_WeaponUsageStatistic.Update();
		}break;
	default:
		{
		}break;
	};
};

void game_cl_GameState::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	HUD().GetUI()->StartStopMenu(pDialog, bDoHideIndicators);
}

void game_cl_GameState::sv_GameEventGen(NET_Packet& P)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());
	P.w_u16		( u16(GE_GAME_EVENT&0xffff) );
	P.w_u16		(0);//dest==0
}

void	game_cl_GameState::sv_EventSend(NET_Packet& P)
{
	Level().Send(P,net_flags(TRUE,TRUE));
}

bool game_cl_GameState::IR_OnKeyboardPress		(int dik)
{
	return OnKeyboardPress(key_binding[dik]);
}

bool game_cl_GameState::IR_OnKeyboardRelease	(int dik)
{
	return OnKeyboardRelease(key_binding[dik]);
}

bool game_cl_GameState::IR_OnMouseMove			(int dx, int dy)
{
	return false;	
}
bool game_cl_GameState::IR_OnMouseWheel			(int direction)
{
	return false;
}

void game_cl_GameState::u_EventGen(NET_Packet& P, u16 type, u16 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());
	P.w_u16		(type);
	P.w_u16		(dest);
}

void game_cl_GameState::u_EventSend(NET_Packet& P)
{
	Level().Send(P,net_flags(TRUE,TRUE));
}

BOOL	g_bStatisticSaveAuto = FALSE;
void				game_cl_GameState::OnSwitchPhase			(u32 old_phase, u32 new_phase)
{
	switch (old_phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			if (g_bStatisticSaveAuto)
				m_WeaponUsageStatistic.SaveData();
		}break;
	default:
		{
		}break;
	};

	switch (new_phase)
	{
		case GAME_PHASE_INPROGRESS:
			{
				m_WeaponUsageStatistic.Clear();
			}break;
		default:
			{
			}break;
	}	
}