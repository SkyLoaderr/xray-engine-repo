#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"
#include "ui\UIBuyWeaponWnd.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUIGameDM::CUIGameDM(CUI* parent):CUIGameCustom(parent)
{
	pFragList	= xr_new<CUIDMFragList>		();
	pPlayerList	= xr_new<CUIDMPlayerList>	();

	int ScreenW = Device.dwWidth;
	int ScreenH = Device.dwHeight;
	//-----------------------------------------------------------
	RECT FrameRect = pFragList->GetFrameRect ();
	int FrameW	= FrameRect.right - FrameRect.left;
	int FrameH	= FrameRect.bottom - FrameRect.top;

	pFragList->SetWndRect((ScreenW-FrameW)/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
	FrameRect = pPlayerList->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerList->SetWndRect((ScreenW-FrameW)/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	//-----------------------------------------------------------
	pBuyMenu	= xr_new<CUIBuyWeaponWnd>	((char*)"deathmatch_team0");
}
//--------------------------------------------------------------------

CUIGameDM::~CUIGameDM()
{
	xr_delete(pFragList);
	xr_delete(pPlayerList);
	xr_delete(pBuyMenu);
}
//--------------------------------------------------------------------

void CUIGameDM::OnFrame()
{
	inherited::OnFrame();	

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerList->Update();
		if (!pPlayerList->IsShown()) 
		{
			pPlayerList->Show();
		};
	break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) 
		{
			pFragList->Update();
		}break;
	break;
	}
}
//--------------------------------------------------------------------

void CUIGameDM::Render()
{
	inherited::Render();

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerList->Draw();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) 
		{
			pFragList->Draw	();
		}break;
	}
}
//--------------------------------------------------------------------

bool CUIGameDM::IR_OnKeyboardPress(int dik)
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
		case DIK_TAB:	
			{
				SetFlag		(flShowFragList,TRUE);	
				return true;
			}break;
		case DIK_B:
			{
				StartStopMenu(pBuyMenu);
			}break;
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIGameDM::IR_OnKeyboardRelease(int dik)
{
	if(inherited::IR_OnKeyboardRelease(dik)) return true;

	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:
			/*
			pFragList->Hide();
			m_pUserMenu = NULL;			
			*/
			SetFlag		(flShowFragList,FALSE);	
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
void CUIGameDM::OnBuyMenu_Ok	()
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

bool		CUIGameDM::CanBeReady				()
{
	u8 res = 0xff;

	res &=	pBuyMenu->GetWeaponIndex(KNIFE_SLOT);
	res &=	pBuyMenu->GetWeaponIndex(PISTOL_SLOT);
	res &=	pBuyMenu->GetWeaponIndex(RIFLE_SLOT);
	res &=	pBuyMenu->GetWeaponIndex(GRENADE_SLOT);
	res &=	~(pBuyMenu->GetBeltSize());

	if (res != 0xff) return true;

	StartStopMenu(pBuyMenu);
	return false;
};