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
	// Models
	struct Model {
		FBasicVisual*		V;
		Fmatrix				M;
		int					iLightLevel;
	};
	svector<Model,8>		Models;

	float				fScale;
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
	
	virtual		void		Render				();
	virtual		void		OnMove				();

	virtual		IC CUI*		GetUI				(){return pUI;}
	virtual		void		UIActivate			();
	virtual		void		UIDeactivate		();
	virtual		BOOL		IsUIActive			(){return pUI->IsActive();}

	virtual		void		Hit					(int idx){HitMarker.Hit(idx);}
	virtual		void		RenderModel			(FBasicVisual* V, Fmatrix& M, int iLightLevel)
	{
		if (Models.size()<8) {
			Models.last().V = V;
			Models.last().M.set(M);
			Models.last().iLightLevel = iLightLevel;
			Models.inc();
		}
	}

	virtual		void		OnDeviceCreate		();
	virtual		void		OnDeviceDestroy		();

	float					GetScale			(){return fScale;}
	void					SetScale			(float s);

	void					ClientToScreenScaled(float left, float top, Fvector2& dest, DWORD align);
	float					ClientToScreenScaledX(float left, DWORD align);
	float					ClientToScreenScaledY(float top, DWORD align);
	void					ClientToScreen		(float left, float top, Fvector2& dest, DWORD align);
	float					ClientToScreenX		(float left, DWORD align);
	float					ClientToScreenY		(float top, DWORD align);
	
	void __cdecl 			outMessage			(DWORD C, LPCSTR from, LPCSTR msg, ...);
};

#endif // __XR_HUDMANAGER_H__
