#include "stdafx.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "UIGameCustom.h"
#include "script_engine.h"
#include "script_space.h"
#include "xr_Level_controller.h"

game_cl_GameState::game_cl_GameState()
{
	local_player				= 0;
	m_game_type_name[0]			= 0;

	shedule.t_min				= 5;
	shedule.t_max				= 20;
	m_game_ui_custom			= NULL;
	shedule_register();
	
}

game_cl_GameState::~game_cl_GameState()
{
	shedule_unregister();
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
//time
	u64				GameTime;
	P.r_u64			(GameTime);
	float			TimeFactor;
	P.r_float		(TimeFactor);

	Level().SetGameTimeFactor	(GameTime,TimeFactor);
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
	u64				GameTime;
	P.r_u64			(GameTime);
	float			TimeFactor;
	P.r_float		(TimeFactor);

	Level().SetGameTimeFactor	(GameTime,TimeFactor);
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
	case GAME_EVENT_PLAYER_CONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);
			
			sprintf(Text, "%sPlayer %s%s %sconnected",Color_Main,Color_Teams[0],PlayerName,Color_Main);
			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_PLAYER_DISCONNECTED:
		{
			string64 PlayerName;
			P.r_stringZ(PlayerName);

			sprintf(Text, "%sPlayer %s%s %sdisconnected",Color_Main,Color_Teams[0],PlayerName,Color_Main);
			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_ROUND_STARTED:
		{
			sprintf(Text, "%sRound statred !!!",Color_Main);
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
	u32 msg;
	P.r_u32(msg);

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
	if (!HUD().GetUI()) 
		return;
	HUD().GetUI()->UIMainIngameWnd.AddGameMessage(msg);
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
}

void game_cl_GameState::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	if( pDialog->IsShown() )
		StopMenu(pDialog, bDoHideIndicators);
	else
		StartMenu(pDialog, bDoHideIndicators);

	xr_vector<CUIWindow*>::iterator it = std::find(m_game_ui_custom->m_dialogsToErase.begin(), m_game_ui_custom->m_dialogsToErase.end(), pDialog);
	if (m_game_ui_custom->m_dialogsToErase.end() != it)
		m_game_ui_custom->m_dialogsToErase.erase(it);
}

void game_cl_GameState::StartMenu (CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	if(m_game_ui_custom->MainInputReceiver() != NULL) return;

	R_ASSERT( !pDialog->IsShown() );
	R_ASSERT( m_game_ui_custom->MainInputReceiver() == NULL );

	m_game_ui_custom->AddDialogToRender(pDialog);
	m_game_ui_custom->SetMainInputReceiver(pDialog);
	pDialog->Show();

	if(bDoHideIndicators){
		HUD().GetUI()->HideIndicators();
		HUD().GetUI()->ShowCursor();
		m_bCrosshair = !!psHUD_Flags.test(HUD_CROSSHAIR);
		if(m_bCrosshair) 
			psHUD_Flags.set(HUD_CROSSHAIR, FALSE);
	}

}

void game_cl_GameState::StopMenu (CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	R_ASSERT( pDialog->IsShown() );
	R_ASSERT( m_game_ui_custom->MainInputReceiver()==pDialog );

	m_game_ui_custom->RemoveDialogToRender(pDialog);
	m_game_ui_custom->SetMainInputReceiver(NULL);
	pDialog->Hide();

	if(bDoHideIndicators){
	HUD().GetUI()->ShowIndicators();
	HUD().GetUI()->HideCursor();
	if(m_bCrosshair) 
		psHUD_Flags.set(HUD_CROSSHAIR, TRUE);
	};
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
