#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "UIGameDM.h"
#include "Spectator.h"
#include "level.h"
#include "xr_level_controller.h"
#include "clsid_game.h"
#include "actor.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIBuyWeaponWnd.h"
#include "ui/UISkinSelector.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapDesc.h"
#include "dinput.h"
#include "gamepersistent.h"

#include "game_cl_deathmatch_snd_messages.h"
#include "game_base_menu_events.h"


#define	TEAM0_MENU		"deathmatch_team0"

game_cl_Deathmatch::game_cl_Deathmatch()
{
	pInventoryMenu	= NULL;
	pBuyMenuTeam0	= NULL;
	pCurBuyMenu		= NULL;
	
	PresetItemsTeam0.clear();
	PlayerDefItems.clear();
	pCurPresetItems	= NULL;;

	pSkinMenuTeam0	= NULL;
	pCurSkinMenu	= NULL;

	pMapDesc		= NULL;

	m_bBuyEnabled	= TRUE;

	m_bSkinSelected	= FALSE;	

	m_game_ui		= NULL;
	
	m_iCurrentPlayersMoney = 0;
//	pChatWnd		= NULL;
	//----------------------------------------------------------------
	pPdaMenu = NULL;

	Actor_Spawn_Effect = "";

	LoadSndMessages();
	//----------------------------------------------------------------
	m_cl_dwWarmUp_Time = 0;
	//----------------------------------------------------------------
	m_bMenuCalledFromReady = FALSE;
	//----------------------------------------------------------------
	m_bFirstRun = TRUE;
}

void game_cl_Deathmatch::Init ()
{
	LoadTeamData(TEAM0_MENU);

	if (pSettings->line_exist("deathmatch_gamedata", "actor_spawn_effect"))
		Actor_Spawn_Effect = pSettings->r_string("deathmatch_gamedata", "actor_spawn_effect");
}

game_cl_Deathmatch::~game_cl_Deathmatch()
{
//	m_aTeamSections.clear();
	xr_delete(pBuyMenuTeam0);
	xr_delete(pSkinMenuTeam0);
	xr_delete(pInventoryMenu);
	//---------------------------------------	
	xr_delete(pPdaMenu);
	//---------------------------------------
	xr_delete(pMapDesc);
	//---------------------------------------
	PresetItemsTeam0.clear();
	PlayerDefItems.clear();
}


CUIGameCustom* game_cl_Deathmatch::createGameUI()
{
	game_cl_mp::createGameUI();
	CLASS_ID clsid			= CLSID_GAME_UI_DEATHMATCH;
	m_game_ui	= smart_cast<CUIGameDM*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(m_game_ui);
	m_game_ui->SetClGame(this);
	m_game_ui->Init();

	//-----------------------------------------------------------
	pBuyMenuTeam0	= InitBuyMenu("deathmatch_base_cost", 0);
	pCurBuyMenu		= pBuyMenuTeam0;
	LoadTeamDefaultPresetItems(TEAM0_MENU, pBuyMenuTeam0, &PresetItemsTeam0);
	//-----------------------------------------------------------
	pSkinMenuTeam0	= InitSkinMenu(0);
	pCurSkinMenu	= pSkinMenuTeam0;

	pInventoryMenu	= xr_new<CUIInventoryWnd>();
	//-----------------------------------------------------------	
	pPdaMenu = xr_new<CUIPdaWnd>();
	//-----------------------------------------------------------
	pMapDesc = xr_new<CUIMapDesc>();
	//-----------------------------------------------------------
	return m_game_ui;
}

