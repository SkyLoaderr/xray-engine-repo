#ifndef __XR_HUDMANAGER_H__
#define __XR_HUDMANAGER_H__
#pragma once

#include "..\CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"
#include "HUDCursor.h"

// refs
class CFontGame;
class CFontSmall;
class CContextMenu;
class CHUDCursor;

class CHUDManager :
	public CCustomHUD,
	public pureDeviceCreate, 
	public pureDeviceDestroy
{
	friend class CUI;
private:
	// ui
	CUI*					pUI;
	// hit marker
	CHitMarker				HitMarker;
	// hud cursor
	CHUDCursor				HUDCursor;

	float					fScale;
public:
	// hud font
	CFontGame*				pGameFont;
	CFontSmall*				pSmallFont;
	CFontHUD*				pHUDFont;
public:
							CHUDManager			();
	virtual					~CHUDManager		();
	virtual		void		OnEvent				(EVENT E, DWORD P1, DWORD P2);

	virtual		void		Load				();
	
	virtual		void		Render_Calcualte	();
	virtual		void		Render_Affected		();
	virtual		void		Render_Direct		();
	virtual		void		OnFrame				();

	virtual		IC CUI*		GetUI				(){return pUI;}
	virtual		void		UIActivate			();
	virtual		void		UIDeactivate		();
	virtual		BOOL		IsUIActive			(){return pUI->IsActive();}

	virtual		void		Hit					(int idx){HitMarker.Hit(idx);}

	virtual		void		OnDeviceCreate		();
	virtual		void		OnDeviceDestroy		();

	float					GetScale			(){return fScale;}
	void					SetScale			(float s);

	void					ClientToScreenScaled(Fvector2& dest, float left, float top, DWORD align);
	float					ClientToScreenScaledX(float left, DWORD align);
	float					ClientToScreenScaledY(float top, DWORD align);
	void					ClientToScreen		(Fvector2& dest, float left, float top, DWORD align);
	float					ClientToScreenX		(float left, DWORD align);
	float					ClientToScreenY		(float top, DWORD align);
	
	void __cdecl 			outMessage			(DWORD C, LPCSTR from, LPCSTR msg, ...);
};

#endif // __XR_HUDMANAGER_H__
