#pragma once
#include "uigamecustom.h"
#include "UIInventory.h"

class CUIGameSP :
	public CUIGameCustom
{
public:
	CUIGameSP(CUI* parent);
	virtual ~CUIGameSP(void);

	virtual void		Render				(){}
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	CUIInventory		InventoryMenu;
};
