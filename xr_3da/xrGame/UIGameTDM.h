#ifndef __XR_UIGAMETDM_H__
#define __XR_UIGAMETDM_H__
#pragma once

#include "UIGameCustom.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"
#include "ui/UISpawnWnd.h"

// refs 
class CUITDMFragList;
class CUITDMPlayerList;

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
public:
	CUIGameTDM			(CUI* parent);
	virtual 			~CUIGameTDM			();

	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	static void			OnSelectTeamCallback	(int ButtonID);

	/////////
	CUIInventoryWnd		InventoryMenu;
	CUIMapWnd			MapMenu;
};
#endif // __XR_UIGAMETDM_H__
