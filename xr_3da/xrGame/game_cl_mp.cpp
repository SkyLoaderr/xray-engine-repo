#include "stdafx.h"
#include "game_cl_mp.h"
#include "xr_level_controller.h"
#include "xrMessages.h"
#include "GameObject.h"
#include "Actor.h"
#include "level.h"
#include "hudmanager.h"
#include "ui/UIChatWnd.h"
#include "ui/UIChatLog.h"
#include "ui/UIGameLog.h"
#include "clsid_game.h"
#include <dinput.h>
#include "UIGameCustom.h"

game_cl_mp::game_cl_mp()
{
	pChatWnd		= NULL;
	pChatLog		= NULL;
	pGameLog		= NULL;

	m_bVotingActive = false;
};

game_cl_mp::~game_cl_mp()
{
	xr_delete(pChatWnd);
	xr_delete(pChatLog);

	CL_TEAM_DATA_LIST_it it = TeamList.begin();
	for(;it!=TeamList.end();++it)
	{
		if (it->IndicatorShader)
			it->IndicatorShader.destroy();
		if (it->InvincibleShader)
			it->InvincibleShader.destroy();
	};
	TeamList.clear();
};

CUIGameCustom*		game_cl_mp::createGameUI			()
{
	pChatLog = xr_new<CUIChatLog>();
	pChatLog->Init();

	pGameLog = xr_new<CUIGameLog>();
	pGameLog->Init();


	pChatWnd = xr_new<CUIChatWnd>(pChatLog);
	pChatWnd->Init();
	pChatWnd->SetOwner(this);
	return NULL;
};

bool game_cl_mp::CanBeReady	()
{
	return true;
}

bool	game_cl_mp::OnKeyboardPress			(int key)
{
	if ( kJUMP == key || kWPN_FIRE == key )
	{
		bool b_need_to_send_ready = false;

		CObject* curr = Level().CurrentEntity();
		if (!curr) return(false);

		bool is_actor		= curr->CLS_ID==CLSID_OBJECT_ACTOR;
		bool is_spectator	= curr->CLS_ID==CLSID_SPECTATOR;
		
		game_PlayerState* ps	= local_player;

		if (is_actor){
			b_need_to_send_ready = ((GAME_PHASE_PENDING == Phase() ) || 
									true == ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) ) && 
									(kWPN_FIRE == key);
		};
		if(is_spectator){
			b_need_to_send_ready =	( GAME_PHASE_PENDING	== Phase() && kWPN_FIRE == key) || 
									( kJUMP == key && GAME_PHASE_INPROGRESS	== Phase() && CanBeReady() );

		};
		if(b_need_to_send_ready){
				CGameObject* GO = smart_cast<CGameObject*>(curr);
				NET_Packet			P;
				GO->u_EventGen		(P,GE_GAME_EVENT,GO->ID()	);
				P.w_u16(GAME_EVENT_PLAYER_READY);
				GO->u_EventSend			(P);
				return true;
		}
		else
			return false;
	};

