// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDManager.h"
#include "hudcursor.h"

#include "actor.h"
#include "car.h"
#include "spectator.h"

//--------------------------------------------------------------------
CHUDManager::CHUDManager()
{ 
	Level().pHUD	= this;
	pFontSmall		= xr_new<CGameFont> ("hud_font_small");
	pFontMedium		= xr_new<CGameFont> ("hud_font_medium");
	pFontDI			= xr_new<CGameFont> ("hud_font_di",CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	pFontBigDigit	= xr_new<CGameFont> ("hud_font_big_digit");

	pFontHeaderEurope = xr_new<CGameFont> ("ui_font_header_europe");
	pFontHeaderRussian = xr_new<CGameFont> ("ui_font_header_russian");
	

	pUI				= 0;
	Device.seqDevCreate.Add	(this);
	if (Device.bReady) OnDeviceCreate();
}
//--------------------------------------------------------------------

CHUDManager::~CHUDManager()
{
	Device.seqDevCreate.Remove(this);
	xr_delete			(pUI);
	xr_delete			(pFontBigDigit);
	xr_delete			(pFontSmall);
	xr_delete			(pFontMedium);
	xr_delete			(pFontDI);

	xr_delete			(pFontHeaderEurope);
	xr_delete			(pFontHeaderRussian);
}
//--------------------------------------------------------------------
void CHUDManager::ClientToScreenScaled(Irect& r, u32 align)
{
	r.x1 = ClientToScreenScaledX(r.x1,align); 
	r.y1 = ClientToScreenScaledY(r.y1,align); 
	r.x2 = ClientToScreenScaledX(r.x2,align); 
	r.y2 = ClientToScreenScaledY(r.y2,align); 
}

void CHUDManager::ClientToScreenScaled(Ivector2& dest, int left, int top, u32 align)
{
	dest.set(ClientToScreenScaledX(left,align),	ClientToScreenScaledY(top,align));
}

int CHUDManager::ClientToScreenScaledX(int left, u32 align)
{
	if (align&alRight)	return iFloor(Device.dwWidth-UI_BASE_WIDTH*fScale + left*fScale);
	else				return iFloor(left*fScale);
}

int CHUDManager::ClientToScreenScaledY(int top, u32 align)
{
	if (align&alBottom)	return iFloor(Device.dwHeight-UI_BASE_HEIGHT*fScale + top*fScale);
	else				return iFloor(top*fScale);
}

void CHUDManager::ClientToScreen(Ivector2& dest, int left, int top, u32 align)
{
	dest.set(ClientToScreenX(left,align),	ClientToScreenY(top,align));
}

void CHUDManager::ClientToScreen(Irect& r, u32 align)
{
	r.x1 = ClientToScreenX(r.x1,align); 
	r.y1 = ClientToScreenY(r.y1,align); 
	r.x2 = ClientToScreenX(r.x2,align); 
	r.y2 = ClientToScreenY(r.y2,align); 
}

int CHUDManager::ClientToScreenX(int left, u32 align)
{
	if (align&alRight)	return iFloor(Device.dwWidth-UI_BASE_WIDTH*fScale + left);
	else				return left;
}

int CHUDManager::ClientToScreenY(int top, u32 align)
{
	if (align&alBottom)	return iFloor(Device.dwHeight-UI_BASE_HEIGHT*fScale + top);
	else				return top;
}

void CHUDManager::Load()
{
	xr_delete			(pUI);
	pUI				= xr_new<CUI> (this);
	pUI->Load		();
}
//--------------------------------------------------------------------
void CHUDManager::OnFrame()
{
	if (pUI) pUI->OnFrame();
}
//--------------------------------------------------------------------

ENGINE_API extern float psHUD_FOV;

void CHUDManager::Render_First()
{
	if (!psHUD_Flags.test(HUD_WEAPON))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= dynamic_cast<CActor*> (O);
	if (A && !A->HUDview())			return;

	// only shadow 
	::Render->set_Invisible			(TRUE);
	::Render->set_Object			(O->H_Root());
	O->renderable_Render			();
	::Render->set_Invisible			(FALSE);
}

void CHUDManager::Render_Last()
{
	if (!psHUD_Flags.test(HUD_WEAPON))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= dynamic_cast<CActor*> (O);
	if (A && !A->HUDview())			return;
	CCar*		C					= dynamic_cast<CCar*>	(O);
	if (C)							return;
	CSpectator*	S					= dynamic_cast<CSpectator*>	(O);
	if (S)							return;

	::Render->set_HUD				(TRUE);
	::Render->set_Object			(O->H_Root());
	O->OnHUDDraw					(this);
	::Render->set_HUD				(FALSE);
}

void CHUDManager::Render_Direct	()
{
	BOOL bAlready = FALSE;
	if (psHUD_Flags.test(HUD_DRAW))
	{
		// draw hit marker
		HitMarker.Render();
		
		// UI
		bAlready = ! (pUI && !pUI->Render());
		pFontDI->OnRender		();
		pFontSmall->OnRender	();
		pFontMedium->OnRender	();
		pFontBigDigit->OnRender	();

		pFontHeaderEurope->OnRender	();
		pFontHeaderRussian->OnRender();

		//render UI cursor
		if(pUI && pUI->GetCursor() && pUI->GetCursor()->IsVisible())
			pUI->GetCursor()->Render();
	}
	if (psHUD_Flags.test(HUD_CROSSHAIR) && !bAlready)	HUDCursor.Render();
}

//--------------------------------------------------------------------
void CHUDManager::OnEvent(EVENT E, u64 P1, u64 P2)
{
}
//--------------------------------------------------------------------
void CHUDManager::SetScale(float s){
	fScale			= s;
}
void CHUDManager::OnDeviceCreate()
{
	if (Device.dwWidth<UI_BASE_WIDTH)	SetScale(float(Device.dwWidth)/float(UI_BASE_WIDTH));
	else								SetScale(1.f);
}
//--------------------------------------------------------------------
void __cdecl CHUDManager::outMessage(u32 C, LPCSTR from, LPCSTR msg, ...)
{
	char		buffer[128];

	va_list		p;
	va_start	(p,msg);
	vsprintf	(buffer,msg,p);
	R_ASSERT	(strlen(buffer)<128);
	va_end		(p);

	GetUI()->AddMessage	(from,buffer,C);
	Msg			("- %s: %s",from,buffer);
}
