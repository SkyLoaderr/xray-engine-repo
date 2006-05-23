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
#include "GameTaskManager.h"
#include "GameTask.h"

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
	PdaMenu			= xr_new<CUIPdaWnd>			();
	TalkMenu		= xr_new<CUITalkWnd>		();
	UICarBodyMenu	= xr_new<CUICarBodyWnd>		();
	UIChangeLevelWnd= xr_new<CChangeLevelWnd>		();
}

CUIGameSP::~CUIGameSP() 
{
	delete_data(InventoryMenu);
	delete_data(PdaMenu);	
	delete_data(TalkMenu);
	delete_data(UICarBodyMenu);
	delete_data(UIChangeLevelWnd);
}

void CUIGameSP::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame				(g);
	m_game = smart_cast<game_cl_Single*>(g);
	R_ASSERT							(m_game);
}


bool CUIGameSP::IR_OnKeyboardPress(int dik) 
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;

	/*
#ifndef DEBUG
	if(key_binding[dik]== kINVENTORY)
	{
		if( !MainInputReceiver() || MainInputReceiver()==InventoryMenu){
			m_game->StartStopMenu(InventoryMenu,true);
			return true;
		}break;
	}
#endif
	*/
	if( Device.Pause()		) return false;

	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor)								return false;
	if( pActor && !pActor->g_Alive() )		return false;

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

	case kSCORES:
		{
			SDrawStaticStruct* ss	= AddCustomStatic("main_task", true);
			SGameTaskObjective* o	= pActor->GameTaskManager().ActiveTask();
			if(!o)
				ss->m_static->SetTextST	("st_no_active_task");
			else
				ss->m_static->SetTextST	(*(o->description));

		}break;
	}
	return false;
}

bool CUIGameSP::IR_OnKeyboardRelease(int dik) 
{
	if(inherited::IR_OnKeyboardRelease(dik)) return true;

	switch (key_binding[dik])
	{
	case kSCORES:
		{
			RemoveCustomStatic		("main_task");
		}break;
	};
	return false;
}


void CUIGameSP::StartTalk()
{
	m_game->StartStopMenu(TalkMenu,true);
}

void CUIGameSP::StartCarBody(CInventoryOwner* pOurInv, CInventoryOwner* pOthers)
{
	UICarBodyMenu->InitCarBody		(pOurInv,  pOthers);
	m_game->StartStopMenu			(UICarBodyMenu,true);
}
void CUIGameSP::StartCarBody(CInventoryOwner* pOurInv, CInventoryBox* pBox)
{
	UICarBodyMenu->InitCarBody		(pOurInv,  pBox);
	m_game->StartStopMenu			(UICarBodyMenu,true);
}

void CUIGameSP::ReInitShownUI() 
{ 
	if (InventoryMenu->IsShown()) 
		InventoryMenu->InitInventory_delayed(); 
	else if(UICarBodyMenu->IsShown())
		UICarBodyMenu->UpdateLists_delayed();
	
};


extern ENGINE_API BOOL bShowPauseString;
void CUIGameSP::ChangeLevel				(GameGraph::_GRAPH_ID game_vert_id, u32 level_vert_id, Fvector pos, Fvector ang)
{
	if( !MainInputReceiver() || MainInputReceiver()!=UIChangeLevelWnd)
	{
		UIChangeLevelWnd->m_game_vertex_id		= game_vert_id;
		UIChangeLevelWnd->m_level_vertex_id		= level_vert_id;
		UIChangeLevelWnd->m_position			= pos;
		UIChangeLevelWnd->m_angles				= ang;
		m_game->StartStopMenu					(UIChangeLevelWnd,true);
	}
}


CChangeLevelWnd::CChangeLevelWnd		()
{
	m_messageBox			= xr_new<CUIMessageBox>();	m_messageBox->SetAutoDelete(true);
	AttachChild				(m_messageBox);
	m_messageBox->Init		("message_box_change_level");
	SetWndPos				(m_messageBox->GetWndPos());
	m_messageBox->SetWndPos	(0.0f,0.0f);
	SetWndSize				(m_messageBox->GetWndSize());
}

void CChangeLevelWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd==m_messageBox){
		if(msg==MESSAGE_BOX_YES_CLICKED){

			Game().StartStopMenu					(this, true);
			NET_Packet								p;
			p.w_begin								(M_CHANGE_LEVEL);
			p.w										(&m_game_vertex_id,sizeof(m_game_vertex_id));
			p.w										(&m_level_vertex_id,sizeof(m_level_vertex_id));
			p.w_vec3								(m_position);
			p.w_vec3								(m_angles);

			Level().Send							(p,net_flags(TRUE));
		}else
		if(msg==MESSAGE_BOX_NO_CLICKED){
			Game().StartStopMenu					(this, true);
		}
	}else
		inherited::SendMessage(pWnd, msg, pData);
}

void CChangeLevelWnd::Show()
{
	Device.Pause							(TRUE);
	bShowPauseString						= FALSE;
}

void CChangeLevelWnd::Hide()
{
	Device.Pause							(FALSE);
}