/*from actorinput.cpp
	if ( (GAME_PHASE_PENDING == Game().phase) || !g_Alive() )
	{
		if (kWPN_FIRE == cmd)	
		{
			// Switch our "ready" status
			NET_Packet			P;
//			u_EventGen			(P,GEG_PLAYER_READY,ID());
			u_EventGen		(P,GE_GAME_EVENT,ID()	);
			P.w_u16(GAME_EVENT_PLAYER_READY);

			u_EventSend			(P);
		}
		return;
	}

*/
/*from spectator.cpp
	if (kJUMP == cmd || kWPN_FIRE == cmd)
	{
		if ((GAME_PHASE_PENDING	== Game().phase && kWPN_FIRE == cmd) || 
			(kJUMP == cmd && GAME_PHASE_INPROGRESS	== Game().phase && HUD().GetUI()->UIGame()->CanBeReady()))
		{
			NET_Packet			P;

			u_EventGen		(P,GE_GAME_EVENT,ID()	);
			P.w_u16(GAME_EVENT_PLAYER_READY);

			u_EventSend			(P);
			return;
		}
		else
			return;
	};
*/

	if( (Phase() != GAME_PHASE_INPROGRESS) && (kQUIT != key) && (kCONSOLE != key))
		return true;

	if (Phase() == GAME_PHASE_INPROGRESS)
	{
		if ((kCHAT == key || kCHAT_TEAM == key) && pChatWnd)
		{
			shared_str prefix;

			if (kCHAT_TEAM == key)
			{
				prefix = "to team> ";
				pChatWnd->TeamChat();
			}
			else
			{
				prefix = "to all> ";
				pChatWnd->AllChat();
			}

			pChatWnd->SetEditBoxPrefix(prefix);

			StartStopMenu(pChatWnd, false);
			if (!pChatWnd->IsShown() && xr_strlen(pChatWnd->UIEditBox.GetText()) > 0)
			{
				shared_str phrase = pChatWnd->UIEditBox.GetText();
//				pChatWnd->Say(phrase);
				(kCHAT == key) ? ChatSayAll(phrase) : ChatSayTeam(phrase);
				pChatWnd->UIEditBox.SetText("");
			}
			return false;
		}

		if (IsVoteEnabled() && IsVotingActive())
		{
			if (key == kVOTEYES)
			{
				SendVoteYesMessage();
			};
			if (key == kVOTENO)
			{
				SendVoteNoMessage();
			};
		}
	}

	return inherited::OnKeyboardPress(key);
}

bool	game_cl_mp::OnKeyboardRelease		(int key)
{
	return inherited::OnKeyboardRelease(key);
}

void game_cl_mp::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
	char	Color_Weapon[]	= "%c255,0,0";
	LPSTR	Color_Teams[3]	= {"%c255,255,255", "%c64,255,64", "%c64,64,255"};
	char	Color_Main[]	= "%c192,192,192";

	switch(msg)	{
	
	case GAME_EVENT_PLAYER_KILLED: //dm
		{
			u16 PlayerID, KillerID, WeaponID;
			P.r_u16 (PlayerID);
			P.r_u16 (KillerID);
			P.r_u16 (WeaponID);

			game_PlayerState* pPlayer = GetPlayerByGameID(PlayerID);
			game_PlayerState* pKiller = GetPlayerByGameID(KillerID);
			CObject* pWeapon = Level().Objects.net_Find(WeaponID);
			if (!pPlayer || !pKiller) break;
			
			if (pKiller && pWeapon && WeaponID != 0)
				sprintf(Text, "%s%s %skilled from %s%s %sby %s%s", 
								Color_Teams[pPlayer->team], 
								pPlayer->name, 
								Color_Main,
								Color_Weapon,
								*(pWeapon->cName()), 
								Color_Main,
								Color_Teams[pKiller->team], 
								pKiller->name);
			else
				sprintf(Text, "%s%s %skilled by %s%s",
								Color_Teams[pPlayer->team],
								pPlayer->name, 
								Color_Main,
								Color_Teams[pKiller->team],
								pKiller->name);

			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_VOTE_START:
		{
			sprintf(Text, "%sVoting started!", 
				Color_Main);
			CommonMessageOut(Text);
			OnVoteStart(P);
		}break;
	case GAME_EVENT_VOTE_STOP:
		{
			sprintf(Text, "%sVoting breaked by server!", 
				Color_Main);
			CommonMessageOut(Text);

			OnVoteStop(P);
		}break;
	case GAME_EVENT_VOTE_END:
		{
			string512 Reason;
			P.r_stringZ(Reason);
			sprintf(Text, "%s%s", 
				Color_Main, 
				Reason);
			CommonMessageOut(Text);

			OnVoteEnd(P);
		}break;
	default:
		inherited::TranslateGameMessage(msg,P);
	}
}


//////////////////////////////////////////////////////////////////////////

void game_cl_mp::ChatSayAll(const shared_str &phrase)
{
	NET_Packet	P;	
	P.w_begin(M_CHAT_MESSAGE);
//	P.w_u16(local_player->GameID);
	P.w_s16(0);
	P.w_stringZ(local_player->getName());
	P.w_stringZ(phrase.c_str());
	u_EventSend(P);
}

//////////////////////////////////////////////////////////////////////////

