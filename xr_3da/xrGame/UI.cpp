#include "stdafx.h"
#include "UI.h"
#include "../xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "UIGameSP.h"
//.#include "UIGameDM.h"
//.#include "UIGameTDM.h"
//.#include "UIGameAHunt.h"
#include "actor.h"
#include "level.h"
#include "game_cl_base.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIMessagesWindow.h"
#include "ui/UIPdaWnd.h"

CUI::CUI(CHUDManager* p)
{
	UIMainIngameWnd	= xr_new<CUIMainIngameWnd>	();
	UIMainIngameWnd->Init						();
	m_pMessagesWnd = xr_new<CUIMessagesWindow>();

	m_Parent		= p;
	pUIGame			= 0;

	m_bShowIndicators = true;

}
//--------------------------------------------------------------------

CUI::~CUI()
{
	xr_delete		(m_pMessagesWnd);
	xr_delete		(pUIGame);
	xr_delete		(UIMainIngameWnd);
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
	if(m_bShowIndicators)
	{
		if (pUIGame) pUIGame->OnFrame	();
	}
	m_pMessagesWnd->Update();
}
//--------------------------------------------------------------------

bool CUI::Render()
{
	if(m_bShowIndicators)
	{
		if (pUIGame) 
			pUIGame->Render	();
	}

	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor)
	{
		//Draw main window and its children
		if(m_bShowIndicators)
		{
			UIMainIngameWnd->Draw();
			m_pMessagesWnd->Draw();
		}
		else{
			CUIGameSP* gSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
			if (gSP){
				if (!gSP->PdaMenu->GetVisible())
					m_pMessagesWnd->Draw();
			}
			else
				m_pMessagesWnd->Draw();

		}
	}
	else
		m_pMessagesWnd->Draw();
	DoRenderDialogs();
//	if(GetUICursor()->IsVisible())
//        GetUICursor()->Render();
	return false;
}
bool	CUI::IR_OnMouseWheel			(int direction)
{
	if ( MainInputReceiver() ){
		if( MainInputReceiver()->IR_OnMouseWheel(direction) )
			return true;
	}

	if (pUIGame&&pUIGame->IR_OnMouseWheel(direction)) 
		return true;

	if( MainInputReceiver() )
		return true;

	return false;
}

//--------------------------------------------------------------------
bool CUI::IR_OnKeyboardHold(int dik)
{
	if ( MainInputReceiver() ) {
		if( MainInputReceiver()->IR_OnKeyboardHold(dik) )
			return true;
	}

	if( MainInputReceiver() )
		return true;

	return false;
}


bool CUI::IR_OnKeyboardPress(int dik)
{

	if ( MainInputReceiver() ) {
		if( MainInputReceiver()->IR_OnKeyboardPress(dik) )
			return true;
	}

	if(UIMainIngameWnd->OnKeyboardPress(dik))
		return true;

	if (pUIGame && pUIGame->IR_OnKeyboardPress(dik)) 
		return true;

	if( MainInputReceiver() )
		return true;

	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnKeyboardRelease(int dik)
{
	if ( MainInputReceiver() ){
		if( MainInputReceiver()->IR_OnKeyboardRelease(dik) )
			return true;
	}

	if (pUIGame&&pUIGame->IR_OnKeyboardRelease(dik)) 
		return true;

	if( MainInputReceiver() )
		return true;

	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnMouseMove(int dx,int dy)
{
	if ( MainInputReceiver() ){
		if ( MainInputReceiver()->IR_OnMouseMove(dx,dy) )
			return true;
	}
	
	if (pUIGame&&pUIGame->IR_OnMouseMove(dx,dy)) 
		return true;

	if( MainInputReceiver() )
		return true;
	
	return false;
}

void CUI::AddInfoMessage			(LPCSTR message)
{
	UIMainIngameWnd->AddInfoMessage	(message);
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

CDeviceResetNotifier::CDeviceResetNotifier	()
{
	Device.seqDeviceReset.Add(this,REG_PRIORITY_NORMAL);
}

CDeviceResetNotifier::~CDeviceResetNotifier	()
{
	Device.seqDeviceReset.Remove(this);
}


