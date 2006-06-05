#include "stdafx.h"
#include "game_cl_teamdeathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"
#include "UIGameTDM.h"
#include "xr_level_controller.h"
#include "clsid_game.h"
#include "map_manager.h"
#include "map_location.h"
#include "actor.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UISkinSelector.h"
#include "ui/UIPDAWnd.h"
#include "ui/UIMapDesc.h"
#include "game_base_menu_events.h"
#include "ui/TeamInfo.h"

#define	TEAM0_MENU		"teamdeathmatch_team0"
#define	TEAM1_MENU		"teamdeathmatch_team1"
#define	TEAM2_MENU		"teamdeathmatch_team2"

#define MESSAGE_MENUS	"tdm_messages_menu"

#include "game_cl_teamdeathmatch_snd_messages.h"

game_cl_TeamDeathmatch::game_cl_TeamDeathmatch()
{
	pUITeamSelectWnd	= xr_new<CUISpawnWnd>	();
	pBuyMenuTeam1		= NULL;
	pBuyMenuTeam2		= NULL;

	PresetItemsTeam1.clear();
	PresetItemsTeam2.clear();

	pSkinMenuTeam1		= NULL;
	pSkinMenuTeam2		= NULL;

	m_bTeamSelected		= FALSE;
	m_game_ui			= NULL;

	m_bShowPlayersNames = false;
	m_bFriendlyIndicators = false;
	m_bFriendlyNames	= false;

	LoadSndMessages();
}
void game_cl_TeamDeathmatch::Init ()
{
	//-----------------------------------------------------------
//	string64	Team1, Team2;
//	std::strcpy(Team1, TEAM1_MENU);
//	std::strcpy(Team2, TEAM2_MENU);
//	m_aTeamSections.push_back(Team1);
//	m_aTeamSections.push_back(Team2);
//	LoadTeamData(TEAM0_MENU);
	LoadTeamData(TEAM1_MENU);
	LoadTeamData(TEAM2_MENU);
}

game_cl_TeamDeathmatch::~game_cl_TeamDeathmatch()
{
	xr_delete(pUITeamSelectWnd);

	xr_delete(pBuyMenuTeam1);
	xr_delete(pBuyMenuTeam2);

	xr_delete(pSkinMenuTeam1);
	xr_delete(pSkinMenuTeam2);
}

void				game_cl_TeamDeathmatch::net_import_state		(NET_Packet& P)
{
	bool teamsEqual = (!teams.empty())?(teams[0].score == teams[1].score) : false;
	inherited::net_import_state	(P);
	m_bFriendlyIndicators = !!P.r_u8();
	m_bFriendlyNames = !!P.r_u8();
	if (!teams.empty())
	{	
		if (teamsEqual)
		{
			if (teams[0].score != teams[1].score)
			{
				if (Level().CurrentViewEntity())
				{
					if (teams[0].score > teams[1].score)
						PlaySndMessage(ID_TEAM1_LEAD);
					else
						PlaySndMessage(ID_TEAM2_LEAD);
				}
			}
		}
		else
		{
			if (teams[0].score == teams[1].score)
				if (Level().CurrentViewEntity())
					PlaySndMessage(ID_TEAMS_EQUAL);
		}
	};
}
void game_cl_TeamDeathmatch::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
//	LPSTR	Color_Teams[3]	= {"%c<255,255,255,255>", "%c<255,64,255,64>", "%c<255,64,64,255>"};
	char	Color_Main[]	= "%c<255,192,192,192>";
