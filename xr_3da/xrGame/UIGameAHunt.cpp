#include "stdafx.h"
#include "UIGameAHunt.h"

#include "UIAHuntPlayerList.h"
#include "UIAHuntFragList.h"

#include "hudmanager.h"

#define MSGS_OFFS 510

#define	TEAM1_MENU		"artefacthunt_team1"
#define	TEAM2_MENU		"artefacthunt_team2"

//--------------------------------------------------------------------
CUIGameAHunt::CUIGameAHunt(CUI* parent):CUIGameCustom(parent)
{
	pFragListT1	= xr_new<CUIAHuntFragList>	();
	pFragListT2	= xr_new<CUIAHuntFragList>	();

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

	pPlayerListT1	= xr_new<CUIAHuntPlayerList>	();
	pPlayerListT2	= xr_new<CUIAHuntPlayerList>	();

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
	pUITeamSelectWnd = xr_new<CUISpawnWnd>	();
//	pUITeamSelectWnd->SetCallbackFunc((ButtonClickCallback)OnSelectTeamCallback);
	//-----------------------------------------------------------
	pBuyMenu = NULL;

	game_cl_GameState::Player* pCurPlayer = Game().local_player;
	if (pCurPlayer->team != -1) InitBuyMenu();
}
//--------------------------------------------------------------------
void CUIGameAHunt::InitBuyMenu				(s16 Team)
{
	if (Team == -1)
	{
		Team = Game().local_player->team;
	}
	

	if (!pBuyMenu)
	{
		if (Team == 1) pBuyMenu	= xr_new<CUIBuyWeaponWnd>	((char*)TEAM1_MENU);
		if (Team == 2) pBuyMenu	= xr_new<CUIBuyWeaponWnd>	((char*)TEAM2_MENU);
	}
	else
	{
		if (Team == 1) pBuyMenu->ReInitItems((char*)TEAM1_MENU);
		if (Team == 2) pBuyMenu->ReInitItems((char*)TEAM2_MENU);
	};
};
//--------------------------------------------------------------------

CUIGameAHunt::~CUIGameAHunt()
{
	xr_delete(pFragListT1);
	xr_delete(pFragListT2);
	xr_delete(pPlayerListT1);
	xr_delete(pPlayerListT2);

	xr_delete(pUITeamSelectWnd);

	xr_delete(pBuyMenu);
}
//--------------------------------------------------------------------

void CUIGameAHunt::OnFrame()
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

void CUIGameAHunt::Render()
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

bool CUIGameAHunt::IR_OnKeyboardPress(int dik)
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;

	switch (dik)
	{
	case DIK_I: 
		StartStopMenu(&InventoryMenu);
		return true;
		break;
	}

	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,TRUE);	return true;
		case DIK_M:
			{
				StartStopMenu(pUITeamSelectWnd);
				return true;
			}break;
		case DIK_B:
			{
				if (!pBuyMenu) InitBuyMenu();
				StartStopMenu(pBuyMenu);
			}break;
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIGameAHunt::IR_OnKeyboardRelease(int dik)
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
void CUIGameAHunt::OnTeamSelect(int Team)
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
	InitBuyMenu(s16(Team+1));
};
//--------------------------------------------------------------------
void CUIGameAHunt::OnBuyMenu_Ok	()
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

bool		CUIGameAHunt::CanBeReady				()
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