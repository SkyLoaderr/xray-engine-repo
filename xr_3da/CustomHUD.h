#ifndef __XR_CUSTOMHUD_H__
#define __XR_CUSTOMHUD_H__
#pragma once

#include "fcontroller.h"

ENGINE_API extern DWORD psHUD_Flags;
#define HUD_CROSSHAIR			(1<<0)
#define HUD_CROSSHAIR_DIST		(1<<1)
#define HUD_WEAPON				(1<<2)
#define HUD_INFO				(1<<3)
#define HUD_DRAW				(1<<4)

class ENGINE_API CVisual;
class CUI;

class ENGINE_API CCustomHUD:
	public DLL_Pure,
	public CEventBase	
{
public:
					CCustomHUD				();
	virtual			~CCustomHUD				();

	virtual		void		Load					(){;}
	
	virtual		void		Render_Calcualte		() {;}
	virtual		void		Render_Affected			() {;}
	virtual		void		Render_Direct			() {;}
	
	virtual		void		OnFrame					(){;}
	virtual		void		OnEvent					(EVENT E, DWORD P1, DWORD P2){;}

	virtual		void		Hit						(int idx){;}

	virtual		void		UIActivate				()=0;
	virtual		void		UIDeactivate			()=0;
	virtual		BOOL		IsUIActive				()=0;

	virtual IC CUI*	GetUI					()=0;
};

#endif //__XR_CUSTOMHUD_H__
