#include "stdafx.h"
#include "uigamesp.h"
#include "actor.h"
#include "level.h"

#include "game_cl_Single.h"

CUIGameSP::CUIGameSP()
{
	m_game			= NULL;
//	m_pUserMenu		= NULL;
//	pUIBuyWeaponWnd = xr_new<CUIBuyWeaponWnd>	((char*)"artefacthunt_team1", (char*)"artefacthunt_base_cost");
}

CUIGameSP::~CUIGameSP() 
{
//	xr_delete(pUIBuyWeaponWnd);
}

void CUIGameSP::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = dynamic_cast<game_cl_Single*>(g);
	R_ASSERT(m_game);
}


bool CUIGameSP::IR_OnKeyboardPress(int dik) 
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;


	switch (dik)
	{
	case DIK_I: 
		m_game->StartStopMenu(&InventoryMenu);
		return true;
		break;
	case DIK_P:
		m_game->StartStopMenu(&PdaMenu);
		PdaMenu.ChangeActiveTab(CUIPdaWnd::TAB_COMM);
		return true;
		break;
	case DIK_M:
		m_game->StartStopMenu(&PdaMenu);
		PdaMenu.ChangeActiveTab(CUIPdaWnd::TAB_MAP);
		return true;
		break;
/*
	case DIK_B:
///		StartStopMenu(&UIStatsWnd);
		StartStopMenu(pUIBuyWeaponWnd);
		return true;
		break;
*/
	case DIK_ESCAPE:
		if(m_pMainInputReceiver)//m_pUserMenu)
		{
			m_game->StartStopMenu(m_pMainInputReceiver);//m_pUserMenu);
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
			if(m_pMainInputReceiver/*m_pUserMenu*/ == NULL)
			{
				//start the inventory menu
				m_game->StartStopMenu(&InventoryMenu);
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
	m_game->StartStopMenu(&TalkMenu);
}
void CUIGameSP::StartCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
							 CInventory* pOthersInv, CGameObject* pOthersObject)
{
	UICarBodyMenu.InitCarBody(pOurInv,  pOurObject,
		                      pOthersInv, pOthersObject);
	m_game->StartStopMenu(&UICarBodyMenu);
}