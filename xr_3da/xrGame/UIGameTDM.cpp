#include "stdafx.h"
#include "UIGameTDM.h"

#include "UITDMPlayerList.h"
#include "UITDMFragList.h"

#include "hudmanager.h"

#define MSGS_OFFS 510

#define	TEAM1_MENU		"teamdeathmatch_team1"
#define	TEAM2_MENU		"teamdeathmatch_team2"

//--------------------------------------------------------------------
CUIGameTDM::CUIGameTDM(CUI* parent):CUIGameDM(parent)
{
	ClearLists();
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
	pUITeamSelectWnd = xr_new<CUISpawnWnd>	();
}
//--------------------------------------------------------------------
CUIGameTDM::~CUIGameTDM()
{
	xr_delete(pUITeamSelectWnd);
}
//--------------------------------------------------------------------
bool CUIGameTDM::IR_OnKeyboardPress(int dik)
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;

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
	return false;
}
//--------------------------------------------------------------------
void CUIGameTDM::OnTeamSelect(int Team)
{
	if (Team+1 == Game().local_player->team) return;

	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P,GEG_PLAYER_CHANGE_TEAM,l_pPlayer->ID()	);
	P.w_s16			(s16(Team+1));
	//P.w_u32			(0);
	l_pPlayer->u_EventSend		(P);
	//-----------------------------------------------------------------
	InitBuyMenu(s16(Team+1));
};
//--------------------------------------------------------------------
/*
void CUIGameTDM::OnFrame()
{
	inherited::OnFrame();	

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerListT1->Update();
		pPlayerListT2->Update();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) 
		{
			pFragListT1->Update();
			pFragListT2->Update();
		}break;
	}
}
//--------------------------------------------------------------------

void CUIGameTDM::Render()
{
	inherited::Render();

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
//		HUD().GetUI()->HideIndicators();
		pPlayerListT1->Draw();
		pPlayerListT2->Draw();
		break;
	case GAME_PHASE_INPROGRESS:
//		HUD().GetUI()->ShowIndicators();
		if (uFlags&flShowFragList) 
		{
			pFragListT1->Draw	();
			pFragListT2->Draw	();
		}break;
	}
}

//--------------------------------------------------------------------

bool CUIGameTDM::IR_OnKeyboardRelease(int dik)
{
	if(inherited::IR_OnKeyboardRelease(dik)) return true;

	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,FALSE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------

void CUIGameTDM::OnBuyMenu_Ok	()
{
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P,GEG_PLAYER_BUY_FINISHED,l_pPlayer->ID()	);

	P.w_u8		(pBuyMenu->GetWeaponIndex(KNIFE_SLOT));
	P.w_u8		(pBuyMenu->GetWeaponIndex(PISTOL_SLOT));
	P.w_u8		(pBuyMenu->GetWeaponIndex(RIFLE_SLOT));
	P.w_u8		(pBuyMenu->GetWeaponIndex(GRENADE_SLOT));

	P.w_u8		(pBuyMenu->GetBeltSize());

	for (u8 i=0; i<pBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
		P.w_u8	(SectID);
		P.w_u8	(ItemID);
	};	

	l_pPlayer->u_EventSend		(P);
};

bool		CUIGameTDM::CanBeReady				()
{
	if (pBuyMenu)
	{
		u8 res = 0xff;

		res &=	pBuyMenu->GetWeaponIndex(KNIFE_SLOT);
		res &=	pBuyMenu->GetWeaponIndex(PISTOL_SLOT);
		res &=	pBuyMenu->GetWeaponIndex(RIFLE_SLOT);
		res &=	pBuyMenu->GetWeaponIndex(GRENADE_SLOT);

		if (res != 0xff) return true;
	};

	IR_OnKeyboardPress(DIK_B);
	return false;
};
*/