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
#include "ui/UIInventoryUtilities.h"
#include "ui/UIMessagesWindow.h"
#include "ui/UIMainIngameWnd.h"
#include "CustomZone.h"
#include "game_base_kill_type.h"

#define EQUIPMENT_ICONS "ui\\ui_mp_icon_kill"
#define KILLEVENT_ICONS "ui\\ui_hud_mp_icon_death"
#define RADIATION_ICONS "ui\\ui_mn_radiations_hard"
#define BLOODLOSS_ICONS "ui\\ui_mn_wounds_hard"
#define RANK_ICONS		"ui\\ui_mp_icon_rank"

#define KILLEVENT_GRID_WIDTH	64
#define KILLEVENT_GRID_HEIGHT	64

#include "game_cl_mp_snd_messages.h"

game_cl_mp::game_cl_mp()
{
	m_bVotingActive = false;
	
	m_pSndMessages.clear();
	LoadSndMessages();
	m_bJustRestarted = true;
	m_pSndMessagesInPlay.clear();
};

game_cl_mp::~game_cl_mp()
{
	CL_TEAM_DATA_LIST_it it = TeamList.begin();
	for(;it!=TeamList.end();++it)
	{
		if (it->IndicatorShader)
			it->IndicatorShader.destroy();
		if (it->InvincibleShader)
			it->InvincibleShader.destroy();
	};
	TeamList.clear();

	if (m_EquipmentIconsShader)
		m_EquipmentIconsShader.destroy();
	
	if (m_KillEventIconsShader)
		m_KillEventIconsShader.destroy();

	if (m_RadiationIconsShader)
		m_RadiationIconsShader.destroy();

	if (m_BloodLossIconsShader)
		m_BloodLossIconsShader.destroy();
	
	m_pSndMessagesInPlay.clear_and_free();
	m_pSndMessages.clear_and_free();
};

CUIGameCustom*		game_cl_mp::createGameUI			()
{
	//pChatLog = xr_new<CUIChatLog>();
	//pChatLog->Init();

	//pGameLog = xr_new<CUIGameLog>();
	//pGameLog->Init();


	//pChatWnd = xr_new<CUIChatWnd>(pChatLog);
	//pChatWnd->Init();
	//pChatWnd->SetOwner(this);

	//HUD().GetUI()->UIMainIngameWnd->SetMPChatLog(pChatWnd, pGameLog);
	HUD().GetUI()->m_pMessagesWnd->SetChatOwner(this);

	return NULL;
};

bool game_cl_mp::CanBeReady	()
{
	return true;
}

bool	game_cl_mp::NeedToSendReady_Actor			(int key, game_PlayerState* ps)
{
	return ((GAME_PHASE_PENDING == Phase() ) || 
			true == ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) ) && 
			(kWPN_FIRE == key);
}

