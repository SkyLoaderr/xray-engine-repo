// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\render.h"
#include "HUDManager.h"
#include "hudcursor.h"

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
}

void CHUDManager::Render_Affected()
{
	if (pUI) pCreator->CurrentViewEntity()->OnHUDDraw	(this);
	
	// HUD model
	if (psHUD_Flags&HUD_WEAPON) {
		Fmatrix Pold			= Device.mProject;
		Fmatrix FTold			= Device.mFullTransform;
		float aspect			= float(Device.dwHeight)/float(Device.dwWidth);
		Device.mProject.build_projection(
			deg2rad(psHUD_FOV*Device.fFOV*aspect), 
			aspect, VIEWPORT_NEAR, 
			pCreator->Environment.Current.Far);
		Device.mFullTransform.mul(Device.mProject, Device.mView);
		Device.set_xform_project(Device.mProject);
		
		// Sort shaders 
		for (DWORD i=0; i<Models.size(); i++) 
		{
			Model& M = Models[i];
			::Render->set_Transform		(&M.M);
			::Render->add_Visual		(M.V);
		}
		
		// Render with ZB hack :)
		::Render->rmNear		();
		::Render->flush			();
		::Render->rmNormal		();
		
		Device.mProject			= Pold;
		Device.mFullTransform	= FTold;
		Device.set_xform_project(Device.mProject);
	}
	Models.clear	();
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