void game_cl_mp::ChatSayTeam(const shared_str &phrase)
{

	NET_Packet	P;
	P.w_begin(M_CHAT_MESSAGE);
//	P.w_u16(local_player->GameID);
	P.w_s16(local_player->team);
	P.w_stringZ(local_player->getName());
	P.w_stringZ(phrase.c_str());
	u_EventSend(P);
}

void game_cl_mp::OnChatMessage			(NET_Packet* P)
{
//	u16 PlayerID	= P->r_u16();
//	s16 PlayerTeam	= 
	P->r_s16();
	string256 PlayerName;
	P->r_stringZ(PlayerName);
	string256 ChatMsg;
	P->r_stringZ(ChatMsg);
	
	Msg("%s : %s", PlayerName, ChatMsg);
	if (pChatLog) pChatLog->AddLogMessage(ChatMsg, PlayerName);
};

void game_cl_mp::CommonMessageOut		(LPCSTR msg)
{
//	if (pChatLog) pChatLog->AddLogMessage(msg, "");
	if (pGameLog) pGameLog->AddLogMessage(msg);
};
//////////////////////////////////////////////////////////////////////////
void game_cl_mp::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	static offFlag = false;
	if (!offFlag)
	{
		if (HUD().GetUI()->UIGame())
		{
			HUD().GetUI()->UIGame()->AddDialogToRender(pChatLog);
			HUD().GetUI()->UIGame()->AddDialogToRender(pGameLog);
			offFlag = true;
		}
	}
}

void game_cl_mp::SendStartVoteMessage	(LPCSTR args)
{
	if (!args) return;
	if (!IsVoteEnabled()) return;
	NET_Packet P;
	Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
	P.w_u16(GAME_EVENT_VOTE_START);
	P.w_stringZ(args);
	Game().u_EventSend		(P);
};

void game_cl_mp::SendVoteYesMessage		()	
{
	if (!IsVoteEnabled() || !IsVotingActive()) return;
	NET_Packet P;
	Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
	P.w_u16(GAME_EVENT_VOTE_YES);
	Game().u_EventSend		(P);
};
void game_cl_mp::SendVoteNoMessage		()	
{
	if (!IsVoteEnabled() || !IsVotingActive()) return;
	NET_Packet P;
	Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
	P.w_u16(GAME_EVENT_VOTE_NO);
	Game().u_EventSend		(P);
};

void game_cl_mp::OnVoteStart				(NET_Packet& P)	
{
	SetVotingActive(true);
};
void game_cl_mp::OnVoteStop				(NET_Packet& P)	
{
	SetVotingActive(false);
};

void game_cl_mp::OnVoteEnd				(NET_Packet& P)
{
	SetVotingActive(false);
};

void game_cl_mp::LoadTeamData			(char* TeamName)
{
	cl_TeamStruct Team;
	ZeroMemory(&Team, sizeof(Team));

	Team.caSection = TeamName;
	if (pSettings->section_exist(TeamName))
	{
		Team.Indicator_r1 =  pSettings->r_float(TeamName, "indicator_r1");
		Team.Indicator_r2 =  pSettings->r_float(TeamName, "indicator_r2");

		Team.IndicatorPos.x =  pSettings->r_float(TeamName, "indicator_x");
		Team.IndicatorPos.y =  pSettings->r_float(TeamName, "indicator_y");
		Team.IndicatorPos.z =  pSettings->r_float(TeamName, "indicator_z");
		
		LPCSTR ShaderType	= pSettings->r_string(TeamName, "indicator_shader");
		LPCSTR ShaderTexture = pSettings->r_string(TeamName, "indicator_texture");
		Team.IndicatorShader.create(ShaderType, ShaderTexture);

		ShaderType	= pSettings->r_string(TeamName, "invincible_shader");
		ShaderTexture = pSettings->r_string(TeamName, "invincible_texture");
		Team.InvincibleShader.create(ShaderType, ShaderTexture);
	};
	TeamList.push_back(Team);
}

void game_cl_mp::OnSwitchPhase			(u32 old_phase, u32 new_phase)
{
	switch (new_phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			if (HUD().GetUI())
			{
				HUD().GetUI()->ShowIndicators();
			};
		}break;
	default:
		{
			if (HUD().GetUI())
			{
				HUD().GetUI()->HideIndicators();
			};
		}break;
	}
}