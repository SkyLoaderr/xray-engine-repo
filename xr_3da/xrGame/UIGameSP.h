#pragma once
#include "uigamecustom.h"

#include "ui\\UIDialogWnd.h"
#include "ui\\UIInventoryWnd.h"
#include "ui\\UITradeWnd.h"
#include "ui\\UIPdaWnd.h"
#include "ui\\UIMapWnd.h"
#include "ui\\UIDiaryWnd.h"

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

	void StartTrade();
	void StartStopMenu(CUIDialogWnd* pDialog);
protected:

	CUIInventoryWnd		InventoryMenu;
	CUITradeWnd			TradeMenu;
	CUIPdaWnd			PdaMenu;
	CUIMapWnd			MapMenu;
	CUIDiaryWnd			DiaryMenu;
	
	//текущее меню пользователя показанное на экране
	//NULL если такого сейчас нет
	CUIDialogWnd* m_pUserMenu;
};