//	LPSTR	TeamsNames[3]	= {"Zero Team", "Team Green", "Team Blue"};

	switch(msg)	{
	case GAME_EVENT_PLAYER_JOIN_TEAM: //tdm
		{
			string64 PlayerName;
			P.r_stringZ	(PlayerName);
			u16 Team;
			P.r_u16		(Team);

			sprintf(Text, "%s%s %sjoined %s%s",
							"",//no color
							PlayerName,
							Color_Main,
							CTeamInfo::GetTeam_color_tag(int(Team)),
							CTeamInfo::GetTeam_name(int(Team)));
			CommonMessageOut(Text);
			//---------------------------------------
			Msg("%s joined %s", PlayerName, 
				CTeamInfo::GetTeam_name(int(Team)));	 // caution!!! : using of shared_str - don't forget about (*)
		}break;

	case PLAYER_CHANGE_TEAM://tdm
		{
			u16 PlayerID, OldTeam, NewTeam;
			P.r_u16 (PlayerID);
			P.r_u16 (OldTeam);
			P.r_u16 (NewTeam);

			game_PlayerState* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %shas switched to %s%s", 
							CTeamInfo::GetTeam_color_tag(int(OldTeam)), 
							pPlayer->name, 
							Color_Main, 
							CTeamInfo::GetTeam_color_tag(int(NewTeam)), 
							CTeamInfo::GetTeam_name(int(NewTeam)));
			CommonMessageOut(Text);
			//---------------------------------------
			Msg("%s has switched to %s", pPlayer->name, CTeamInfo::GetTeam_name(int(NewTeam)));
		}break;

	default:
		inherited::TranslateGameMessage(msg,P);
	};
}

CUIGameCustom* game_cl_TeamDeathmatch::createGameUI()
{
	game_cl_mp::createGameUI();
	CLASS_ID clsid			= CLSID_GAME_UI_TEAMDEATHMATCH;
	m_game_ui	= smart_cast<CUIGameTDM*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(m_game_ui);
	m_game_ui->SetClGame(this);
	m_game_ui->Init();


	//-----------------------------------------------------------
	pBuyMenuTeam1 = InitBuyMenu("teamdeathmatch_base_cost", 1);
	LoadTeamDefaultPresetItems(TEAM1_MENU, pBuyMenuTeam1, &PresetItemsTeam1);
	pBuyMenuTeam2 = InitBuyMenu("teamdeathmatch_base_cost", 2);
	LoadTeamDefaultPresetItems(TEAM2_MENU, pBuyMenuTeam2, &PresetItemsTeam2);
	//-----------------------------------------------------------
	pSkinMenuTeam1 = InitSkinMenu(1);
	pSkinMenuTeam2 = InitSkinMenu(2);

	pInventoryMenu = xr_new<CUIInventoryWnd>();
	//-----------------------------------------------------------	
	pPdaMenu = xr_new<CUIPdaWnd>();
	//-----------------------------------------------------------
	pMapDesc = xr_new<CUIMapDesc>();
	pMapDesc->SetWorkPhase(GAME_PHASE_INPROGRESS);
	//-----------------------------------------------------------
	LoadMessagesMenu(MESSAGE_MENUS);
	//-----------------------------------------------------------
	return m_game_ui;
}


void game_cl_TeamDeathmatch::GetMapEntities(xr_vector<SZoneMapEntityData>& dst)
{
	SZoneMapEntityData D;
	u32 color_self_team		=		0xff00ff00;
	D.color					=		color_self_team;

	s16 local_team			=		local_player->team;

	PLAYERS_MAP_IT it = players.begin();
	for(;it!=players.end();++it){
		if(local_team == it->second->team){
			u16 id = it->second->GameID;
			CObject* pObject = Level().Objects.net_Find(id);
			if (!pObject) continue;
			if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) continue;

			VERIFY(pObject);
			D.pos = pObject->Position();
			dst.push_back(D);
		}
	}
}

void game_cl_TeamDeathmatch::OnMapInfoAccept			()
{
	if (CanCallTeamSelectMenu())
		StartStopMenu(pUITeamSelectWnd, true);
};

void game_cl_TeamDeathmatch::OnTeamMenuBack			()
{
	if (local_player->testFlag(GAME_PLAYER_FLAG_SPECTATOR))
        StartStopMenu(pMapDesc, true);
};

void game_cl_TeamDeathmatch::OnTeamMenu_Cancel		()
{
	StartStopMenu(pUITeamSelectWnd, true);
	if (!m_bTeamSelected && !m_bSpectatorSelected)
	{
		if (CanCallTeamSelectMenu() && !pUITeamSelectWnd->IsShown())
		{
			StartStopMenu(pUITeamSelectWnd, true);
			return;
		}
	}
	m_bMenuCalledFromReady = FALSE;
};

void game_cl_TeamDeathmatch::OnSkinMenuBack			()
{
	if (CanCallTeamSelectMenu())
		StartStopMenu(pUITeamSelectWnd, true);
};

