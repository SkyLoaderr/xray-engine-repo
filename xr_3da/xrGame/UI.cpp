#include "stdafx.h"
#include "UI.h"
#include "../xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "UIGameSP.h"
#include "UIGameDM.h"
#include "UIGameTDM.h"
#include "UIGameAHunt.h"
#include "actor.h"
#include "level.h"
#include "game_cl_base.h"


#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUI::CUI(CHUDManager* p)
{

	UIMainIngameWnd.Init();

	m_Parent		= p;
	pUIGame			= 0;

	msgs_offs		= (float)m_Parent->ClientToScreenY(MSGS_OFFS,alLeft|alBottom);

	m_bShowIndicators = true;

	shedule.t_min			= 5;
	shedule.t_max			= 20;
	shedule_register();
}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); messages.end() != it; ++it)
		xr_delete(*it);
	xr_delete(pUIGame);

	shedule_unregister();
}

float CUI::shedule_Scale		() 
{
	return 0.5f;
};
//--------------------------------------------------------------------

void CUI::Load()
{
	pUIGame = Game().createGameUI();
	//pUIGame->IR_OnKeyboardPress(8);
	R_ASSERT(pUIGame);
}
//--------------------------------------------------------------------

u32 ScaleAlpha(u32 val, float factor)
{
	u32	r, g, b, a;

	float	_a		= float((val >> 24)&0xff)*factor;
	a				= iFloor(_a);
	r				= (val >> 16)&0xff;
	g				= (val >>  8)&0xff;
	b				= (val >>  0)&0xff;
    return ((u32)(a<<24) | (r<<16) | (g<<8) | (b));
}

void CUI::OnFrame()
{
	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor){
		
		//update windows
		if(m_bShowIndicators)
		{
			UIMainIngameWnd.SetFont(m_Parent->pFontMedium);
			UIMainIngameWnd.Update();
		}
			

	}
	// out GAME-style depend information
	if (pUIGame) pUIGame->OnFrame	();

#ifdef DEBUG
	if (!messages.empty()){
		m_Parent->pFontSmall->OutSet(0,msgs_offs);
		for (int i=messages.size()-1; i>=0; --i){
			SUIMessage* M = messages[i];
			M->life_time-=Device.dwTimeDelta;
			if (M->life_time<0){
				xr_delete(messages[i]);
				messages.erase(i);
				--i;
				continue;
			}
			u32 color = messages[i]->color;
			if (M->life_time<=(HIDE_TIME*1000))
				color = ScaleAlpha(color, float(M->life_time)/float(HIDE_TIME*1000));
			m_Parent->pFontSmall->SetColor(color);
			m_Parent->pFontSmall->OutPrev("%s: %s",messages[i]->sender,messages[i]->msg);
		}
	}
#endif
}
//--------------------------------------------------------------------

