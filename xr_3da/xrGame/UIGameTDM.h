#ifndef __XR_UIGAMETDM_H__
#define __XR_UIGAMETDM_H__
#pragma once

#include "UIGameCustom.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"
#include "ui/UISpawnWnd.h"

#include "ui/UIBuyWeaponWnd.h"

// refs 
class CUITDMFragList;
class CUITDMPlayerList;
class CUIBuyWeaponWnd;

class CUIGameTDM: public CUIGameCustom
{
private:
	typedef CUIGameCustom inherited;
protected:
	CUITDMFragList*		pFragListT1;
	CUITDMFragList*		pFragListT2;
	CUITDMPlayerList*	pPlayerListT1;
	CUITDMPlayerList*	pPlayerListT2;

	CUISpawnWnd*		pUITeamSelectWnd;

	CUIBuyWeaponWnd*	pBuyMenu;
public:
	CUIGameTDM			(CUI* parent);
	virtual 			~CUIGameTDM			();

	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	virtual void		OnTeamSelect			(int Result);
	virtual void		OnBuyMenu_Ok			();
protected:
	void				InitBuyMenu				(s16 Team = -1);
	/////////
	CUIInventoryWnd		InventoryMenu;
	CUIMapWnd			MapMenu;
};
#endif // __XR_UIGAMETDM_H__
