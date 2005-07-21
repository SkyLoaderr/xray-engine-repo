#include "stdafx.h"
#include "uigamesp.h"
#include "actor.h"
#include "level.h"

#include "game_cl_Single.h"
#include "ui/UIPdaAux.h"
#include "xr_level_controller.h"
#include "actorcondition.h"
#include "../xr_ioconsole.h"
#include "object_broker.h"

#include "ui/UIInventoryWnd.h"
#include "ui/UITradeWnd.h"
#include "ui/UIPdaWnd.h"
#include "ui/UITalkWnd.h"
#include "ui/UICarBodyWnd.h"

CUIGameSP::CUIGameSP()
{
	m_game			= NULL;

	InventoryMenu	= xr_new<CUIInventoryWnd>	();
	TradeMenu		= xr_new<CUITradeWnd>		();
	PdaMenu			= xr_new<CUIPdaWnd>			();
	TalkMenu		= xr_new<CUITalkWnd>		();
	UICarBodyMenu	= xr_new<CUICarBodyWnd>		();

}

CUIGameSP::~CUIGameSP() 
{
	delete_data(InventoryMenu);
	delete_data(TradeMenu);	
	delete_data(PdaMenu);	
	delete_data(TalkMenu);
	delete_data(UICarBodyMenu);
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
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if( pActor && !pActor->g_Alive() ) return false;

	switch (key_binding[dik])
	{
	case kINVENTORY: 
		if( !MainInputReceiver() || MainInputReceiver()==InventoryMenu){
			m_game->StartStopMenu(InventoryMenu,true);
			return true;
		}break;

	case kACTIVE_JOBS:
		if( !MainInputReceiver() || MainInputReceiver()==PdaMenu){
			PdaMenu->SetActiveSubdialog(eptQuests);
			m_game->StartStopMenu(PdaMenu,true);
			return true;
		}break;

	case kMAP:
		if( !MainInputReceiver() || MainInputReceiver()==PdaMenu){
			PdaMenu->SetActiveSubdialog(eptMap);
			m_game->StartStopMenu(PdaMenu,true);
			return true;
		}break;

	case kCONTACTS:
		if( !MainInputReceiver() || MainInputReceiver()==PdaMenu){
			PdaMenu->SetActiveSubdialog(eptContacts);
			m_game->StartStopMenu(PdaMenu,true);
			return true;
		}break;

/*	case kQUIT:
		if( MainInputReceiver() )//m_pUserMenu)
		{
			m_game->StartStopMenu(MainInputReceiver(), true);
			return true;
		}break;
*/
	}
	
/*	
	//временно! пока, по убыванию сил до критического
	//уровня персонаж не засыпает, а просто переходит
	//в меню инвенторя
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && pActor->g_Alive() && !pActor->conditions().GetPower())
	{
			if( MainInputReceiver() == NULL)
			{
				//start the inventory menu
				m_game->StartStopMenu(&InventoryMenu,true);
				return true;
			}
	}
	*/

	return false;
}

bool CUIGameSP::IR_OnKeyboardRelease(int dik) 
{
	if(inherited::IR_OnKeyboardRelease(dik)) return true;
	return false;
}


void CUIGameSP::StartTalk()
{
	m_game->StartStopMenu(TalkMenu,true);
}

void CUIGameSP::StartCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
							 CInventory* pOthersInv, CGameObject* pOthersObject)
{
	UICarBodyMenu->InitCarBody(pOurInv,  pOurObject,
		                      pOthersInv, pOthersObject);
	m_game->StartStopMenu(UICarBodyMenu,true);
}

void CUIGameSP::ReInitInventoryWnd		() 
{ 
	if (InventoryMenu->IsShown()) 
		InventoryMenu->InitInventory(); 
};
