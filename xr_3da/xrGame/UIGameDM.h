#ifndef __XR_UIGAMEDM_H__
#define __XR_UIGAMEDM_H__
#pragma once

#include "UIGameCustom.h"


#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"

#include "ui/UIBuyWeaponWnd.h"

// refs 
class CUIDMFragList;
class CUIDMPlayerList;
class CUIBuyWeaponWnd;

class CUIGameDM: public CUIGameCustom
{
private:
	typedef CUIGameCustom inherited;
protected:
	CUIDMFragList*		pFragList;
	CUIDMPlayerList*	pPlayerList;
	CUIBuyWeaponWnd*	pBuyMenu;
public:
						CUIGameDM			(CUI* parent);
	virtual 			~CUIGameDM			();
	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	virtual void		OnBuyMenu_Ok			();

	virtual CUIDialogWnd*	GetBuyWnd			()	{ return (CUIDialogWnd*) pBuyMenu; };

	/////////
	CUIInventoryWnd		InventoryMenu;
//	CUIBuyWeaponWnd		UIBuyWeaponWnd;
//	CUIStatsWnd*		pUIStatsWnd;
};
#endif // __XR_UIGAMEDM_H__
