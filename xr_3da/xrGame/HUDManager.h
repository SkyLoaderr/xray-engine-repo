#ifndef __XR_HUDMANAGER_H__
#define __XR_HUDMANAGER_H__
#pragma once

#include "..\CustomHUD.h"
#include "HitMarker.h"
#include "UI.h"
#include "HUDCursor.h"

// refs
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
	CGameFont*				pFontSmall;
	CGameFont*				pFontMedium;
	CGameFont*				pFontDI;
public:
							CHUDManager			();
	virtual					~CHUDManager		();
	virtual		void		OnEvent				(EVENT E, u32 P1, u32 P2);

	virtual		void		Load				();
	
	virtual		void		Render_Calcualte	();
	virtual		void		Render_Affected		();
	virtual		void		Render_Direct		();
	virtual		void		OnFrame				();

	virtual		IC CUI*		GetUI				(){return pUI;}

	virtual		void		Hit					(int idx){HitMarker.Hit(idx);}

	virtual		void		OnDeviceCreate		();
	virtual		void		OnDeviceDestroy		();

	float					GetScale			(){return fScale;}
	void					SetScale			(float s);

	void					ClientToScreenScaled(Irect& r, u32 align);
	void					ClientToScreenScaled(Ivector2& dest, int left, int top, u32 align);
	int						ClientToScreenScaledX(int left, u32 align);
	int						ClientToScreenScaledY(int top, u32 align);
	void					ClientToScreen		(Irect& r, u32 align);
	void					ClientToScreen		(Ivector2& dest, int left, int top, u32 align);
	int						ClientToScreenX		(int left, u32 align);
	int						ClientToScreenY		(int top, u32 align);
	
	void __cdecl 			outMessage			(u32 C, LPCSTR from, LPCSTR msg, ...);
};

#endif // __XR_HUDMANAGER_H__