void game_cl_Deathmatch::net_import_state	(NET_Packet& P)
{
	inherited::net_import_state	(P);

	P.r_s32			(fraglimit);
	P.r_s32			(timelimit);
//	P.r_u32			(damageblocklimit);
	m_u32ForceRespawn = P.r_u32();
	m_cl_dwWarmUp_Time = P.r_u32();
	m_bDamageBlockIndicators = !!P.r_u8();
	// Teams
	// Teams
	u16				t_count;
	P.r_u16			(t_count);
	teams.clear	();
	for (u16 t_it=0; t_it<t_count; ++t_it)
	{
		game_TeamState	ts;
		P.r				(&ts,sizeof(game_TeamState));
		teams.push_back	(ts);
	};

	switch (Phase())
	{
	case GAME_PHASE_PLAYER_SCORES:
		{
			bool NeedSndMessage = xr_strlen(WinnerName) == 0;
			P.r_stringZ(WinnerName);
			if (NeedSndMessage && !xr_strcmp(WinnerName, local_player->getName()))
			{
				PlaySndMessage(ID_YOU_WON);
			}
		}break;
	}
}

CUIBuyWeaponWnd* game_cl_Deathmatch::InitBuyMenu			(LPCSTR BasePriceSection, s16 Team)
{
	if (Team == -1)
	{
		Team = local_player->team;
	};

	cl_TeamStruct *pTeamSect = &TeamList[ModifyTeam(Team)];
	
	CUIBuyWeaponWnd* pMenu	= xr_new<CUIBuyWeaponWnd>	((LPCSTR)pTeamSect->caSection.c_str(), BasePriceSection);
	pMenu->SetSkin(0);
	return pMenu;
};

//--------------------------------------------------------------------
CUISkinSelectorWnd* game_cl_Deathmatch::InitSkinMenu			(s16 Team)
{
	if (Team == -1)
	{
		Team = local_player->team;
	};

	cl_TeamStruct *pTeamSect = &TeamList[ModifyTeam(Team)];	

	CUISkinSelectorWnd* pMenu	= xr_new<CUISkinSelectorWnd>	((char*)pTeamSect->caSection.c_str());
	
	return pMenu;
};

void game_cl_Deathmatch::OnMapInfoAccept			()
{
	if (CanCallSkinMenu())
		StartStopMenu(pCurSkinMenu, true);
};

void game_cl_Deathmatch::OnSkinMenuBack			()
{
	StartStopMenu(pMapDesc, true);
};

void game_cl_Deathmatch::OnSkinMenu_Ok			()
{
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P, GE_GAME_EVENT, l_pPlayer->ID()	);
//	P.w_u16(GAME_EVENT_PLAYER_CHANGE_SKIN);
	P.w_u16(GAME_EVENT_PLAYER_GAME_MENU);
	P.w_u8(PLAYER_CHANGE_SKIN);


#pragma todo("SATAN -> MAD_MAX: select skin you like if (-1 == pCurSkinMenu->GetActiveIndex()) :)")
	
    P.w_s8	((u8)pCurSkinMenu->GetActiveIndex());
	l_pPlayer->u_EventSend		(P);
	//-----------------------------------------------------------------
	m_bSkinSelected = TRUE;

	// second stub here
	if (pCurBuyMenu) 
	{
		if (-1 == pCurSkinMenu->GetActiveIndex())
			pCurBuyMenu->SetSkin(0);
		else
            pCurBuyMenu->SetSkin((u8)pCurSkinMenu->GetActiveIndex());
	}
	//---------------------------
	/*
	if (m_bMenuCalledFromReady)
	{
		OnKeyboardPress(kJUMP);
	}
	*/
};

void game_cl_Deathmatch::OnSkinMenu_Cancel		()
{
	if (!m_bSkinSelected && !m_bSpectatorSelected)
	{
		if (CanCallSkinMenu() && !pCurSkinMenu->IsShown())
		{
			StartStopMenu(pCurSkinMenu, true);
			return;
		}
	}
	m_bMenuCalledFromReady = FALSE;
};

BOOL game_cl_Deathmatch::CanCallBuyMenu			()
{
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;	
	if (Level().CurrentEntity() && Level().CurrentEntity()->CLS_ID != CLSID_SPECTATOR)
	{
		return FALSE;
	};
	if (!m_bSkinSelected || m_bSpectatorSelected) return FALSE;
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
	{
		return FALSE;
	};
	if (pInventoryMenu && pInventoryMenu->IsShown())
	{
		return FALSE;
	};
	pCurBuyMenu->SetSkin(u8(local_player->skin));
	return m_bBuyEnabled;
};