void game_cl_TeamDeathmatch::OnSpectatorSelect		()
{
	m_bTeamSelected = FALSE;
	inherited::OnSpectatorSelect();
}

void game_cl_TeamDeathmatch::OnTeamSelect(int Team)
{
	bool NeedToSendTeamSelect = true;
	if (Team != -1)
	{
		if (Team+1 == local_player->team && m_bSkinSelected)
			NeedToSendTeamSelect = false;
		else
		{
				NeedToSendTeamSelect = true;				
		}
	}

	if (NeedToSendTeamSelect)
	{
		CObject *l_pObj = Level().CurrentEntity();

		CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
		if(!l_pPlayer) return;

		NET_Packet		P;
		l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
//		P.w_u16(GAME_EVENT_PLAYER_CHANGE_TEAM);
		P.w_u16(GAME_EVENT_PLAYER_GAME_MENU);
		P.w_u8(PLAYER_CHANGE_TEAM);
		
		P.w_s16			(s16(Team+1));
		//P.w_u32			(0);
		l_pPlayer->u_EventSend		(P);
		//-----------------------------------------------------------------
		m_bSkinSelected = FALSE;
	};
	//-----------------------------------------------------------------
	m_bTeamSelected = TRUE;	
	//---------------------------
//	if (m_bMenuCalledFromReady)
//	{
//		OnKeyboardPress(kJUMP);
//	}
};
//-----------------------------------------------------------------
void game_cl_TeamDeathmatch::SetCurrentBuyMenu	()
{
	if (!local_player) return;
	if (!local_player->team || local_player->skin == -1) return;
	if (!pCurBuyMenu || !pCurBuyMenu->IsShown())
	{
		if (local_player->team == 1) 
		{
			pCurBuyMenu = pBuyMenuTeam1;
			pCurPresetItems = &PresetItemsTeam1;
		}
		else 
		{
			pCurBuyMenu = pBuyMenuTeam2;
			pCurPresetItems = &PresetItemsTeam2;
		};
	};

//	if(!pCurSkinMenu)SetCurrentSkinMenu	();
	if (pCurBuyMenu) pCurBuyMenu->SetSkin(local_player->skin);
	if (!pCurBuyMenu) return;	

	//-----------------------------------
	if (m_cl_dwWarmUp_Time != 0) pCurBuyMenu->IgnoreMoneyAndRank(true);
	else pCurBuyMenu->IgnoreMoneyAndRank(false);
	//-----------------------------------
};

void game_cl_TeamDeathmatch::SetCurrentSkinMenu	()
{
	CUISkinSelectorWnd* pNewSkinMenu;
	if (!local_player) return;
	if (local_player->team == 1) pNewSkinMenu = pSkinMenuTeam1;
	else pNewSkinMenu = pSkinMenuTeam2;

	if (pNewSkinMenu != pCurSkinMenu)
		if (pCurSkinMenu && pCurSkinMenu->IsShown()) StartStopMenu(pCurSkinMenu,true);

	pCurSkinMenu = pNewSkinMenu;
	//pCurSkinMenu->SwitchSkin(local_player->skin);
};

bool game_cl_TeamDeathmatch::CanBeReady				()
{
	if (!local_player) return false;
	
	m_bMenuCalledFromReady = TRUE;

	if (!m_bTeamSelected)
	{
		if (CanCallTeamSelectMenu())
			StartStopMenu(pUITeamSelectWnd,true);
		return false;
	}

	return inherited::CanBeReady();
};

char*	game_cl_TeamDeathmatch::getTeamSection(int Team)
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
	default:
		return NULL;
	};
};

