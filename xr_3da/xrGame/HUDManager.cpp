// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDManager.h"
#include "hudcursor.h"

#include "actor.h"
//#include "car.h"
//#include "spectator.h"
#include "../igame_level.h"
#include "clsid_game.h"


CFontManager::CFontManager()
{
	pFontSmall				= xr_new<CGameFont> ("hud_font_small");
	pFontMedium				= xr_new<CGameFont> ("hud_font_medium");//,CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	pFontDI					= xr_new<CGameFont> ("hud_font_di",CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	pFontBigDigit			= xr_new<CGameFont> ("hud_font_big_digit");

	pFontHeaderEurope		= xr_new<CGameFont> ("ui_font_header_europe");
	pFontHeaderRussian		= xr_new<CGameFont> ("ui_font_header_russian");

	pArialN21Russian		= xr_new<CGameFont> ("ui_font_arial_n_21_russian");
	pFontGraffiti19Russian	= xr_new<CGameFont> ("ui_font_graffiti19_russian");
	pFontGraffiti22Russian	= xr_new<CGameFont> ("ui_font_graffiti22_russian");
	pFontLetterica16Russian = xr_new<CGameFont> ("ui_font_letterica16_russian");
	pFontLetterica18Russian = xr_new<CGameFont> ("ui_font_letterica18_russian");
	pFontGraffiti32Russian	= xr_new<CGameFont> ("ui_font_graff_32");
	pFontGraffiti50Russian	= xr_new<CGameFont> ("ui_font_graff_50");
	pFontLetterica25		= xr_new<CGameFont> ("ui_font_letter_25");
	
	pFontStat				= xr_new<CGameFont> ("stat_font");
}

CFontManager::~CFontManager()
{
	xr_delete			(pFontBigDigit);
	xr_delete			(pFontSmall);
	xr_delete			(pFontMedium);
	xr_delete			(pFontDI);

	xr_delete			(pFontHeaderEurope);
	xr_delete			(pFontHeaderRussian);

	xr_delete			(pArialN21Russian);
	xr_delete			(pFontGraffiti19Russian);
	xr_delete			(pFontGraffiti22Russian);
	xr_delete			(pFontLetterica16Russian);
	xr_delete			(pFontLetterica18Russian);
	xr_delete			(pFontGraffiti32Russian);
	xr_delete			(pFontGraffiti50Russian);
	xr_delete			(pFontLetterica25);

	xr_delete			(pFontStat);
}

void CFontManager::Render()
{
	pFontDI->OnRender			();
	pFontSmall->OnRender		();
	pFontMedium->OnRender		();
	pFontBigDigit->OnRender		();

	pFontHeaderEurope->OnRender	();
	pFontHeaderRussian->OnRender();

	pArialN21Russian->OnRender();
	pFontGraffiti19Russian->OnRender();
	pFontGraffiti22Russian->OnRender(); 
	pFontGraffiti32Russian->OnRender(); 
	pFontGraffiti50Russian->OnRender(); 
	pFontLetterica16Russian->OnRender();
	pFontLetterica18Russian->OnRender();
	pFontLetterica25->OnRender();

	pFontStat->OnRender			();
}
//--------------------------------------------------------------------
CHUDManager::CHUDManager()
{ 
#pragma todo("Yura, what is this and why? Oles.")
	// Level().pHUD	= this;
	
	pUI						= 0;
//	if (Device.bReady) OnDeviceCreate();

	m_pHUDCursor			= xr_new<CHUDCursor>();
}
//--------------------------------------------------------------------
CHUDManager::~CHUDManager()
{
	xr_delete			(pUI);
	xr_delete			(m_pHUDCursor);
}

//--------------------------------------------------------------------

void CHUDManager::Load()
{
	xr_delete			(pUI);
	pUI				= xr_new<CUI> (this);
	pUI->Load		();
}
//--------------------------------------------------------------------
void CHUDManager::OnFrame()
{
	if (pUI) pUI->UIOnFrame();
	m_pHUDCursor->CursorOnFrame();
}
//--------------------------------------------------------------------

ENGINE_API extern float psHUD_FOV;

void CHUDManager::Render_First()
{
	if (!psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= smart_cast<CActor*> (O);
	if (!A)							return;
	if (A && !A->HUDview())			return;

	// only shadow 
	::Render->set_Invisible			(TRUE);
	::Render->set_Object			(O->H_Root());
	O->renderable_Render			();
	::Render->set_Invisible			(FALSE);
}

void CHUDManager::Render_Last()
{
	if (!psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= smart_cast<CActor*> (O);
	if (A && !A->HUDview())			return;
//	CCar*		C					= smart_cast<CCar*>	(O);
//	if (C)							return;
	if(O->CLS_ID == CLSID_CAR)
		return;

//	CSpectator*	S					= smart_cast<CSpectator*>	(O);
//	if (S)							return;
	if(O->CLS_ID == CLSID_SPECTATOR)
		return;

	// hud itself
	::Render->set_HUD				(TRUE);
	::Render->set_Object			(O->H_Root());
	O->OnHUDDraw					(this);
	::Render->set_HUD				(FALSE);
}

//отрисовка элементов интерфейса
void  CHUDManager::RenderUI()
{
	BOOL bAlready					= FALSE;
	if (psHUD_Flags.test(HUD_DRAW))
	{
		// draw hit marker
		HitMarker.Render			();

		// UI
		bAlready					= ! (pUI && !pUI->Render());
		//Font
		Font().Render();

		//render UI cursor
		if(pUI && GetUICursor() && GetUICursor()->IsVisible())
			GetUICursor()->Render();
	}
	if (psHUD_Flags.is(HUD_CROSSHAIR|HUD_CROSSHAIR_RT) && !bAlready)	
		m_pHUDCursor->Render();

	// Recalc new scale factor if resolution was changed
//	OnDeviceCreate();
}

//--------------------------------------------------------------------
void CHUDManager::OnEvent(EVENT E, u64 P1, u64 P2)
{
}
//--------------------------------------------------------------------
void __cdecl CHUDManager::outMessage(u32 C, LPCSTR from, LPCSTR msg, ...)
{
	char		buffer	[256];

	va_list		p;
	va_start	(p,msg);
	vsprintf	(buffer,msg,p);
	R_ASSERT	(xr_strlen(buffer)<256);
	va_end		(p);

	GetUI()->AddMessage	(from,buffer,C);
	Msg			("- %s: %s",from,buffer);
}
void __cdecl CHUDManager::outMessage(u32 C, const shared_str& from, LPCSTR msg, ...)
{
	string256	buffer;

	va_list		p;
	va_start	(p,msg);
	_vsnprintf	(buffer,sizeof(buffer)-1,msg,p); buffer[sizeof(buffer)-1] = 0;
	va_end		(p);

	GetUI()->AddMessage	(*from,buffer,C);
	Msg			("- %s: %s",from,buffer);
}

collide::rq_result&	CHUDManager::GetCurrentRayQuery	() 
{
	return m_pHUDCursor->RQ;
}

BOOL	g_bDynamicCrosshair = TRUE;
void CHUDManager::SetCrosshairDisp	(float disp)
{	
	m_pHUDCursor->HUDCrosshair.SetDispersion(g_bDynamicCrosshair ? disp : 0.f);
}

void  CHUDManager::ShowCrosshair	(bool show)
{
	m_pHUDCursor->m_bShowCrosshair = show;
}

//////////////////////////////////////////////////////////////////////////

