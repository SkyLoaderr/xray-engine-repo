#ifndef __XR_UI_H__
#define __XR_UI_H__
#pragma once

#include "UICursor.h"
#include "UIZoneMap.h"
#include "UIWeapon.h"
#include "UIHealth.h"
#include "UIGroup.h"
#include "UIGameCustom.h"



///////////////////////////////
// включает менюшку вместо игры
////////////////////////////////

//#define UI_INTERFACE_ON





#ifdef UI_INTERFACE_ON

#include "ui\UIFrameWindow.h"
#include "ui\UIButton.h"
#include "ui\UICheckButton.h"

#include "ui\UIRadioButton.h"
#include "ui\UIRadioGroup.h"

#include "ui\UIScrollBar.h"

#include "ui\UIMessageBox.h"
#include "ui\UIProgressBar.h"
#include "ui\UIStatic.h"

#include "ui\UIDragDropItem.h"
#include "ui\UIDragDropList.h"

#include "ui\UIInventoryWnd.h"

#endif


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
		xr_free(sender);
		xr_free(msg);
	}
};

DEFINE_SVECTOR(SUIMessage*,MAX_UIMESSAGES,UIMsgSVec,UIMsgIt);

class CUI{
	CUICursor			UICursor;
	CUIZoneMap			UIZoneMap;
	CUIWeapon			UIWeapon;
	CUIHealth			UIHealth;
	CUISquad			UISquad;


	//////////////////////////////////
	// window tests begin
	///////////////////////////////////

#ifdef UI_INTERFACE_ON

	//CUIFrameWindow		UIMainWindow;

	CUIInventoryWnd		UIMainWindow;

	CUIButton			UIButton1;
	CUIButton			UIButton2;
	CUIButton			UIButton3;

	CUIRadioGroup		UIRadioGroup;
	CUIRadioButton		UIRadioButton1;
	CUIRadioButton		UIRadioButton2;

	CUIScrollBar		UIScrollBar;

	CUIMessageBox		UIMessageBox; 
	CUIMessageBox		UIMessageBox1;
	CUIMessageBox		UIMessageBox2;

	CUIProgressBar		UIProgressBar; 
	
	CUIStatic			UIStatic;
	
	CUIDragDropItem		UIDragDropItem;
	CUIDragDropItem		UIDragDropItem1;
	CUIDragDropItem		UIDragDropItem2;
	CUIDragDropItem		UIDragDropItem3;
	CUIDragDropItem		UIDragDropItem4;
	CUIDragDropItem		UIDragDropItem5;
	CUIDragDropItem		UIDragDropItem6;

	CUIDragDropList		UIDragDropList; 
	CUIDragDropList		UIDragDropList1; 
	CUIDragDropList		UIDragDropList2;



#endif

	//////////////////////////////////
	// window tests end
	///////////////////////////////////




	CUIGameCustom*		pUIGame;

	// messages
	float				msgs_offs;
	float				menu_offs;
	UIMsgSVec			messages;
public:
	CHUDManager*		m_Parent;
public:
						CUI					(CHUDManager* p);
	virtual				~CUI				();

	bool				Render				();
	void				OnFrame				();

	void				Load				();

	bool				OnKeyboardPress		(int dik);
	bool				IR_OnKeyboardRelease	(int dik);
	bool				IR_OnMouseMove			(int,int);

	CUIGameCustom*		UIGame				(){return pUIGame;}
	// --- depends on game type
	// frag		(g_fraglimit)
	// time		(g_timelimit)
	// frag-list.....

	void				AddMessage			(LPCSTR S, LPCSTR M, u32 C=0xffffffff, float life_time=LIFE_TIME);
};

#endif // __XR_UI_H__

