#ifndef __XR_UIGAMETDM_H__
#define __XR_UIGAMETDM_H__
#pragma once

#include "UIGameCustom.h"
#include "UIGameDM.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"
#include "ui/UISpawnWnd.h"

#include "ui/UIBuyWeaponWnd.h"

// refs 

class CUITDMFragList;
class CUITDMPlayerList;
class CUIBuyWeaponWnd;

class CUIGameTDM: public CUIGameDM
{
private:
	typedef CUIGameDM inherited;
protected:
	/*
	CUITDMFragList*		pFragListT1;
	CUITDMFragList*		pFragListT2;
	CUITDMPlayerList*	pPlayerListT1;
	CUITDMPlayerList*	pPlayerListT2;


	CUIBuyWeaponWnd*	pBuyMenu;
	*/
	CUISpawnWnd*		pUITeamSelectWnd;

	virtual s16			ModifyTeam			(s16 Team)	{return Team-1;};
public:
	CUIGameTDM			(CUI* parent);
	virtual 			~CUIGameTDM			();

	virtual bool		IR_OnKeyboardPress		(int dik);

	virtual void		OnTeamSelect			(int Result);
/*
	virtual void		Render				();
	virtual void		OnFrame				();

	
	virtual bool		IR_OnKeyboardRelease	(int dik);

	

	virtual void		OnBuyMenu_Ok			();

	virtual bool		CanBeReady				();
	*/
protected:
	/////////
	CUIInventoryWnd		InventoryMenu;
	CUIMapWnd			MapMenu;
};
#endif // __XR_UIGAMETDM_H__