BOOL game_cl_Deathmatch::CanCallSkinMenu			()
{
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;
	if (pInventoryMenu && pInventoryMenu->IsShown())
	{
		return FALSE;
	};
	if (pCurBuyMenu && pCurBuyMenu->IsShown())
	{
		return FALSE;
	};

	pCurSkinMenu->SetCurSkin(local_player->skin);
	return TRUE;
};

BOOL game_cl_Deathmatch::CanCallInventoryMenu			()
{
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;
	if (Level().CurrentEntity() && Level().CurrentEntity()->CLS_ID != CLSID_OBJECT_ACTOR)
	{
		return FALSE;
	}
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
	{
		return FALSE;
	};
	return TRUE;
};


void game_cl_Deathmatch::SetCurrentBuyMenu	()	
{
	pCurBuyMenu = pBuyMenuTeam0; 
	pCurPresetItems	= &PresetItemsTeam0;

	if (!local_player) return;
};

void game_cl_Deathmatch::ClearBuyMenu			()
{
	if (!pCurBuyMenu) return;
	pCurBuyMenu->ClearSlots();
	pCurBuyMenu->ClearRealRepresentationFlags();
};

bool game_cl_Deathmatch::CanBeReady				()
{
	if (!local_player) return false;

	m_bMenuCalledFromReady = TRUE;

	SetCurrentSkinMenu();

	SetCurrentBuyMenu();
	
	if (pCurBuyMenu && !pCurBuyMenu->IsShown())
		SetBuyMenuItems		();

	if (!m_bSkinSelected)
	{
		if (CanCallSkinMenu())
			StartStopMenu(pCurSkinMenu,true);
		return false;
	};

	u8 res = 0xff;
//	res &=	pCurBuyMenu->GetWeaponIndex(KNIFE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(PISTOL_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(RIFLE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(GRENADE_SLOT);

	if (res == 0xff || !pCurBuyMenu->CanBuyAllItems())
	{
		if (CanCallBuyMenu())
			StartStopMenu(pCurBuyMenu,true);
		return false;
	};

	m_bMenuCalledFromReady = FALSE;
	OnBuyMenu_Ok();
	return true;
};

void	game_cl_Deathmatch::OnSpectatorSelect		()
{
	m_bMenuCalledFromReady = FALSE;
	m_bSkinSelected = FALSE;
	inherited::OnSpectatorSelect();
};


char*	game_cl_Deathmatch::getTeamSection(int Team)
{
	return "deathmatch_team0";
};

void game_cl_Deathmatch::Check_Invincible_Players()
{
};

void game_cl_Deathmatch::ConvertTime2String		(string64* str, u32 Time)
{
	if (!str) return;
	
	u32 RHour = Time / 3600000;
	Time %= 3600000;
	u32 RMinutes = Time / 60000;
	Time %= 60000;
	u32 RSecs = Time / 1000;

	sprintf(*str,"%02d:%02d:%02d", RHour, RMinutes, RSecs);
};

int game_cl_Deathmatch::GetPlayersPlace			(game_PlayerState* ps)
{
	if (!ps) return -1;
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	// create temporary map (sort by kills)
	xr_vector<LPVOID>	Players;
	for (;I!=E;++I)		Players.push_back(I->second);
	std::sort			(Players.begin(),Players.end(),DM_Compare_Players);

	int Place = 1;
	for (u32 i=0; i<Players.size(); i++)
	{
		if (Players[i] == ps)
			return Place;
		Place++;
	};
	return -1;
}

string16 places[] = {
	"1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th",
	"9th", "10th", "11th", "12th", "13th", "15th", "15th", "16th",
	"17th", "18th", "19th", "20th", "21th", "22th", "23th", "24th",
	"25th", "26th", "27th", "28th", "29th", "30th", "31th", "32th"
};

