#ifndef __XR_UIGAMETDM_H__
#define __XR_UIGAMETDM_H__
#pragma once

#include "UIGameCustom.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"

// refs 
class CUITDMFragList;
class CUITDMPlayerList;

class CUIGameTDM: public CUIGameCustom
{
private:
	typedef CUIGameCustom inherited;
protected:
	CUITDMFragList*		pFragList;
	CUITDMPlayerList*	pPlayerList;
public:
	CUIGameTDM			(CUI* parent);
	virtual 			~CUIGameTDM			();

	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	/////////
	CUIInventoryWnd		InventoryMenu;
	CUIMapWnd			MapMenu;
};
#endif // __XR_UIGAMETDM_H__
