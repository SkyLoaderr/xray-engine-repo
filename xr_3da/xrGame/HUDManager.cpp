// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDManager.h"
#include "hudcursor.h"

#include "actor.h"
#include "car.h"

//--------------------------------------------------------------------
CHUDManager::CHUDManager()
{ 
	Level().pHUD	= this;
	pHUDFont		= new CGameFont("font","ui\\ui_font_hud_03",256,16,0);
	pGameFont		= new CGameFont("font","ui\\ui_font_hud_02",256,16,CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	pUI				= 0;
	Device.seqDevCreate.Add	(this);
	Device.seqDevDestroy.Add(this);
	if (Device.bReady) OnDeviceCreate();
}
//--------------------------------------------------------------------

CHUDManager::~CHUDManager()
{
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	_DELETE			(pUI);
	_DELETE			(pHUDFont);
	_DELETE			(pGameFont);
}
//--------------------------------------------------------------------
void CHUDManager::ClientToScreenScaled(Irect& r, DWORD align)
{
	r.x1 = ClientToScreenScaledX(r.x1,align); 
	r.y1 = ClientToScreenScaledY(r.y1,align); 
	r.x2 = ClientToScreenScaledX(r.x2,align); 
	r.y2 = ClientToScreenScaledY(r.y2,align); 
}

void CHUDManager::ClientToScreenScaled(Ivector2& dest, int left, int top, DWORD align)
{
	dest.set(ClientToScreenScaledX(left,align),	ClientToScreenScaledY(top,align));
}

int CHUDManager::ClientToScreenScaledX(int left, DWORD align)
{
	if (align&alRight)	return iFloor(Device.dwWidth-UI_BASE_WIDTH*fScale + left*fScale);
	else				return iFloor(left*fScale);
}

int CHUDManager::ClientToScreenScaledY(int top, DWORD align)
{
	if (align&alBottom)	return iFloor(Device.dwHeight-UI_BASE_HEIGHT*fScale + top*fScale);
	else				return iFloor(top*fScale);
}

void CHUDManager::ClientToScreen(Ivector2& dest, int left, int top, DWORD align)
{
	dest.set(ClientToScreenX(left,align),	ClientToScreenY(top,align));
}

void CHUDManager::ClientToScreen(Irect& r, DWORD align)
{
	r.x1 = ClientToScreenX(r.x1,align); 
	r.y1 = ClientToScreenY(r.y1,align); 
	r.x2 = ClientToScreenX(r.x2,align); 
	r.y2 = ClientToScreenY(r.y2,align); 
}

int CHUDManager::ClientToScreenX(int left, DWORD align)
{
	if (align&alRight)	return iFloor(Device.dwWidth-UI_BASE_WIDTH*fScale + left);
	else				return left;
}

int CHUDManager::ClientToScreenY(int top, DWORD align)
{
	if (align&alBottom)	return iFloor(Device.dwHeight-UI_BASE_HEIGHT*fScale + top);
	else				return top;
}

void CHUDManager::Load()
{
	_DELETE			(pUI);
	pUI				= new CUI(this);
	pUI->Load		();
}
//--------------------------------------------------------------------
void CHUDManager::OnFrame()
{
	if (pUI) pUI->OnFrame();
}
//--------------------------------------------------------------------

ENGINE_API extern float psHUD_FOV;

void CHUDManager::Render_Calcualte()
{
	if (0==(psHUD_Flags&HUD_WEAPON))return;
	if (0==pUI)						return;
	CObject*	O					= pCreator->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= dynamic_cast<CActor*> (O);
	if (A && !A->HUDview())			return;
	CCar*		C					= dynamic_cast<CCar*>	(O);
	if (C)							return;

	::Render->set_HUD				(TRUE);
	::Render->set_Object			(O->H_Root());
	O->OnHUDDraw					(this);
	::Render->set_HUD				(FALSE);
}

void CHUDManager::Render_Affected()
{
}

void CHUDManager::Render_Direct	()
{
	BOOL bAlready = FALSE;
	if (psHUD_Flags&HUD_DRAW)
	{
		// draw hit marker
		HitMarker.Render();
		
		// UI
		bAlready = ! (pUI && !pUI->Render());
		pGameFont->OnRender		();
		pHUDFont->OnRender		();
	}
	if ((psHUD_Flags&HUD_CROSSHAIR) && !bAlready)	HUDCursor.Render();
}

//--------------------------------------------------------------------
void CHUDManager::OnEvent(EVENT E, DWORD P1, DWORD P2)
{
}
//--------------------------------------------------------------------

void CHUDManager::SetScale(float s){
	fScale			= s;
	pHUDFont->Scale	(fScale);
}

void CHUDManager::OnDeviceCreate()
{
	pUI				= new CUI(this);
	
	if (Device.dwWidth<UI_BASE_WIDTH)	SetScale(float(Device.dwWidth)/float(UI_BASE_WIDTH));
	else								SetScale(1.f);

}
//--------------------------------------------------------------------

void CHUDManager::OnDeviceDestroy()
{
	_DELETE		(pUI);
}
//--------------------------------------------------------------------
void __cdecl CHUDManager::outMessage(DWORD C, LPCSTR from, LPCSTR msg, ...)
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