void game_cl_Deathmatch::shedule_Update			(u32 dt)
{
	inherited::shedule_Update(dt);
	//fake	
	if(!m_game_ui && HUD().GetUI() ) m_game_ui = smart_cast<CUIGameDM*>( HUD().GetUI()->UIGame() );
	if(m_game_ui)
	{
		m_game_ui->SetTimeMsgCaption("");
		m_game_ui->SetRoundResultCaption("");
		m_game_ui->SetSpectatorMsgCaption("");
		m_game_ui->SetPressJumpMsgCaption("");
		m_game_ui->SetPressBuyMsgCaption("");
		m_game_ui->SetForceRespawnTimeCaption("");
	};

	if (HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
		HUD().GetUI()->UIMainIngameWnd->GetPDAOnline()->SetText("");

	switch (Phase())
	{
	case GAME_PHASE_INPROGRESS:
		{
			Check_Invincible_Players();

			if (timelimit)
			{
				if (Level().timeServer()<(start_time + timelimit))
				{
					u32 lts = Level().timeServer();
					u32 Rest = (start_time + timelimit) - lts;
					string64 S;
					ConvertTime2String(&S, Rest);
					//SetTimeMsgCaption(S);
					if(m_game_ui)
						m_game_ui->SetTimeMsgCaption(S);
				}
				else
				{
					if(m_game_ui)
						m_game_ui->SetTimeMsgCaption("00:00:00");
				}
			};
			
			if(local_player)
			{
				if (m_bFirstRun)
				{
					m_bFirstRun = FALSE;
					StartStopMenu(pMapDesc, TRUE);
				};

				if (HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
				{
					shared_str MoneyStr;
					MoneyStr.sprintf("%d", local_player->money_for_round);
					HUD().GetUI()->UIMainIngameWnd->ChangeTotalMoneyIndicator(MoneyStr);
				}				

				m_game_ui->SetSpectatorMsgCaption("");
				m_game_ui->SetPressJumpMsgCaption("");
				m_game_ui->SetPressBuyMsgCaption("");
				m_game_ui->SetWarmUpCaption("");

				if (m_cl_dwWarmUp_Time > Level().timeServer())
				{
					u32 TimeRemains = m_cl_dwWarmUp_Time - Level().timeServer();
					string64 S;
					ConvertTime2String(&S, TimeRemains);
					string1024 tmpStr = "";
					if (TimeRemains > 10000)
						strconcat(tmpStr, "Time To Start: ", S);
					else
					{
						if (TimeRemains < 1000)
							strconcat(tmpStr, "GO!", "");
						else
						{
							_itoa(TimeRemains/1000, S, 10);
							strconcat(tmpStr, "Ready ... ", S);
						}
					};
					
					m_game_ui->SetWarmUpCaption(tmpStr);
				}

				if (Level().CurrentEntity() && Level().CurrentEntity()->CLS_ID == CLSID_SPECTATOR)
				{
					if (!(pCurBuyMenu && pCurBuyMenu->IsShown()) && 
						!(pCurSkinMenu && pCurSkinMenu->IsShown()) &&
						!(pMapDesc && pMapDesc->IsShown())
						)
					{
						m_game_ui->SetSpectatorMsgCaption("SPECTATOR : Free-fly camera");
						m_game_ui->SetPressJumpMsgCaption("Press Jump to start");
						m_game_ui->SetPressBuyMsgCaption("Press 'B' to access buy menu");
					};
				};

				u32 CurTime = Level().timeServer();
				if (IsVoteEnabled() && IsVotingActive() && m_dwVoteEndTime>=CurTime)
				{
					u32 TimeLeft = m_dwVoteEndTime - Level().timeServer();
					string1024 VoteTimeResStr;
					u32 SecsLeft = (TimeLeft % 60000) / 1000;
					u32 MinitsLeft = (TimeLeft - SecsLeft) / 60000;

					u32 NumAgreed = 0;
					PLAYERS_MAP_IT I;
					I	= players.begin();
					for(;I!=players.end(); ++I)
					{
						game_PlayerState* ps = I->second;
						if (ps->m_bCurrentVoteAgreed == 1) NumAgreed++;
					}

					sprintf(VoteTimeResStr, "Time Left : %d:%d; Agreed %.2f%", MinitsLeft, SecsLeft, float(NumAgreed)/players.size());
					if (m_game_ui)
						m_game_ui->SetVoteTimeResultMsg(VoteTimeResStr);
				};

				if (local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) && m_u32ForceRespawn)
				{
					if(m_game_ui)
					{
						//					u32 lts = Level().timeServer();
						//					u32 Rest = (start_time + local_player->DeathTime) - lts;
						u32 Rest = m_u32ForceRespawn - local_player->DeathTime;
						string64 S;
						ConvertTime2String(&S, Rest);
						string128 FullS;
						sprintf(FullS, "Time to Respawn : %s", S);

						m_game_ui->SetForceRespawnTimeCaption(FullS);
					};
				};
		//-----------------------------------------------------
				if (fraglimit)
				{
					if (HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
					{
						string64 S; sprintf(S, "%d", fraglimit);
						HUD().GetUI()->UIMainIngameWnd->GetPDAOnline()->SetText(S);
					}
				}

				if (Level().CurrentViewEntity() && HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
				{
					game_PlayerState* ps = GetPlayerByGameID(Level().CurrentViewEntity()->ID());
					if (ps) HUD().GetUI()->UIMainIngameWnd->SetRank(int(ps->rank));
				}
				//-----------------------------------------------------------------------
			};
		}break;
	case GAME_PHASE_PENDING:
		{
			if (m_game_ui)
				m_game_ui->ShowPlayersList(true);
		}break;
	case GAME_PHASE_PLAYER_SCORES:
		{
//			HUD().GetUI()->HideIndicators();
//			GetUICursor()->Hide();
			string128 resstring;
			sprintf(resstring, "Player %s wins the match!", WinnerName);
			m_game_ui->SetRoundResultCaption(resstring);
		}break;
	};
	
	if(m_game_ui)
		m_game_ui->ShowPlayersList	(Phase()==GAME_PHASE_PENDING);

	//-----------------------------------------
	if (pCurBuyMenu && pCurBuyMenu->IsShown() && !CanCallBuyMenu())
		StartStopMenu(pCurBuyMenu, true);
	if (pCurSkinMenu && pCurSkinMenu->IsShown() && !CanCallSkinMenu())
		StartStopMenu(pCurSkinMenu, true);
	if (pInventoryMenu && pInventoryMenu->IsShown() && !CanCallInventoryMenu())
		StartStopMenu(pInventoryMenu,true);
	//-----------------------------------------
}

bool	game_cl_Deathmatch::OnKeyboardPress			(int key)
{
	if (kSCORES == key && Phase() == GAME_PHASE_INPROGRESS)
	{
		if(m_game_ui)
			if (Level().IR_GetKeyState(DIK_LCONTROL))
				m_game_ui->ShowStatistic(true);
			else
				m_game_ui->ShowFragList(true);
		return true;
	};

	if (kINVENTORY == key )
	{
		if (pInventoryMenu->IsShown())
			StartStopMenu(pInventoryMenu,true);
		else
		{
			if (CanCallInventoryMenu())
			{
				StartStopMenu(pInventoryMenu,true);
			};
		};
		return true;
	};

	if (kBUY == key )
	{
		if (pCurBuyMenu && pCurBuyMenu->IsShown())
			StartStopMenu(pCurBuyMenu,true);
		else
		{		
			if(CanCallBuyMenu()){
				SetCurrentBuyMenu	();

				if (pCurBuyMenu && !pCurBuyMenu->IsShown())
					SetBuyMenuItems		();

				LoadDefItemsForRank(pCurBuyMenu);
				StartStopMenu(pCurBuyMenu,true);
			}
		};

		return true;
	};
	
	if (kSKIN == key )
	{
		if (pCurSkinMenu && pCurSkinMenu->IsShown())
			StartStopMenu(pCurSkinMenu,true);
		else
		{
			if (CanCallSkinMenu())
			{
				SetCurrentSkinMenu();
				StartStopMenu(pCurSkinMenu,true);
			}
		}
		return true;
	};
	//---------------------------------------------
	if( kMAP == key)
	{
		if (pPdaMenu && pPdaMenu->IsShown())
			StartStopMenu(pPdaMenu,true);
		else
		{
			pPdaMenu->SetActiveSubdialog(eptMap);
			StartStopMenu(pPdaMenu,true);
		};		
		return true;
	};
	//---------------------------------------------

	return inherited::OnKeyboardPress(key);
}

bool	game_cl_Deathmatch::OnKeyboardRelease		(int key)
{
	if (kSCORES == key )
	{
		if (m_game_ui)
		{
			m_game_ui->ShowFragList(false);
			m_game_ui->ShowStatistic(false);
			
		};
		return true;
	};
	return inherited::OnKeyboardRelease(key);
}

void game_cl_Deathmatch::OnVoteStart				(NET_Packet& P)
{
	inherited::OnVoteStart(P);

	string1024	Command = "";
	string64	Player = "";
	P.r_stringZ(Command);
	P.r_stringZ(Player);
	m_dwVoteEndTime = Level().timeServer() + P.r_u32();
	
	if(m_game_ui)
	{
		string1024 VoteStr;
		sprintf(VoteStr, "Voting \"%s\" has been started by %s.", Command, Player);		

		m_game_ui->SetVoteMessage(VoteStr);
		m_game_ui->SetVoteTimeResultMsg("");
	};
};

void game_cl_Deathmatch::OnVoteStop				(NET_Packet& P)	
{
	inherited::OnVoteStop(P);
	if(m_game_ui)
	{
		m_game_ui->SetVoteMessage("");
		m_game_ui->SetVoteTimeResultMsg("");
	}
};

void game_cl_Deathmatch::OnVoteEnd				(NET_Packet& P)
{
	inherited::OnVoteEnd(P);
	if(m_game_ui)
	{
		m_game_ui->SetVoteMessage("");
		m_game_ui->SetVoteTimeResultMsg("");
	}
};

void game_cl_Deathmatch::GetMapEntities(xr_vector<SZoneMapEntityData>& dst)
{
	/*
	SZoneMapEntityData D;
	u32 color_self_team		=		0xff00ff00;
	D.color					=		color_self_team;

	PLAYERS_MAP_IT it = players.begin();
	for(;it!=players.end();++it)
	{
		game_PlayerState* ps = it->second;
		u16 id = ps->GameID;
		if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
		CObject* pObject = Level().Objects.net_Find(id);
		if (!pObject) continue;
		if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) continue;

		VERIFY(pObject);
		D.pos = pObject->Position();
		dst.push_back(D);
	}
	*/
}

bool		game_cl_Deathmatch::IsEnemy					(game_PlayerState* ps)
{
	return true;
}

bool		game_cl_Deathmatch::IsEnemy					(CEntityAlive* ea1, CEntityAlive* ea2)
{
	return true;
};

void		game_cl_Deathmatch::OnRender				()
{
	if (m_bDamageBlockIndicators && local_player)
	{
		PLAYERS_MAP_IT it = players.begin();
		for(;it!=players.end();++it)
		{
			game_PlayerState* ps = it->second;
			u16 id = ps->GameID;
			if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
			if (!ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE)) continue;
			CObject* pObject = Level().Objects.net_Find(id);
			if (!pObject) continue;
			if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) continue;
			if (ps == local_player) continue;
			if (!IsEnemy(ps)) continue;
			cl_TeamStruct *pTS = &TeamList[ModifyTeam(ps->team)]; 

			VERIFY(pObject);
			CActor* pActor = smart_cast<CActor*>(pObject);
			VERIFY(pActor);
			pActor->RenderIndicator(pTS->IndicatorPos, pTS->Indicator_r1, pTS->Indicator_r2, pTS->InvincibleShader);
		}
	};
}

IC bool	DM_Compare_Players		(LPVOID v1, LPVOID v2)
{
	game_PlayerState* p1 = (game_PlayerState*)v1;
	game_PlayerState* p2 = (game_PlayerState*)v2;
	if (p1->testFlag(GAME_PLAYER_FLAG_SPECTATOR) && !p2->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) return false;
	if (!p1->testFlag(GAME_PLAYER_FLAG_SPECTATOR) && p2->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) return true;	
	if (p1->kills==p2->kills)
	{
		return p1->deaths<p2->deaths;
	}
	return p1->kills>p2->kills;
};

void				game_cl_Deathmatch::PlayParticleEffect		(LPCSTR EffName, Fvector& pos)
{
	if (!EffName) return;
	// ��������� ������� � �������������� ��������
	Fmatrix M; 
	M.translate(pos);

//	CParticlesPlayer::MakeXFORM(pObj,0,Fvector().set(0.f,1.f,0.f),Fvector().set(0.f,0.f,0.f),pos);

	// ���������� particles
	CParticlesObject* ps = NULL;

	ps = CParticlesObject::Create(EffName,TRUE);

	ps->UpdateParent(M,Fvector().set(0.f,0.f,0.f));
	GamePersistent().ps_needtoplay.push_back(ps);
}

void				game_cl_Deathmatch::OnSpawn					(CObject* pObj)
{
	inherited::OnSpawn(pObj);
	if (!pObj) return;
	if (pObj->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		if (xr_strlen(Actor_Spawn_Effect))
			PlayParticleEffect(Actor_Spawn_Effect.c_str(), pObj->Position());
	};
}

void				game_cl_Deathmatch::LoadSndMessages				()
{
	LoadSndMessage("dm_snd_messages", "you_won", ID_YOU_WON);
};

void				game_cl_Deathmatch::OnSwitchPhase_InProgress()
{
	LoadTeamDefaultPresetItems(TEAM0_MENU, pBuyMenuTeam0, &PresetItemsTeam0);
};

void				game_cl_Deathmatch::OnSwitchPhase			(u32 old_phase, u32 new_phase)
{
	inherited::OnSwitchPhase(old_phase, new_phase);
	switch (new_phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			WinnerName[0] = 0;
		}break;
	case GAME_PHASE_PLAYER_SCORES:
		{
			if (!xr_strcmp(WinnerName, local_player->getName()))
			{
				PlaySndMessage(ID_YOU_WON);
			}
		}break;
	default:
		{			
		}break;
	};
}

