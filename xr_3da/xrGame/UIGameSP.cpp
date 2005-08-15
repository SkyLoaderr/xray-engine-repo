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
#include "ui/UIMessageBox.h"

CUIGameSP::CUIGameSP()
{
	m_game			= NULL;

	InventoryMenu	= xr_new<CUIInventoryWnd>	();
	TradeMenu		= xr_new<CUITradeWnd>		();
	PdaMenu			= xr_new<CUIPdaWnd>			();
	TalkMenu		= xr_new<CUITalkWnd>		();
	UICarBodyMenu	= xr_new<CUICarBodyWnd>		();
	UIChangeLevelWnd= xr_new<CChangeLevelWnd>		();

}

CUIGameSP::~CUIGameSP() 
{
	delete_data(InventoryMenu);
	delete_data(TradeMenu);	
	delete_data(PdaMenu);	
	delete_data(TalkMenu);
	delete_data(UICarBodyMenu);
	delete_data(UIChangeLevelWnd);
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

void CUIGameSP::ChangeLevel				(u32 game_vert_id, u32 level_vert_id, Fvector pos, Fvector ang)
{
	UIChangeLevelWnd->m_game_vertex_id		= game_vert_id;
	UIChangeLevelWnd->m_level_vertex_id		= level_vert_id;
	UIChangeLevelWnd->m_position			= pos;
	UIChangeLevelWnd->m_angles				= ang;

	m_game->StartStopMenu(UIChangeLevelWnd,true);
}


CChangeLevelWnd::CChangeLevelWnd		()
{
	m_messageBox			= xr_new<CUIMessageBox>();	m_messageBox->SetAutoDelete(true);
	SetWndPos				(200.0f,200.0f);
	SetWndSize				(Fvector2().set(300.0f,150.0f));
	AttachChild				(m_messageBox);
	m_messageBox->Init		("message_box_error");
//	m_messageBox->AutoCenter();
//	m_messageBox->SetStyle	(CUIMessageBox::MESSAGEBOX_YES_NO);
	m_messageBox->SetText	("Leave current level and jump to another ?");
}

void CChangeLevelWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd==m_messageBox){
		if(msg==MESSAGE_BOX_YES_CLICKED){
			Game().StartStopMenu	(this, true);
			NET_Packet		p;
			p.w_begin		(M_CHANGE_LEVEL);
			p.w				(&m_game_vertex_id,sizeof(m_game_vertex_id));
			p.w				(&m_level_vertex_id,sizeof(m_level_vertex_id));
			p.w_vec3		(m_position);
			p.w_vec3		(m_angles);

//			Level().Send	(m_net_packet,net_flags(TRUE));
		}
	}else{
		if(msg==MESSAGE_BOX_NO_CLICKED){
			Game().StartStopMenu	(this, true);
		}
	}
}
