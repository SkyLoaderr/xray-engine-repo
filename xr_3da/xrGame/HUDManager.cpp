// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDManager.h"
#include "hudcursor.h"
#include "actor.h"

//--------------------------------------------------------------------
CHUDManager::CHUDManager()
{ 
	Level().pHUD	= this;
	pGameFont		= new CFontGame;
	pSmallFont		= new CFontSmall;
	pHUDFont		= new CFontHUD;
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
	_DELETE			(pGameFont);
	_DELETE			(pSmallFont);
	_DELETE			(pHUDFont);
}
//--------------------------------------------------------------------

void CHUDManager::ClientToScreenScaled(float left, float top, Fvector2& dest, DWORD align){
	dest.set(ClientToScreenScaledX(left,align),	ClientToScreenScaledY(top,align));
}

float CHUDManager::ClientToScreenScaledX(float left, DWORD align){
	if (align&alRight)	return Device.dwWidth-UI_BASE_WIDTH*fScale + left*fScale;
	else				return left*fScale;
}

float CHUDManager::ClientToScreenScaledY(float top, DWORD align){
	if (align&alBottom)	return Device.dwHeight-UI_BASE_HEIGHT*fScale + top*fScale;
	else				return top*fScale;
}

void CHUDManager::ClientToScreen(float left, float top, Fvector2& dest, DWORD align){
	dest.set(ClientToScreenX(left,align),	ClientToScreenY(top,align));
}

float CHUDManager::ClientToScreenX(float left, DWORD align){
	if (align&alRight)	return Device.dwWidth-UI_BASE_WIDTH*fScale + left;
	else				return left;
}

float CHUDManager::ClientToScreenY(float top, DWORD align){
	if (align&alBottom)	return Device.dwHeight-UI_BASE_HEIGHT*fScale + top;
	else				return top;
}

void CHUDManager::Load()
{
	_DELETE			(pUI);
	pUI				= new CUI(this);
}
//--------------------------------------------------------------------
void CHUDManager::OnMove()
{
	if (pUI) pUI->OnMove();
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
	if (A && A->HUDview())			return;

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
		
		// font render
		pSmallFont->OnRender	();
		pGameFont->OnRender		();

		// UI
		bAlready = ! (pUI && !pUI->Render());
		pHUDFont->OnRender		();
	}
	if ((psHUD_Flags&HUD_CROSSHAIR) && !bAlready)	HUDCursor.Render();
}

//--------------------------------------------------------------------
void CHUDManager::OnEvent(EVENT E, DWORD P1, DWORD P2)
{
}
//--------------------------------------------------------------------

void CHUDManager::UIActivate		()
{
	if (pUI) pUI->Activate		();
}
//--------------------------------------------------------------------

void CHUDManager::UIDeactivate	()
{
	if (pUI) pUI->Deactivate	();
}
//--------------------------------------------------------------------

void CHUDManager::SetScale(float s){
	fScale			= s;
	pHUDFont->SetScale(fScale);
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
