#include "stdafx.h"
#include "uigamesp.h"
#include "actor.h"
#include "level.h"

#include "game_cl_Single.h"
#include "ui/UIPdaAux.h"
#include "xr_level_controller.h"

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
	m_game = smart_cast<game_cl_Single*>(g);
	R_ASSERT(m_game);
}


bool CUIGameSP::IR_OnKeyboardPress(int dik) 
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;
	
	switch (key_binding[dik])
	{
	case kINVENTORY: 
		m_game->StartStopMenu(&InventoryMenu,true);
		return true;
		break;
	case kACTIVE_JOBS:
		PdaMenu.SetActiveSubdialog(epsActiveJobs);
		m_game->StartStopMenu(&PdaMenu,true);
		return true;
		break;
	case kMAP:
		PdaMenu.SetActiveSubdialog(epsMap);
		m_game->StartStopMenu(&PdaMenu,true);
		return true;
		break;
	case kCONTACTS:
		PdaMenu.SetActiveSubdialog(epsContacts);
		m_game->StartStopMenu(&PdaMenu,true);
		return true;
		break;
/*
	case DIK_B:
///		StartStopMenu(&UIStatsWnd);
		StartStopMenu(pUIBuyWeaponWnd);
		return true;
		break;
*/
	case kQUIT:
		if(m_pMainInputReceiver)//m_pUserMenu)
		{
			m_game->StartStopMenu(m_pMainInputReceiver,true);//m_pUserMenu);
			return true;
		}
		break;
	}
	
	
	//временно! пока, по убыванию сил до критического
	//уровня персонаж не засыпает, а просто переходит
	//в меню инвенторя
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && pActor->g_Alive() && !pActor->GetPower())
	{
			if(m_pMainInputReceiver/*m_pUserMenu*/ == NULL)
			{
				//start the inventory menu
				m_game->StartStopMenu(&InventoryMenu,true);
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
	m_game->StartStopMenu(&TalkMenu,true);
}
void CUIGameSP::StartCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
							 CInventory* pOthersInv, CGameObject* pOthersObject)
{
	UICarBodyMenu.InitCarBody(pOurInv,  pOurObject,
		                      pOthersInv, pOthersObject);
	m_game->StartStopMenu(&UICarBodyMenu,true);
}