void game_cl_TeamDeathmatch::shedule_Update			(u32 dt)
{
	inherited::shedule_Update(dt);

	if(!m_game_ui && HUD().GetUI() ) m_game_ui = smart_cast<CUIGameTDM*>( HUD().GetUI()->UIGame() );
	//---------------------------------------------------------
	if (pUITeamSelectWnd && pUITeamSelectWnd->IsShown() && !CanCallTeamSelectMenu())
		StartStopMenu(pUITeamSelectWnd,true);
	//---------------------------------------------------------
	
	switch (phase)
	{
	case GAME_PHASE_TEAM1_SCORES:
		{
			m_game_ui->SetRoundResultCaption("Team Green WINS!");
		}break;
	case GAME_PHASE_TEAM2_SCORES:
		{
			m_game_ui->SetRoundResultCaption("Team Blue WINS!");
		}break;
	case GAME_PHASE_INPROGRESS:
		{
			if (local_player)
			{			
				s16 lt = local_player->team;
				if (lt>=0)
				{
					if(m_game_ui)
						m_game_ui->SetScoreCaption	(teams[0].score, teams[1].score);

//					if (HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
//					{
//						HUD().GetUI()->UIMainIngameWnd->UpdateTeamsScore(teams[0].score, teams[1].score);
//					}
				};

				if (m_game_ui)
				{
					if (pUITeamSelectWnd->IsShown())
					{
//						m_game_ui->SetSpectatorMsgCaption("");
					}
				}
				//---------------------------------------------------------
/*				if (local_player->team == 0)
				{
					if (!m_bTeamSelected && !m_bSpectatorSelected)
					{
						if (CanCallTeamSelectMenu() && !pUITeamSelectWnd->IsShown())
							StartStopMenu(pUITeamSelectWnd, true);
					}
				}
*/
			};
		}break;
	default:
		{
		}break;
	};
}

//BOOL	g_bShowPlayerNames = FALSE;

bool	game_cl_TeamDeathmatch::OnKeyboardPress			(int key)
{
	if (kTEAM == key )
	{
		if (CanCallTeamSelectMenu())
		{
			StartStopMenu(pUITeamSelectWnd,true);
		};

		return true;
	};
	
	return inherited::OnKeyboardPress(key);
}

bool		game_cl_TeamDeathmatch::IsEnemy					(game_PlayerState* ps)
{
	if (!local_player) return false;
	return local_player->team != ps->team;
};

bool		game_cl_TeamDeathmatch::IsEnemy					(CEntityAlive* ea1, CEntityAlive* ea2)
{
	return (ea1->g_Team() != ea2->g_Team());
};

#define PLAYER_NAME_COLOR 0xff40ff40

void	game_cl_TeamDeathmatch::OnRender				()
{
	if (local_player)
	{
		cl_TeamStruct *pTS = &TeamList[ModifyTeam(local_player->team)]; 
		PLAYERS_MAP_IT it = players.begin();
		for(;it!=players.end();++it)
		{
			game_PlayerState* ps = it->second;
			u16 id = ps->GameID;
			if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
			CObject* pObject = Level().Objects.net_Find(id);
			if (!pObject) continue;
			if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) continue;
			if (IsEnemy(ps)) continue;
			if (ps == local_player) continue;

			float dup = 0.0f;
			if (/*m_bFriendlyNames && */m_bShowPlayersNames)
			{
				VERIFY(pObject);
				CActor* pActor = smart_cast<CActor*>(pObject);
				VERIFY(pActor); 
				Fvector IPos = pTS->IndicatorPos;
				IPos.y -= pTS->Indicator_r2;
				pActor->RenderText(ps->getName(), IPos, &dup, PLAYER_NAME_COLOR);
			}
			if (m_bFriendlyIndicators)
			{
				VERIFY(pObject);
				CActor* pActor = smart_cast<CActor*>(pObject);
				VERIFY(pActor);
				Fvector IPos = pTS->IndicatorPos;
				IPos.y += dup;
				pActor->RenderIndicator(IPos, pTS->Indicator_r1, pTS->Indicator_r2, pTS->IndicatorShader);
			};
		}
	};
	inherited::OnRender();
}

BOOL game_cl_TeamDeathmatch::CanCallBuyMenu			()
{
	if (pUITeamSelectWnd && pUITeamSelectWnd->IsShown())
		return FALSE;
	if (!m_bTeamSelected) return FALSE;
	if (!m_bSkinSelected) return FALSE;

	return inherited::CanCallBuyMenu();
};

BOOL game_cl_TeamDeathmatch::CanCallSkinMenu			()
{
	if (pUITeamSelectWnd && pUITeamSelectWnd->IsShown())
		return FALSE;
	if (!m_bTeamSelected) return FALSE;

	return inherited::CanCallSkinMenu();
};

BOOL game_cl_TeamDeathmatch::CanCallInventoryMenu			()
{
	if (pUITeamSelectWnd && pUITeamSelectWnd->IsShown())
		return FALSE;

	return inherited::CanCallInventoryMenu();	
};