bool	game_cl_mp::NeedToSendReady_Spectator			(int key, game_PlayerState* ps)
{
	return ( GAME_PHASE_PENDING	== Phase() && kWPN_FIRE == key) || 
			( (/*kWPN_FIRE == key || */kJUMP == key) && GAME_PHASE_INPROGRESS	== Phase() && 
			CanBeReady() && ps->DeathTime > 1000);
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
			b_need_to_send_ready = NeedToSendReady_Actor(key, ps);
		};
		if(is_spectator){
			b_need_to_send_ready =	NeedToSendReady_Spectator(key, ps);

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
		if ((kCHAT == key || kCHAT_TEAM == key) /*&& pChatWnd*/)
		{
			shared_str prefix;

			CUIChatWnd* pChatWnd = HUD().GetUI()->m_pMessagesWnd->GetChatWnd();

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

char	Color_Weapon[]	= "%c<255,255,1,1>";
u32		Color_Teams_u32[3]	= {color_rgba(255,240,190,255), color_rgba(64,255,64,255), color_rgba(64,64,255,255)};
LPSTR	Color_Teams[3]	= {"%c<255,255,240,190>", "%c<255,64,255,64>", "%c<255,64,64,255>"};
char	Color_Main[]	= "%c<255,192,192,192>";
char	Color_Radiation[]	= "%c<255,0,255,255>";
char	Color_Neutral[]	= "%c<255,255,0,255>";
u32		Color_Neutral_u32	= color_rgba(255,0,255,255);

void game_cl_mp::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;

	switch(msg)	{
	
	case GAME_EVENT_PLAYER_KILLED: //dm
		{
			OnPlayerKilled(P);
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
	case GAME_EVENT_PLAYER_NAME:
		{
			OnPlayerChangeName(P);
		}break;
	case GAME_EVENT_PLAYERS_MONEY_CHANGED:
		{
			OnMoneyChanged(P);
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
	P->r_s16();
	string256 PlayerName;
	P->r_stringZ(PlayerName);
	string256 ChatMsg;
	P->r_stringZ(ChatMsg);
	
	Msg("%s : %s", PlayerName, ChatMsg);
	
	if (Level().CurrentViewEntity())
		HUD().GetUI()->m_pMessagesWnd->AddChatMessage(ChatMsg, PlayerName);
};

void game_cl_mp::CommonMessageOut		(LPCSTR msg)
{
#pragma todo("SATAN -> SATAN very very bad solution")
	if (HUD().GetUI())
        HUD().GetUI()->m_pMessagesWnd->AddLogMessage(msg);
};
//////////////////////////////////////////////////////////////////////////
void game_cl_mp::shedule_Update(u32 dt)
{
	UpdateSndMessages();

	inherited::shedule_Update(dt);
	//-----------------------------------------
	switch (Phase())
	{
	case GAME_PHASE_PENDING:
		{
			if (m_bJustRestarted)
			{
				if (Level().CurrentViewEntity())
				{
					PlaySndMessage(ID_READY);
					m_bJustRestarted = false;
				};
			}
		}break;
	default:
		{
		}break;
	}
	UpdateMapLocations();	
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
	inherited::OnSwitchPhase(old_phase, new_phase);
	switch (new_phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			if (HUD().GetUI())
			{
				HUD().GetUI()->ShowIndicators();
			};
		}break;
	case GAME_PHASE_PENDING:
		{
			m_bJustRestarted = true;
		};
	default:
		{
			if (HUD().GetUI())
			{
				HUD().GetUI()->HideIndicators();
			};
		}break;
	}
}

ref_shader game_cl_mp::GetEquipmentIconsShader	()
{
	if (m_EquipmentIconsShader) return m_EquipmentIconsShader;

	m_EquipmentIconsShader.create("hud\\default", EQUIPMENT_ICONS);
	return m_EquipmentIconsShader;
}

ref_shader game_cl_mp::GetKillEventIconsShader	()
{
	return GetEquipmentIconsShader();
	/*
	if (m_KillEventIconsShader) return m_KillEventIconsShader;

	m_KillEventIconsShader.create("hud\\default", KILLEVENT_ICONS);
	return m_KillEventIconsShader;
	*/
}

ref_shader game_cl_mp::GetRadiationIconsShader	()
{
	return GetEquipmentIconsShader();
	/*
	if (m_RadiationIconsShader) return m_RadiationIconsShader;

	m_RadiationIconsShader.create("hud\\default", RADIATION_ICONS);
	return m_RadiationIconsShader;
	*/
}

ref_shader game_cl_mp::GetBloodLossIconsShader	()
{
	return GetEquipmentIconsShader();
	/*
	if (m_BloodLossIconsShader) return m_BloodLossIconsShader;

	m_BloodLossIconsShader.create("hud\\default", BLOODLOSS_ICONS);
	return m_BloodLossIconsShader;
	*/
}
ref_shader		game_cl_mp::GetRankIconsShader()
{
	if (m_RankIconsShader) return m_RankIconsShader;

	m_RankIconsShader.create("hud\\default", RANK_ICONS);
	return m_RankIconsShader;
}

void game_cl_mp::OnPlayerKilled			(NET_Packet& P)
{
	//-----------------------------------------------------------
	KILL_TYPE KillType = KILL_TYPE(P.r_u8());
	u16 KilledID = P.r_u16();
	u16 KillerID = P.r_u16();
	u16	WeaponID = P.r_u16();
	SPECIAL_KILL_TYPE SpecialKill = SPECIAL_KILL_TYPE(P.r_u8());
	//-----------------------------------------------------------
	CObject* pOKiller = Level().Objects.net_Find(KillerID);
	CObject* pWeapon = Level().Objects.net_Find(WeaponID);

	game_PlayerState* pPlayer = GetPlayerByGameID(KilledID);
	game_PlayerState* pKiller = GetPlayerByGameID(KillerID);
	//-----------------------------------------------------------
	string512 PlayerText = "";
	string512 Text = "";
	sprintf(PlayerText, "%s%s ", Color_Teams[pPlayer->team], pPlayer->name);

	KillMessageStruct KMS;
	KMS.m_victim.m_name = pPlayer->name;
	KMS.m_victim.m_color = Color_Teams_u32[pPlayer->team];

	KMS.m_killer.m_name = NULL;
	KMS.m_killer.m_color = color_rgba(255,255,255,255);

	KMS.m_initiator.m_shader = NULL;
	KMS.m_ext_info.m_shader = NULL;

	switch (KillType)
	{
		//-----------------------------------------------------------
	case KT_HIT:			//from hit
		{

			if (pWeapon)
			{
				sprintf (Text, "%skilled from %s%s ",
						Color_Main,
						Color_Weapon,
						*(pWeapon->cName()));
				//-----------------------------------
				CInventoryItem* pIItem = smart_cast<CInventoryItem*>(pWeapon);
				if (pIItem)
				{
					KMS.m_initiator.m_shader = GetEquipmentIconsShader();
					KMS.m_initiator.m_rect.x1 = float(pIItem->GetKillMsgXPos());
					KMS.m_initiator.m_rect.y1 = float(pIItem->GetKillMsgYPos());
					KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + pIItem->GetKillMsgWidth();
					KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + pIItem->GetKillMsgHeight();
				};
			}
			else
			{
				sprintf (Text, "%skilled ",
						Color_Main);
			};

			if (pKiller || pOKiller)
			{
				string512 KillerText;
				
				sprintf (KillerText, "%sby %s%s ",
						Color_Main,
						pKiller ? Color_Teams[pKiller->team] : Color_Neutral,
						pKiller ? pKiller->name : *(pOKiller->cNameSect()));
				std::strcat(Text, KillerText);
				
				if (!pKiller)
				{
					CCustomZone* pAnomaly = smart_cast<CCustomZone*>(pOKiller);
					if (pAnomaly)
					{
						KMS.m_initiator.m_shader = GetKillEventIconsShader();
						KMS.m_initiator.m_rect.x1 = 1;
						KMS.m_initiator.m_rect.y1 = 202;
						KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 31;
						KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 30;
						break;
					}
					/*
					CActor* pActor = smart_cast<CActor*>(pOKiller);
					if (pActor)
					{
						KMS.m_killer.m_name = pKiller ? pKiller->name : *(pActor->cName());
						KMS.m_killer.m_color = pKiller ? Color_Teams_u32[pKiller->team] : Color_Teams_u32[pActor->g_Team()];
					};
					*/
				};

				if (pKiller)
				{
					KMS.m_killer.m_name = pKiller ? pKiller->name : *(pOKiller->cNameSect());
					KMS.m_killer.m_color = pKiller ? Color_Teams_u32[pKiller->team] : Color_Neutral_u32;
				};
			};
			//-------------------------------------------
			string512 SpecialKillText = "";
			switch (SpecialKill)
			{
			case SKT_NONE:		// not special
				{
					if (pOKiller && pOKiller==Level().CurrentViewEntity())
					{
						if (pWeapon && pWeapon->CLS_ID == CLSID_OBJECT_W_KNIFE)
							PlaySndMessage(ID_BUTCHER);
					};
				}break;
			case SKT_HEADSHOT:		// Head Shot
				{
					sprintf(SpecialKillText, "%swith %sHEADSHOT!",
						Color_Main,
						Color_Weapon);					

					KMS.m_ext_info.m_shader = GetKillEventIconsShader();
					KMS.m_ext_info.m_rect.x1 = 62;
					KMS.m_ext_info.m_rect.y1 = 202;
					KMS.m_ext_info.m_rect.x2 = KMS.m_ext_info.m_rect.x1 + 26;
					KMS.m_ext_info.m_rect.y2 = KMS.m_ext_info.m_rect.y1 + 30;

					if (pOKiller && pOKiller==Level().CurrentViewEntity())
						PlaySndMessage(ID_HEADSHOT);
				}break;
			case SKT_BACKSTAB:		// BackStab
				{
					
					sprintf(SpecialKillText, "%swith %sBACKSTAB!",
						Color_Main,
						Color_Weapon);

					KMS.m_initiator.m_shader = GetKillEventIconsShader();
					KMS.m_initiator.m_rect.x1 = 88;
					KMS.m_initiator.m_rect.y1 = 202;
					KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 30;
					KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 30;

					if (pOKiller && pOKiller==Level().CurrentViewEntity())
						PlaySndMessage(ID_ASSASSIN);					
				}break;
			}
			std::strcat(Text, SpecialKillText);

			//suicide
			if (KilledID == KillerID)
			{
				KMS.m_victim.m_name = NULL;
//				KMS.m_victim.m_name = " ";

				KMS.m_ext_info.m_shader = GetKillEventIconsShader();
				KMS.m_ext_info.m_rect.x1 = 32;
				KMS.m_ext_info.m_rect.y1 = 202;
				KMS.m_ext_info.m_rect.x2 = KMS.m_ext_info.m_rect.x1 + 30;
				KMS.m_ext_info.m_rect.y2 = KMS.m_ext_info.m_rect.y1 + 30;
			};

		}break;
		//-----------------------------------------------------------
	case KT_BLEEDING:			//from bleeding
		{
			sprintf(Text, "%shas %sbleed out %s, thanks to %s%s",
				Color_Main,
				Color_Weapon,
				Color_Main,
				pKiller ? Color_Teams[pKiller->team] : Color_Neutral,
				pKiller ? pKiller->name : *(pOKiller->cName()));

			KMS.m_initiator.m_shader = GetBloodLossIconsShader();
			KMS.m_initiator.m_rect.x1 = 238;
			KMS.m_initiator.m_rect.y1 = 31;
			KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 17;
			KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 26;

			if (!pKiller)
			{
				CCustomZone* pAnomaly = smart_cast<CCustomZone*>(pOKiller);
				if (pAnomaly)
				{
					KMS.m_ext_info.m_shader = GetKillEventIconsShader();
						KMS.m_ext_info.m_rect.x1 = 1;
						KMS.m_ext_info.m_rect.y1 = 202;
						KMS.m_ext_info.m_rect.x2 = KMS.m_ext_info.m_rect.x1 + 31;
						KMS.m_ext_info.m_rect.y2 = KMS.m_ext_info.m_rect.y1 + 30;
					break;
				}
			};

			if (pKiller)
			{
				KMS.m_killer.m_name = pKiller ? pKiller->name : *(pOKiller->cNameSect());
				KMS.m_killer.m_color = pKiller ? Color_Teams_u32[pKiller->team] : Color_Neutral_u32;
			};
		}break;
		//-----------------------------------------------------------
	case KT_RADIATION:			//from radiation
		{			
			sprintf(Text, "%shas died from %sradiation",
				Color_Main,
				Color_Radiation);

			KMS.m_initiator.m_shader = GetRadiationIconsShader();
			KMS.m_initiator.m_rect.x1 = 215;
			KMS.m_initiator.m_rect.y1 = 195;
			KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 24;
			KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 24;

		}break;
	default:
		break;
	}
	std::strcat(PlayerText, Text);

	HUD().GetUI()->m_pMessagesWnd->AddLogMessage(KMS);
};

void	game_cl_mp::OnPlayerChangeName		(NET_Packet& P)
{
	u16 ObjID = P.r_u16();
	s16 Team = P.r_s16();
	string1024 OldName, NewName;
	P.r_stringZ(OldName);
	P.r_stringZ(NewName);

	string1024 resStr;
	sprintf(resStr, "%s\"%s\" %sis now %s\"%s\"", Color_Teams[Team], OldName, Color_Main, Color_Teams[Team], NewName);
	CommonMessageOut(resStr);
	//-------------------------------------------
	CObject* pObj = Level().Objects.net_Find(ObjID);
	if (pObj)
		pObj->cName_set(NewName);

}

void	game_cl_mp::LoadSndMessages				()
{
	LoadSndMessage("mp_snd_messages", "headshot", ID_HEADSHOT);
	LoadSndMessage("mp_snd_messages", "butcher", ID_BUTCHER);
	LoadSndMessage("mp_snd_messages", "assassin", ID_ASSASSIN);
	LoadSndMessage("mp_snd_messages", "ready", ID_READY);
};

void	game_cl_mp::OnRankChanged	()
{
	/*
	KillMessageStruct KMS;
	KMS.m_killer.m_name = "You are now";
	sprintf(tmp, "rank_%d",local_player->rank);
	KMS.m_victim.m_name.sprintf("%s", READ_IF_EXISTS(pSettings, r_string, tmp, "rank_name", ""));
//	KMS.m_initiator.m_shader = GetRankIconsShader();
//	KMS.m_initiator.m_rect.x1 = float(local_player->rank*32);
//	KMS.m_initiator.m_rect.y1 = 0;
//	KMS.m_initiator.m_rect.x2 = float((local_player->rank+1)*32);
//	KMS.m_initiator.m_rect.y2 = 32;

	HUD().GetUI()->m_pMessagesWnd->AddLogMessage(KMS);
*/
	string256 tmp;
	string1024 RankStr;
	sprintf(tmp, "rank_%d",local_player->rank);
	sprintf(RankStr, "Your rank now is : %s", READ_IF_EXISTS(pSettings, r_string, tmp, "rank_name", ""));
	CommonMessageOut(RankStr);	
};

void	game_cl_mp::net_import_update		(NET_Packet& P)
{
	u8 OldRank = u8(-1);
	s16 OldTeam = -1;
	if (local_player) 
	{
		OldRank = local_player->rank;
		OldTeam = local_player->team;
	};
	//---------------------------------------------
	inherited::net_import_update(P);
	//---------------------------------------------
	if (local_player)
	{
		if (OldTeam != local_player->team)	OnTeamChanged();
		if (OldRank != local_player->rank)	OnRankChanged();
	};
}

void	game_cl_mp::net_import_state		(NET_Packet& P)
{
	u8 OldRank = u8(-1);
	s16 OldTeam = -1;
	if (local_player) 
	{
		OldRank = local_player->rank;
		OldTeam = local_player->team;
	};

	inherited::net_import_state(P);

	if (local_player)
	{
		if (OldTeam != local_player->team)	OnTeamChanged();
		if (OldRank != local_player->rank)	OnRankChanged();
	};
}

void	game_cl_mp::OnMoneyChanged			(NET_Packet& P)
{
	if (!local_player) return;
	shared_str MoneyStr;
	s32 Money_Added = P.r_s32();
	if (Money_Added != 0)
	{
		MoneyStr.sprintf((Money_Added>0)?"+%d":"%d", Money_Added);
		if (HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
			HUD().GetUI()->UIMainIngameWnd->DisplayMoneyChange(MoneyStr);
	};
	u8 NumBonuses = P.r_u8();
	s32 TotalBonusMoney = 0;
	shared_str BonusStr = (NumBonuses > 1) ? "Your bonuses : " : ((NumBonuses == 1) ? "Your bonus : " : "");
	for (u8 i=0; i<NumBonuses; i++)
	{
		s32 BonusMoney = P.r_s32();
		SPECIAL_KILL_TYPE BonusReason = SPECIAL_KILL_TYPE(P.r_u8());
		u8 BonusKills = (BonusReason == SKT_KIR)? P.r_u8() : 0;
		TotalBonusMoney += BonusMoney;
		//---------------------------------------------------------
		shared_str BName = "";
		switch (BonusReason)
		{
		case SKT_HEADSHOT: BName = "headshot"; break;
		case SKT_BACKSTAB: BName = "backstab"; break;
		case SKT_KNIFEKILL: BName = "knife_kill"; break;
		case SKT_PDA: BName = "pda_taken"; break;
		case SKT_KIR: BName.sprintf("%d_kill_in_row", BonusKills); break;
		};
		shared_str BStr = READ_IF_EXISTS(pSettings, r_string, "mp_bonus_money", BName.c_str(), "");
		if (!BStr.size()) continue;
		string256 tmp;
		_GetItem(BStr.c_str(), 1, tmp);
		if (BonusReason == SKT_KIR) sprintf(tmp, "%s Kill", tmp);
		BonusStr.sprintf("%s%s +%d%s", BonusStr.c_str(), tmp, BonusMoney, (i == NumBonuses-1) ? "" : "; ");		
	};
	if (TotalBonusMoney != 0)
	{
		if (BonusStr.size()) CommonMessageOut(BonusStr.c_str());

		MoneyStr.sprintf("+%d", TotalBonusMoney);
		if (HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
			HUD().GetUI()->UIMainIngameWnd->DisplayMoneyBonus(MoneyStr);
	};	
};