#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "UIGameDM.h"
#include "Spectator.h"
#include "level.h"
#include "xr_level_controller.h"

#define	TEAM0_MENU		"deathmatch_team0"

game_cl_Deathmatch::game_cl_Deathmatch()
{
	pInventoryMenu	= NULL;
	pBuyMenuTeam0	= NULL;
	pCurBuyMenu		= NULL;
	
	PresetItemsTeam0.clear();
	pCurPresetItems	= NULL;;

	pSkinMenuTeam0	= NULL;
	pCurSkinMenu	= NULL;

	m_bBuyEnabled	= TRUE;

	m_bSkinSelected	= FALSE;

	m_bSkinSelected	= FALSE;

	m_game_ui		= NULL;
	
	m_iCurrentPlayersMoney = 0;
}

void game_cl_Deathmatch::Init ()
{
	//-----------------------------------------------------------
	string64	Team0;
	std::strcpy(Team0, TEAM0_MENU);
	m_aTeamSections.push_back(Team0);

}

game_cl_Deathmatch::~game_cl_Deathmatch()
{
	m_aTeamSections.clear();
	xr_delete(pBuyMenuTeam0);
	xr_delete(pSkinMenuTeam0);
	xr_delete(pInventoryMenu);
}


CUIGameCustom* game_cl_Deathmatch::createGameUI()
{
	CLASS_ID clsid			= CLSID_GAME_UI_DEATHMATCH;
	m_game_ui	= smart_cast<CUIGameDM*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(m_game_ui);
	m_game_ui->SetClGame(this);
	m_game_ui->Init();

	//-----------------------------------------------------------
	pBuyMenuTeam0	= InitBuyMenu("deathmatch_base_cost", 0);
	pCurBuyMenu		= pBuyMenuTeam0;
	//-----------------------------------------------------------
	pSkinMenuTeam0	= InitSkinMenu(0);
	pCurSkinMenu	= pSkinMenuTeam0;

	pInventoryMenu = xr_new<CUIInventoryWnd>();


	return m_game_ui;
}

void game_cl_Deathmatch::net_import_state	(NET_Packet& P)
{
	inherited::net_import_state	(P);

	P.r_s32			(fraglimit);
	P.r_s32			(timelimit);
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
}

CUIBuyWeaponWnd* game_cl_Deathmatch::InitBuyMenu			(LPCSTR BasePriceSection, s16 Team)
{
	if (Team == -1)
	{
		Team = local_player->team;
	};

	std::string *pTeamSect = &m_aTeamSections[ModifyTeam(Team)];

	
	CUIBuyWeaponWnd* pMenu	= xr_new<CUIBuyWeaponWnd>	((LPCSTR)pTeamSect->c_str(), BasePriceSection);
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

	std::string *pTeamSect = &m_aTeamSections[ModifyTeam(Team)];

	CUISkinSelectorWnd* pMenu	= xr_new<CUISkinSelectorWnd>	((char*)pTeamSect->c_str());
	
	return pMenu;
};

void game_cl_Deathmatch::OnSkinMenu_Ok			()
{
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P, GE_GAME_EVENT, l_pPlayer->ID()	);
	P.w_u16(GAME_EVENT_PLAYER_CHANGE_SKIN);

	P.w_u8			(pCurSkinMenu->GetActiveIndex());
	l_pPlayer->u_EventSend		(P);
	//-----------------------------------------------------------------
	m_bSkinSelected = TRUE;

	if (pCurBuyMenu) 
		pCurBuyMenu->SetSkin(pCurSkinMenu->GetActiveIndex());
};