BOOL game_cl_TeamDeathmatch::CanCallTeamSelectMenu			()
{
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;
	if (!local_player) return false;
	if (pInventoryMenu && pInventoryMenu->IsShown())
	{
		return FALSE;
	};
	if (pCurBuyMenu && pCurBuyMenu->IsShown())
	{
		return FALSE;
	};
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
	{
		return FALSE;
	};

	pUITeamSelectWnd->SetCurTeam(ModifyTeam(local_player->team));
	return TRUE;	
};

#define FRIEND_LOCATION	"mp_friend_location"
#define SELF_LOCATION	"mp_self_location"

void game_cl_TeamDeathmatch::UpdateMapLocations		()
{
	inherited::UpdateMapLocations();
	if (local_player)
	{
		PLAYERS_MAP_IT it = players.begin();
		for(;it!=players.end();++it)
		{
			game_PlayerState* ps = it->second;			
			u16 id = ps->GameID;
			if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) 
			{
				Level().MapManager().RemoveMapLocation(FRIEND_LOCATION, id);
				continue;
			};
			if (ps == local_player) 
			{
				if (!Level().MapManager().HasMapLocation(SELF_LOCATION, id))
				{
					(Level().MapManager().AddMapLocation(SELF_LOCATION, id))->EnablePointer();
				}
				continue;
			};
			CObject* pObject = Level().Objects.net_Find(id);
			if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) continue;
			if (IsEnemy(ps)) 
			{
				if (Level().MapManager().HasMapLocation(FRIEND_LOCATION, id))
				{
					Level().MapManager().RemoveMapLocation(FRIEND_LOCATION, id);
				};
				continue;
			};
			if (!Level().MapManager().HasMapLocation(FRIEND_LOCATION, id))
			{
				(Level().MapManager().AddMapLocation(FRIEND_LOCATION, id))->EnablePointer();
			}
		}
	};
};

void				game_cl_TeamDeathmatch::LoadSndMessages				()
{
//	LoadSndMessage("dm_snd_messages", "you_won", ID_YOU_WON);
	LoadSndMessage("tdm_snd_messages", "team1_win", ID_TEAM1_WIN);
	LoadSndMessage("tdm_snd_messages", "team2_win", ID_TEAM2_WIN);
	LoadSndMessage("tdm_snd_messages", "teams_equal", ID_TEAMS_EQUAL);
	LoadSndMessage("tdm_snd_messages", "team1_lead", ID_TEAM1_LEAD);
	LoadSndMessage("tdm_snd_messages", "team2_lead", ID_TEAM2_LEAD);
};

void				game_cl_TeamDeathmatch::OnSwitchPhase_InProgress()
{
	LoadTeamDefaultPresetItems(TEAM1_MENU, pBuyMenuTeam1, &PresetItemsTeam1);
	LoadTeamDefaultPresetItems(TEAM2_MENU, pBuyMenuTeam2, &PresetItemsTeam2);
	if (!m_bSkinSelected) m_bTeamSelected = FALSE;
};

void				game_cl_TeamDeathmatch::OnSwitchPhase			(u32 old_phase, u32 new_phase)
{
	inherited::OnSwitchPhase(old_phase, new_phase);
	switch (new_phase)
	{
	case GAME_PHASE_TEAM1_SCORES:
		{
			if (Level().CurrentViewEntity())
				PlaySndMessage(ID_TEAM1_WIN);
		}break;
	case GAME_PHASE_TEAM2_SCORES:
		{
			if (Level().CurrentViewEntity())
				PlaySndMessage(ID_TEAM2_WIN);
		}break;
	default:
		{			
		}break;
	};
}

void				game_cl_TeamDeathmatch::OnTeamChanged			()
{
	SetCurrentBuyMenu();
	inherited::OnTeamChanged();
};

void				game_cl_TeamDeathmatch::OnGameMenuRespond_ChangeTeam	(NET_Packet& P)
{
	s16 NewTeam = P.r_s16();
	local_player->team = NewTeam;

	SetCurrentSkinMenu();
	if (pCurSkinMenu)
	{
		pCurSkinMenu->SetCurSkin(local_player->skin);
		if (CanCallSkinMenu())
			StartStopMenu(pCurSkinMenu, true);
	}
};