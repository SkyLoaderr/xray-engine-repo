#include "stdafx.h"
#include "uigamesp.h"

#include "actor.h"

CUIGameSP::CUIGameSP(CUI* parent) : CUIGameCustom(parent) 
{
	m_pUserMenu = NULL;

	pUIBuyWeaponWnd = xr_new<CUIBuyWeaponWnd>	((char*)"deathmatch_team0");
}

CUIGameSP::~CUIGameSP(void) 
{
	xr_delete(pUIBuyWeaponWnd);
}


bool CUIGameSP::IR_OnKeyboardPress(int dik) 
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;


	switch (dik)
	{
	case DIK_I: 
		StartStopMenu(&InventoryMenu);
		return true;
		break;
	case DIK_P:
		StartStopMenu(&PdaMenu);
		return true;
		break;
	case DIK_M:
		StartStopMenu(&UISpawnWnd);
		return true;
		break;
	case DIK_B:
///		StartStopMenu(&UIStatsWnd);
		StartStopMenu(pUIBuyWeaponWnd);
		return true;
		break;
	case DIK_ESCAPE:
		if(m_pUserMenu)
		{
			StartStopMenu(m_pUserMenu);
			return true;
		}
		break;
	}
	
	
	//временно! пока, по убыванию сил до критического
	//уровня персонаж не засыпает, а просто переходит
	//в меню инвенторя
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor && pActor->g_Alive() && !pActor->GetPower())
	{
			if(m_pUserMenu == NULL)
			{
				//start the inventory menu
				StartStopMenu(&InventoryMenu);
				return true;
			}
	}
	

	return false;
}

bool CUIGameSP::IR_OnKeyboardRelease(int dik) 
{
	if(inherited::IR_OnKeyboardRelease(dik)) return true;
	return false;
}


void CUIGameSP::StartTalk()
{
	StartStopMenu(&TalkMenu);
}
void CUIGameSP::StartCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
							 CInventory* pOthersInv, CGameObject* pOthersObject)
{
	UICarBodyMenu.InitCarBody(pOurInv,  pOurObject,
		                      pOthersInv, pOthersObject);
	StartStopMenu(&UICarBodyMenu);
}