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
#include "ui/UIMainIngameWnd.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUI::CUI(CHUDManager* p)
{
	UIMainIngameWnd	= xr_new<CUIMainIngameWnd>	();
	UIMainIngameWnd->Init						();

	m_Parent		= p;
	pUIGame			= 0;

	msgs_offs		= (float)(UI()->ClientToScreenY(MSGS_OFFS,alLeft|alBottom));

	m_bShowIndicators = true;

}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); messages.end() != it; ++it)
		xr_delete(*it);
	xr_delete(pUIGame);
	xr_delete(UIMainIngameWnd);
}

//--------------------------------------------------------------------

void CUI::Load()
{
	pUIGame = Game().createGameUI();
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

void CUI::UIOnFrame()
{
	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor){
		
		//update windows
		if(m_bShowIndicators)
		{
			UIMainIngameWnd->SetFont(m_Parent->Font().pFontMedium);
			UIMainIngameWnd->Update	();
		}
	}

	// out GAME-style depend information
	if (pUIGame) pUIGame->OnFrame	();

#ifdef DEBUG
	if (!messages.empty()){
		m_Parent->Font().pFontSmall->OutSet(0,msgs_offs);
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
			m_Parent->Font().pFontSmall->SetColor(color);
			m_Parent->Font().pFontSmall->OutPrev("%s: %s",messages[i]->sender,messages[i]->msg);
		}
	}
#endif
}
//--------------------------------------------------------------------

bool CUI::Render()
{
	DoRenderDialogs();
	// out GAME-style depend information
/*	xr_vector<CUIWindow*>::iterator it = m_dialogsToRender.begin();
	for(; it!=m_dialogsToRender.end();++it)
		if((*it)->IsShown())
			(*it)->Draw();
*/
	if (pUIGame) pUIGame->Render	();

	//----------
	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor)
	{
		//Draw main window and its children
		if(m_bShowIndicators)
			UIMainIngameWnd->D raw();
		else
			UIMainIngameWnd->DrawPdaMessages();
		//render cursor only when it visible
		if(GetUICursor()->IsVisible())
            GetUICursor()->Render();
	}	

	return false;
}
bool	CUI::IR_OnMouseWheel			(int direction)
{
	if ( MainInputReceiver()&&MainInputReceiver()->IR_OnMouseWheel(direction)) 
		return true;

	if (pUIGame&&pUIGame->IR_OnMouseWheel(direction)) 
		return true;

	return false;
}

//--------------------------------------------------------------------
bool CUI::IR_OnKeyboardPress(int dik)
{
	if(UIMainIngameWnd->OnKeyboardPress(dik))
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

void CUI::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{

	if( pDialog->IsShown() ){
		
		if( bDoHideIndicators && pDialog==MainInputReceiver() ){
			ShowIndicators();
			if(m_bCrosshair) 
				psHUD_Flags.set(HUD_CROSSHAIR_RT, TRUE);
		};

	}else{
		if(bDoHideIndicators){
			HideIndicators();
			m_bCrosshair = !!psHUD_Flags.is(HUD_CROSSHAIR|HUD_CROSSHAIR_RT);
			if(m_bCrosshair) 
				psHUD_Flags.set(HUD_CROSSHAIR_RT, FALSE);
		}
	}

	CDialogHolder::StartStopMenu(pDialog,bDoHideIndicators);
}
