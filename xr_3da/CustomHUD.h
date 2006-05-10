#pragma once

#include "iinputreceiver.h"

ENGINE_API extern Flags32		psHUD_Flags;
#define HUD_CROSSHAIR			(1<<0)
#define HUD_CROSSHAIR_DIST		(1<<1)
#define HUD_WEAPON				(1<<2)
#define HUD_INFO				(1<<3)
#define HUD_DRAW				(1<<4)
#define HUD_CROSSHAIR_RT		(1<<5)
#define HUD_WEAPON_RT			(1<<6)
#define HUD_CROSSHAIR_DYNAMIC	(1<<7)
#define HUD_CAM_ANIM_HIT		(1<<8)

class ENGINE_API IRender_Visual;
class CUI;

class ENGINE_API CCustomHUD:
	public DLL_Pure,
	public IEventReceiver	
{
public:
					CCustomHUD				();
	virtual			~CCustomHUD				();

	virtual		void		Load					(){;}
	
	virtual		void		Render_First			(){;}
	virtual		void		Render_Last				(){;}
	
	virtual		void		OnFrame					(){;}
	virtual		void		OnEvent					(EVENT E, u64 P1, u64 P2){;}

	virtual IC	CUI*		GetUI					()=0;
};
