#ifndef __XR_UI_H__
#define __XR_UI_H__
#pragma once

#include "UICursor.h"
#include "UIZoneMap.h"
#include "UIWeapon.h"
#include "UIHealth.h"
#include "UIGroup.h"
#include "UIGameCustom.h"

#define UI_BASE_WIDTH	800
#define UI_BASE_HEIGHT	600

// refs
class CHUDManager;
class CWeapon;
#define MAX_GROUPS		10
#define MAX_UIMESSAGES	7
#define LIFE_TIME		7.f		// sec
#define HIDE_TIME		2.5f	// sec 

struct SUIMessage
{
	LPSTR sender;
	LPSTR msg;
	u32 color;
	int life_time;
	SUIMessage(LPCSTR S, LPCSTR M, u32 clr, float lt)
	{
		sender		= xr_strdup(S);
		msg			= xr_strdup(M);
		life_time	= iFloor(lt*1000);
		color		= clr;
	}
	~SUIMessage()
	{
		_FREE(sender);
		_FREE(msg);
	}
};

DEFINE_SVECTOR(SUIMessage*,MAX_UIMESSAGES,UIMsgSVec,UIMsgIt);

class CUI{
	CUICursor			UICursor;
	CUIZoneMap			UIZoneMap;
	CUIWeapon			UIWeapon;
	CUIHealth			UIHealth;
	CUISquad			UISquad;

	CUIGameCustom*		pUIGame;

	CHUDManager*		m_Parent;

	// messages
	float				msgs_offs;
	float				menu_offs;
	UIMsgSVec			messages;
public:
						CUI					(CHUDManager* p);
	virtual				~CUI				();

	bool				Render				();
	void				OnFrame				();

	void				Load				();

	bool				OnKeyboardPress		(int dik);
	bool				OnKeyboardRelease	(int dik);

	CUIGameCustom*		UIGame				(){return pUIGame;}
	// --- depends on game type
	// frag		(g_fraglimit)
	// time		(g_timelimit)
	// frag-list.....

	void				AddMessage			(LPCSTR S, LPCSTR M, u32 C=0xffffffff, float life_time=LIFE_TIME);
};

#endif // __XR_UI_H__
