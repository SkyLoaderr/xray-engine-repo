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

	virtual bool		OnKeyboardPress		(int dik);
	virtual bool		OnKeyboardRelease	(int dik);

	CUIInventory		InventoryMenu;
};