void				game_cl_Deathmatch::OnRankChanged			()
{
	inherited::OnRankChanged();
	if (pCurBuyMenu) pCurBuyMenu->SetRank(local_player->rank);
	LoadDefItemsForRank(pCurBuyMenu);
	ChangeItemsCosts(pCurBuyMenu);
};

void				game_cl_Deathmatch::OnTeamChanged			()
{
	if (!pCurBuyMenu) return;
	if (pCurBuyMenu) pCurBuyMenu->SetRank(local_player->rank);
	LoadDefItemsForRank(pCurBuyMenu);
	ChangeItemsCosts(pCurBuyMenu);
};

void				game_cl_Deathmatch::LoadPlayerDefItems			(char* TeamName, CUIBuyWeaponWnd* pBuyMenu)
{
	if (!local_player) return;
	LoadTeamDefaultPresetItems(TeamName, pBuyMenu, &PlayerDefItems);
};

void				game_cl_Deathmatch::OnGameMenuRespond_ChangeSkin	(NET_Packet& P)
{
	s8 NewSkin = P.r_s8();
	local_player->skin = NewSkin;
	
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
		StartStopMenu(pCurSkinMenu, true);
	if (pMapDesc && pMapDesc->IsShown())
		StartStopMenu(pMapDesc, TRUE);

	SetCurrentSkinMenu();
	if (pCurSkinMenu) pCurSkinMenu->SetCurSkin(local_player->skin);
	SetCurrentBuyMenu();
	if (pCurBuyMenu) pCurBuyMenu->SetSkin(local_player->skin);	
	m_bSpectatorSelected = FALSE;
	
	if (m_bMenuCalledFromReady)
	{
		OnKeyboardPress(kJUMP);
	}
};