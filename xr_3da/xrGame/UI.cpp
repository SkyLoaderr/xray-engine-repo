#include "stdafx.h"
#include "UI.h"
#include "../xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "UIGameSP.h"
#include "actor.h"
#include "level.h"
#include "game_cl_base.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIMessagesWindow.h"
#include "ui/UIPdaWnd.h"

CUI::CUI(CHUDManager* p)
{
	UIMainIngameWnd					= xr_new<CUIMainIngameWnd>	();
	UIMainIngameWnd->Init			();
	m_pMessagesWnd					= xr_new<CUIMessagesWindow>();

	m_Parent						= p;
	pUIGame							= 0;

	ShowIndicators					();
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

void CUI::UIOnFrame()
{
	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor){
		
		//update windows
		if( IndicatorsShown() )
		{
			UIMainIngameWnd->SetFont(m_Parent->Font().pFontMedium);
			UIMainIngameWnd->Update	();
		}
	}

	// out GAME-style depend information
	if( IndicatorsShown() )
	{
		if (pUIGame) pUIGame->OnFrame	();
	}
	m_pMessagesWnd->Update();
}
//--------------------------------------------------------------------

bool CUI::Render()
{
	if( IndicatorsShown() )
	{
		if (pUIGame) 
			pUIGame->Render	();
	}

	CEntity* m_Actor = smart_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor)
	{
		//Draw main window and its children
		if( IndicatorsShown() )
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
	CDialogHolder::StartStopMenu(pDialog,bDoHideIndicators);
}

void CUI::ShowIndicators()
{
	m_bShowIndicators	= true;
	psHUD_Flags.set		(HUD_CROSSHAIR_RT, TRUE);
}

void CUI::HideIndicators()					
{
	m_bShowIndicators	= false;
	psHUD_Flags.set		(HUD_CROSSHAIR_RT, FALSE);
}
