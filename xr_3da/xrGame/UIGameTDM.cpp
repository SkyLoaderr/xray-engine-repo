#include "stdafx.h"
#include "UIGameTDM.h"

#include "UITDMPlayerList.h"
#include "UITDMFragList.h"

#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"

#define MSGS_OFFS 510

#define	TEAM1_MENU		"teamdeathmatch_team1"
#define	TEAM2_MENU		"teamdeathmatch_team2"

//--------------------------------------------------------------------
CUIGameTDM::CUIGameTDM(CUI* parent):CUIGameDM(parent)
{
	pUITeamSelectWnd = xr_new<CUISpawnWnd>	();
	pBuyMenuTeam1 = NULL;
	pBuyMenuTeam2 = NULL;

	pSkinMenuTeam1 = NULL;
	pSkinMenuTeam2 = NULL;

	m_bTeamSelected	= TRUE;
}
//--------------------------------------------------------------------
void		CUIGameTDM::Init				()
{
	//-----------------------------------------------------------
	CUITDMFragList* pFragListT1	= xr_new<CUITDMFragList>	();
	CUITDMFragList* pFragListT2	= xr_new<CUITDMFragList>	();

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
	m_aFragsLists.push_back(pFragListT1);
	m_aFragsLists.push_back(pFragListT2);
	//-----------------------------------------------------------
	CUITDMPlayerList* pPlayerListT1	= xr_new<CUITDMPlayerList>	();
	CUITDMPlayerList* pPlayerListT2	= xr_new<CUITDMPlayerList>	();

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
	m_aPlayersLists.push_back(pPlayerListT1);
	m_aPlayersLists.push_back(pPlayerListT2);
	//-----------------------------------------------------------
	string64	Team1, Team2;
	std::strcpy(Team1, TEAM1_MENU);
	std::strcpy(Team2, TEAM2_MENU);
	m_aTeamSections.push_back(Team1);
	m_aTeamSections.push_back(Team2);
	//-----------------------------------------------------------
	pBuyMenuTeam1 = InitBuyMenu(1);
	pBuyMenuTeam2 = InitBuyMenu(2);
	//-----------------------------------------------------------
	pSkinMenuTeam1 = InitSkinMenu(1);
	pSkinMenuTeam2 = InitSkinMenu(2);
}
//--------------------------------------------------------------------
CUIGameTDM::~CUIGameTDM()
{
	xr_delete(pUITeamSelectWnd);

	xr_delete(pBuyMenuTeam1);
	xr_delete(pBuyMenuTeam2);

	xr_delete(pSkinMenuTeam1);
	xr_delete(pSkinMenuTeam2);
}
//--------------------------------------------------------------------
bool CUIGameTDM::IR_OnKeyboardPress(int dik)
{
	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_M:
			{
				StartStopMenu(pUITeamSelectWnd);
				return true;
			}break;
		};
	}
	if(inherited::IR_OnKeyboardPress(dik)) return true;

	return false;
}
//--------------------------------------------------------------------
void CUIGameTDM::OnTeamSelect(int Team)
{
	if (Team+1 != Game().local_player->team) 
	{
		CObject *l_pObj = Level().CurrentEntity();

		CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
		if(!l_pPlayer) return;

		NET_Packet		P;
		l_pPlayer->u_EventGen		(P,GEG_PLAYER_CHANGE_TEAM,l_pPlayer->ID()	);
		P.w_s16			(s16(Team+1));
		//P.w_u32			(0);
		l_pPlayer->u_EventSend		(P);
		//-----------------------------------------------------------------
		m_bSkinSelected = FALSE;
	};
	//-----------------------------------------------------------------
	m_bTeamSelected = TRUE;	
};
//-----------------------------------------------------------------
void CUIGameTDM::SetCurrentBuyMenu	()
{
	if (!pCurBuyMenu || !pCurBuyMenu->IsShown())
	{
		if (Game().local_player->team == 1) pCurBuyMenu = pBuyMenuTeam1;
		else pCurBuyMenu = pBuyMenuTeam2;
	};
};

void		CUIGameTDM::SetCurrentSkinMenu	()
{
	CUISkinSelectorWnd* pNewSkinMenu;
	if (Game().local_player->team == 1) pNewSkinMenu = pSkinMenuTeam1;
	else pNewSkinMenu = pSkinMenuTeam2;

	if (pNewSkinMenu != pCurSkinMenu)
		if (pCurSkinMenu && pCurSkinMenu->IsShown()) StartStopMenu(pCurSkinMenu);

	pCurSkinMenu = pNewSkinMenu;
};

bool		CUIGameTDM::CanBeReady				()
{
	if (!m_bTeamSelected)
	{
		StartStopMenu(pUITeamSelectWnd);
		return false;
	}

	return inherited::CanBeReady();
};