BOOL game_cl_Deathmatch::CanCallBuyMenu			()
{
	CSpectator* pCurPlayer = smart_cast<CSpectator*> (Level().CurrentEntity());
	if (!pCurPlayer) return FALSE;

	return m_bBuyEnabled;
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

	SetCurrentSkinMenu();

	SetCurrentBuyMenu();
	
	if (pCurBuyMenu && !pCurBuyMenu->IsShown())
		SetBuyMenuItems		();

	if (!m_bSkinSelected)
	{
		StartStopMenu(pCurSkinMenu,true);
		return false;
	};

	u8 res = 0xff;
	res &=	pCurBuyMenu->GetWeaponIndex(KNIFE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(PISTOL_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(RIFLE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(GRENADE_SLOT);

	if (res == 0xff || !pCurBuyMenu->CanBuyAllItems())
	{
		StartStopMenu(pCurBuyMenu,true);
		return false;
	};

	OnBuyMenu_Ok();
	return true;
};

char*	game_cl_Deathmatch::getTeamSection(int Team)
{
	return "deathmatch_team0";
};

void game_cl_Deathmatch::shedule_Update			(u32 dt)
{
	//fake
	if(!m_game_ui && HUD().GetUI() ) m_game_ui = smart_cast<CUIGameDM*>( HUD().GetUI()->UIGame() );
	if (timelimit && Phase()==GAME_PHASE_INPROGRESS)
	{
		if (Level().timeServer()<(start_time + timelimit))
		{
			u32 lts = Level().timeServer();
			u32 Rest = (start_time + timelimit) - lts;

			u32 RHour = Rest / 3600000;
			Rest %= 3600000;
			u32 RMinutes = Rest / 60000;
			Rest %= 60000;
			u32 RSecs = Rest / 1000;
			string64 S;
			sprintf(S,"%02d:%02d:%02d", RHour, RMinutes, RSecs);
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

	if(m_game_ui)
		m_game_ui->ShowPlayersList	(Phase()==GAME_PHASE_PENDING);

	if(Phase()==GAME_PHASE_INPROGRESS && local_player)
	{
			game_PlayerState * P = local_player;

			string16	tmp;
			_itoa(P->money_for_round, tmp, 10);
			ref_str PMoney(tmp);
			HUD().GetUI()->UIMainIngameWnd.ChangeTotalMoneyIndicator(PMoney);

			if (P->money_for_round != m_iCurrentPlayersMoney)
			{
				s16 dMoney = P->money_for_round - m_iCurrentPlayersMoney;
				if (dMoney > 0)
					sprintf(tmp,"+%d", dMoney);
				else
					sprintf(tmp,"%d", dMoney);				
				
				PMoney._set(tmp);

				HUD().GetUI()->UIMainIngameWnd.DisplayMoneyChange(PMoney);

				m_iCurrentPlayersMoney = P->money_for_round;
			};
			m_game_ui->SetSpectatorMsgCaption("");
			m_game_ui->SetPressJumpMsgCaption("");
			m_game_ui->SetPressBuyMsgCaption("");

			if (Level().CurrentEntity() && Level().CurrentEntity()->SUB_CLS_ID == CLSID_SPECTATOR)
			{
				if (!pCurBuyMenu || !pCurBuyMenu->IsShown())
				{
					m_game_ui->SetSpectatorMsgCaption("SPECTATOR : Free-fly camera");
					m_game_ui->SetPressJumpMsgCaption("Press Jump to start");
					m_game_ui->SetPressBuyMsgCaption("Press 'B' to access buy menu");
					/*
					HUD().pFontDI->SetAligment		(CGameFont::alCenter);

					HUD().pFontDI->SetColor		(0xffffffff);
					HUD().pFontDI->Out			(0.f,0.0f,"SPECTATOR : Free-fly camera");

					float OldSize = HUD().pFontDI->GetSize			();

					HUD().pFontDI->SetSize		(0.02f);
					HUD().pFontDI->SetColor		(0xffffffff);
					HUD().pFontDI->Out			(0.f,0.9f,"Press Jump to start");
					HUD().pFontDI->SetSize		(OldSize);
					*/
				};
			};

	};
	if (Phase()==GAME_PHASE_PENDING)
	{
		m_game_ui->ShowPlayersList(true);
	}


	inherited::shedule_Update(dt);
}

bool	game_cl_Deathmatch::OnKeyboardPress			(int key)
{
	if (kSCORES == key )
	{
				m_game_ui->ShowFragList(true);
		return true;
	};

	if (kINVENTORY == key )
	{
		StartStopMenu(pInventoryMenu,true);
		return true;
	};

	if (kBUY == key )
	{
		if(CanCallBuyMenu()){
			SetCurrentBuyMenu	();

			if (pCurBuyMenu && !pCurBuyMenu->IsShown())
				SetBuyMenuItems		();
			StartStopMenu(pCurBuyMenu,true);
		}

		return true;
	};
	
	if (kSKIN == key )
	{
		if(!pCurSkinMenu) SetCurrentSkinMenu();
		StartStopMenu(pCurSkinMenu,true);

		return true;
	};

	
	return inherited::OnKeyboardPress(key);
}

bool	game_cl_Deathmatch::OnKeyboardRelease		(int key)
{
	if (kSCORES == key )
	{
		m_game_ui->ShowFragList(false);
//		m_game_ui->ShowPlayersList(false);
		return true;
	};
	return inherited::OnKeyboardRelease(key);
}