bool CUI::Render()
{
	// out GAME-style depend information
	xr_vector<CUIWindow*>::iterator it = m_dialogsToRender.begin();
	for(; it!=m_dialogsToRender.end();++it)
		if((*it)->IsShown())
			(*it)->Draw();

	if (pUIGame) pUIGame->Render	();

	//----------
	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor)
	{
		//Draw main window and its children
		if(m_bShowIndicators)
			UIMainIngameWnd.Draw();
		else
			UIMainIngameWnd.DrawPdaMessages();
		//render cursor only when it visible
		if(UICursor.IsVisible())
            UICursor.Render();
	}	

	return false;
}
//--------------------------------------------------------------------
bool CUI::IR_OnKeyboardPress(int dik)
{
	if(UIMainIngameWnd.OnKeyboardPress(dik))
		return true;

	if ( MainInputReceiver()&&MainInputReceiver()->IR_OnKeyboardPress(dik)) 
		return true;

	if (pUIGame && pUIGame->IR_OnKeyboardPress(dik)) 
		return true;

	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnKeyboardRelease(int dik)
{
	if ( MainInputReceiver()&&MainInputReceiver()->IR_OnKeyboardRelease(dik)) 
		return true;

	if (pUIGame&&pUIGame->IR_OnKeyboardRelease(dik)) 
		return true;

	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnMouseMove(int dx,int dy)
{
	if ( MainInputReceiver()&&MainInputReceiver()->IR_OnMouseMove(dx,dy)) 
		return true;
	
	if (pUIGame&&pUIGame->IR_OnMouseMove(dx,dy)) 
		return true;
	
	return false;
}

void CUI::AddMessage(LPCSTR S, LPCSTR M, u32 C, float life_time)
{
	if (messages.size()==MAX_UIMESSAGES){
		xr_delete(messages.front());
		messages.erase(u32(0));
	}
	messages.push_back(xr_new<SUIMessage> (S,M,C,life_time));
}


CUIDialogWnd* CUI::MainInputReceiver()
{ 
	if ( !m_input_receivers.empty() ) 
		return m_input_receivers.top(); 
	return NULL; 
};

void CUI::SetMainInputReceiver	(CUIDialogWnd* ir)	
{ 
	if( MainInputReceiver() == ir ) return;
	if(!ir){//remove
		if(!m_input_receivers.empty())
				m_input_receivers.pop();
	}else{
		m_input_receivers.push(ir);
	}
};

void CUI::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	if( pDialog->IsShown() )
		StopMenu(pDialog, bDoHideIndicators);
	else
		StartMenu(pDialog, bDoHideIndicators);

	xr_vector<CUIWindow*>::iterator it = std::find(m_dialogsToErase.begin(), m_dialogsToErase.end(), pDialog);
	if (m_dialogsToErase.end() != it)
		m_dialogsToErase.erase(it);
}

void CUI::StartMenu (CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
//	if (m_game_ui_custom->MainInputReceiver() != NULL) return;

	R_ASSERT( !pDialog->IsShown() );
//	R_ASSERT( m_game_ui_custom->MainInputReceiver() == NULL );

	AddDialogToRender(pDialog);
	SetMainInputReceiver(pDialog);
	pDialog->Show();

	if(bDoHideIndicators){
		HideIndicators();
		ShowCursor();
		m_bCrosshair = !!psHUD_Flags.test(HUD_CROSSHAIR);
		if(m_bCrosshair) 
			psHUD_Flags.set(HUD_CROSSHAIR, FALSE);
	}

}

void CUI::StopMenu (CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	R_ASSERT( pDialog->IsShown() );
	R_ASSERT( MainInputReceiver()==pDialog );

	RemoveDialogToRender(pDialog);
	SetMainInputReceiver(NULL);
	pDialog->Hide();

	if( bDoHideIndicators && !MainInputReceiver() ){
		ShowIndicators();
		HideCursor();
		if(m_bCrosshair) 
			psHUD_Flags.set(HUD_CROSSHAIR, TRUE);
	};
}

void CUI::AddDialogToRender(CUIWindow* pDialog)
{
	if(std::find(m_dialogsToRender.begin(), m_dialogsToRender.end(), pDialog) == m_dialogsToRender.end() )
	{
		m_dialogsToRender.push_back(pDialog);
		pDialog->Show(true);
	}
}

void CUI::RemoveDialogToRender(CUIWindow* pDialog)
{
	xr_vector<CUIWindow*>::iterator it = std::find(m_dialogsToRender.begin(),m_dialogsToRender.end(),pDialog);
	if(it != m_dialogsToRender.end())
	{
		(*it)->Show(false);
		(*it)->Enable(false);
		m_dialogsToErase.push_back(*it);
	}
}

void CUI::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);

	xr_vector<CUIWindow*>::iterator it = m_dialogsToRender.begin();
	for(; it!=m_dialogsToRender.end();++it)
		if((*it)->IsEnabled())
			(*it)->Update();


	for(it = m_dialogsToErase.begin(); it!=m_dialogsToErase.end(); ++it)
	{
		xr_vector<CUIWindow*>::iterator it_find = std::find(m_dialogsToRender.begin(),
															m_dialogsToRender.end(), *it);
		if (it_find != m_dialogsToRender.end())
		{
			m_dialogsToRender.erase(it_find);
		}
    }
	m_dialogsToErase.clear();

}
