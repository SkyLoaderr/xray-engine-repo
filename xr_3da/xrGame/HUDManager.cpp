// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDManager.h"
#include "hudcursor.h"

#include "actor.h"
#include "car.h"
#include "spectator.h"
#include "../igame_level.h"

//--------------------------------------------------------------------
CHUDManager::CHUDManager()
{ 
#pragma todo("Yura, what is this and why? Oles.")
	// Level().pHUD	= this;
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
	
	pUI						= 0;
	if (Device.bReady) OnDeviceCreate();

	m_pHUDCursor			= xr_new<CHUDCursor>();
}
//--------------------------------------------------------------------
CHUDManager::~CHUDManager()
{
	xr_delete			(pUI);
	xr_delete			(pFontBigDigit);
	xr_delete			(pFontSmall);
	xr_delete			(pFontMedium);
	xr_delete			(pFontDI);

	xr_delete			(m_pHUDCursor);
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
	if (!psHUD_Flags.test(HUD_WEAPON))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= smart_cast<CActor*> (O);
	if (A && !A->HUDview())			return;
	CCar*		C					= smart_cast<CCar*>	(O);
	if (C)							return;
	CSpectator*	S					= smart_cast<CSpectator*>	(O);
	if (S)							return;

	// hud itself
	::Render->set_HUD				(TRUE);
	::Render->set_Object			(O->H_Root());
	O->OnHUDDraw					(this);
	::Render->set_HUD				(FALSE);
}

void CHUDManager::Render_Direct	()
{
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

		//render UI cursor
		if(pUI && pUI->GetCursor() && pUI->GetCursor()->IsVisible())
			pUI->GetCursor()->Render();
	}
	if (psHUD_Flags.test(HUD_CROSSHAIR) && !bAlready)	
		m_pHUDCursor->Render();

	// Recalc new scale factor if resolution was changed
	OnDeviceCreate();
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

Collide::rq_result&	CHUDManager::GetCurrentRayQuery	() 
{
	return m_pHUDCursor->RQ;
}

void CHUDManager::SetCrosshairDisp	(float disp)
{
	m_pHUDCursor->HUDCrosshair.SetDispersion(disp);
}

void  CHUDManager::ShowCrosshair	(bool show)
{
	m_pHUDCursor->m_bShowCrosshair = show;
}

//////////////////////////////////////////////////////////////////////////

void CHUDManager::OutText(CGameFont *pFont, Irect r, float x, float y, LPCSTR fmt, ...)
{
	if (r.in(static_cast<int>(x), static_cast<int>(y)))
	{
		R_ASSERT(pFont);
		va_list	lst;
		static string512 buf;
		::ZeroMemory(buf, 512);
		std::string str;

		va_start(lst, fmt);
			vsprintf(buf, fmt, lst);
			str += buf;
		va_end(lst);

		// Rescale position in lower resolution
		if (x >= 1.0f && y >= 1.0f)
		{
			x *= GetScale();
			y *= GetScale();
		}

		pFont->Out(x, y, str.c_str());
	}
}
