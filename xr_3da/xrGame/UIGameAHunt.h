#ifndef __XR_UIGAMEAHUNT_H__
#define __XR_UIGAMEAHUNT_H__
#pragma once

#include "UIGameCustom.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"
#include "ui/UISpawnWnd.h"

#include "ui/UIBuyWeaponWnd.h"

// refs 
class CUIAHuntFragList;
class CUIAHuntPlayerList;
class CUIBuyWeaponWnd;

class CUIGameAHunt: public CUIGameCustom
{
private:
	typedef CUIGameCustom inherited;
protected:
	CUIAHuntFragList*		pFragListT1;
	CUIAHuntFragList*		pFragListT2;
	CUIAHuntPlayerList*		pPlayerListT1;
	CUIAHuntPlayerList*		pPlayerListT2;

	CUISpawnWnd*		pUITeamSelectWnd;

	CUIBuyWeaponWnd*	pBuyMenu;
public:
	CUIGameAHunt		(CUI* parent);
	virtual 			~CUIGameAHunt			();

	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	virtual void		OnTeamSelect			(int Result);
	virtual void		OnBuyMenu_Ok			();

	virtual bool		CanBeReady				();
protected:
	void				InitBuyMenu				(s16 Team = -1);
	/////////
	CUIInventoryWnd		InventoryMenu;
	CUIMapWnd			MapMenu;
};
#endif // __XR_UIGAMEAHUNT_H__
