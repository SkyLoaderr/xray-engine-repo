#pragma once
#include "uigamecustom.h"
#include "UIInventory.h"
#include "ui\\UITradeWnd.h"

class CUIGameSP :
	public CUIGameCustom
{
public:
	CUIGameSP(CUI* parent);
	virtual ~CUIGameSP(void);

	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	//by Dandy 4.07.03
	virtual bool		IR_OnMouseMove			(int dx, int dy);

	CUIInventory		InventoryMenu;
	CUITradeWnd			TradeMenu;

	bool m_bUserMenuShown;
};
