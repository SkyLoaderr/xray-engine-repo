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
class CUISkinSelectorWnd;

class CUIGameTDM: public CUIGameDM
{
private:
	typedef CUIGameDM inherited;
protected:
	CUISpawnWnd*		pUITeamSelectWnd;

	CUIBuyWeaponWnd*	pBuyMenuTeam1;
	CUIBuyWeaponWnd*	pBuyMenuTeam2;

	CUISkinSelectorWnd*	pSkinMenuTeam1;
	CUISkinSelectorWnd*	pSkinMenuTeam2;

	virtual s16			ModifyTeam			(s16 Team)	{return Team-1;};
public:
	CUIGameTDM			(CUI* parent);
	virtual 			~CUIGameTDM			();

	virtual	void		Init				();
	virtual	void		SetCurrentBuyMenu	();
	virtual	void		SetCurrentSkinMenu	();

	virtual bool		IR_OnKeyboardPress		(int dik);

	virtual void		OnTeamSelect			(int Result);
protected:
	/////////
	CUIInventoryWnd		InventoryMenu;
	CUIMapWnd			MapMenu;
};
#endif // __XR_UIGAMETDM_H__
