#include "stdafx.h"
#include "UIGameTDM.h"

#include "UITDMPlayerList.h"
#include "UITDMFragList.h"

#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"

#include "game_cl_TeamDeathmatch.h"

#define MSGS_OFFS 510

// moved to game_cl_teamdeathmatch
// #define	TEAM1_MENU		"teamdeathmatch_team1"
// #define	TEAM2_MENU		"teamdeathmatch_team2"

//--------------------------------------------------------------------
CUIGameTDM::CUIGameTDM()
{
	m_game				= NULL;
// moved to game_cl_teamdeathmatch
//	pUITeamSelectWnd	= xr_new<CUISpawnWnd>	();
//	pBuyMenuTeam1		= NULL;
//	pBuyMenuTeam2		= NULL;
//
//	PresetItemsTeam1.clear();
//	PresetItemsTeam2.clear();
//
//	pSkinMenuTeam1		= NULL;
//	pSkinMenuTeam2		= NULL;
//
//	m_bTeamSelected		= TRUE;
}
//--------------------------------------------------------------------
void CUIGameTDM::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = dynamic_cast<game_cl_TeamDeathmatch*>(g);
	R_ASSERT(m_game);
}

void CUIGameTDM::Init ()
{
	//-----------------------------------------------------------
	CUITDMFragList* pFragListT1	= xr_new<CUITDMFragList>	();pFragListT1->SetAutoDelete(true);
	CUITDMFragList* pFragListT2	= xr_new<CUITDMFragList>	();pFragListT2->SetAutoDelete(true);

	pFragListT1->Init(1);
	pFragListT2->Init(2);

	int ScreenW = Device.dwWidth;
	int ScreenH = Device.dwHeight;
	//-----------------------------------------------------------
	RECT FrameRect = pFragListT1->GetFrameRect ();
	int FrameW	= FrameRect.right - FrameRect.left;
	int FrameH	= FrameRect.bottom - FrameRect.top;

	pFragListT1->SetWndRect(ScreenW/4-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
	FrameRect = pFragListT2->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pFragListT2->SetWndRect(ScreenW/4*3-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
//	m_aFragsLists.push_back(pFragListT1);
	m_pFragLists->AttachChild(pFragListT1);
//	m_aFragsLists.push_back(pFragListT2);
	m_pFragLists->AttachChild(pFragListT2);
	//-----------------------------------------------------------
	CUITDMPlayerList* pPlayerListT1	= xr_new<CUITDMPlayerList>	();pPlayerListT1->SetAutoDelete(true);
	CUITDMPlayerList* pPlayerListT2	= xr_new<CUITDMPlayerList>	();pPlayerListT2->SetAutoDelete(true);

	pPlayerListT1->Init(1);
	pPlayerListT2->Init(2);
	//-----------------------------------------------------------
	FrameRect = pPlayerListT1->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT1->SetWndRect(ScreenW/4-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
	FrameRect = pPlayerListT2->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT2->SetWndRect(ScreenW/4*3-FrameW/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
//	m_aPlayersLists.push_back(pPlayerListT1);
	m_pPlayerLists->AttachChild(pPlayerListT1);
//	m_aPlayersLists.push_back(pPlayerListT2);
	m_pPlayerLists->AttachChild(pPlayerListT2);

// moved to game_cl_teamdeathmatch
//	//-----------------------------------------------------------
//	string64	Team1, Team2;
//	std::strcpy(Team1, TEAM1_MENU);
//	std::strcpy(Team2, TEAM2_MENU);
//	m_aTeamSections.push_back(Team1);
//	m_aTeamSections.push_back(Team2);
//	//-----------------------------------------------------------
//	pBuyMenuTeam1 = InitBuyMenu("teamdeathmatch_base_cost", 1);
//	pBuyMenuTeam2 = InitBuyMenu("teamdeathmatch_base_cost", 2);
//	//-----------------------------------------------------------
//	pSkinMenuTeam1 = InitSkinMenu(1);
//	pSkinMenuTeam2 = InitSkinMenu(2);
}
//--------------------------------------------------------------------
CUIGameTDM::~CUIGameTDM()
{
// moved to game_cl_teamdeathmatch
//	xr_delete(pUITeamSelectWnd);
//
//	xr_delete(pBuyMenuTeam1);
//	xr_delete(pBuyMenuTeam2);
//
//	xr_delete(pSkinMenuTeam1);
//	xr_delete(pSkinMenuTeam2);
}
//--------------------------------------------------------------------
bool CUIGameTDM::IR_OnKeyboardPress(int dik)
{
/*
	if (m_game->phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_M:
			{
				m_game->StartStopMenu(pUITeamSelectWnd);
				return true;
			}break;
		};
	}*/
	if(inherited::IR_OnKeyboardPress(dik)) return true;

	return false;
}
/* moved to game_cl_teamdeathmatch
void CUIGameTDM::OnTeamSelect(int Team)
{
	if (Team+1 != m_game->local_player->team) 
	{
		CObject *l_pObj = Level().CurrentEntity();

		CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
		if(!l_pPlayer) return;

		NET_Packet		P;
		l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
		P.w_u16(GAME_EVENT_PLAYER_CHANGE_TEAM);
		
		P.w_s16			(s16(Team+1));
		//P.w_u32			(0);
		l_pPlayer->u_EventSend		(P);
		//-----------------------------------------------------------------
		m_bSkinSelected = TRUE;
	};
	//-----------------------------------------------------------------
	m_bTeamSelected = TRUE;	
};
//-----------------------------------------------------------------
void CUIGameTDM::SetCurrentBuyMenu	()
{
	if (!pCurBuyMenu || !pCurBuyMenu->IsShown())
	{
		if (m_game->local_player->team == 1) 
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

	if (pCurBuyMenu) pCurBuyMenu->SetSkin(m_game->local_player->skin);// pCurSkinMenu->GetActiveIndex());
	if (!pCurBuyMenu) return;

	game_cl_GameState::Player* P = m_game->local_player;
	if (!P) return;
//	pCurBuyMenu->SetMoneyAmount(P->money_for_round);
};

void		CUIGameTDM::SetCurrentSkinMenu	()
{
	CUISkinSelectorWnd* pNewSkinMenu;
	if (m_game->local_player->team == 1) pNewSkinMenu = pSkinMenuTeam1;
	else pNewSkinMenu = pSkinMenuTeam2;

	if (pNewSkinMenu != pCurSkinMenu)
		if (pCurSkinMenu && pCurSkinMenu->IsShown()) StartStopMenu(pCurSkinMenu);

	pCurSkinMenu = pNewSkinMenu;
	pCurSkinMenu->SwitchSkin(m_game->local_player->skin);
};

bool		CUIGameTDM::CanBeReady				()
{
	if (!m_bTeamSelected)
	{
		StartStopMenu(pUITeamSelectWnd);
		return false;
	}

	return inherited::CanBeReady();
};*/