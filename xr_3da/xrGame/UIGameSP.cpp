#include "stdafx.h"
#include "uigamesp.h"
#include "actor.h"
#include "level.h"

CUIGameSP::CUIGameSP()
{
	m_pUserMenu = NULL;
	pUIBuyWeaponWnd = xr_new<CUIBuyWeaponWnd>	((char*)"artefacthunt_team1", (char*)"artefacthunt_base_cost");
}

CUIGameSP::~CUIGameSP() 
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
		StartStopMenu(&PdaMenu);
		PdaMenu.ChangeActiveTab(CUIPdaWnd::TAB_MAP);
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
	
	
	//��������! ����, �� �������� ��� �� ������������
	//������ �������� �� ��������, � ������ ���������
	//� ���� ���������
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