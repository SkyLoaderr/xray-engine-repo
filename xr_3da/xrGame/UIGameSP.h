#pragma once
#include "uigamecustom.h"

#include "ui/UIInventoryWnd.h"
#include "ui/UITradeWnd.h"
#include "ui/UIPdaCommunication.h"
#include "ui/UIMapWnd.h"
#include "ui/UIDiaryWnd.h"
#include "ui/UITalkWnd.h"
#include "ui/UICarBodyWnd.h"

class CUIGameSP : public CUIGameCustom
{
private:
	typedef CUIGameCustom inherited;
public:
	CUIGameSP(CUI* parent);
	virtual ~CUIGameSP(void);

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	void StartTalk();
	void StartCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
					  CInventory* pOthersInv, CGameObject* pOthersObject);

	CUIInventoryWnd		InventoryMenu;
	CUITradeWnd			TradeMenu;
	CUIPdaCommunication	PdaMenu;
	CUIMapWnd			MapMenu;
	CUIDiaryWnd			DiaryMenu;
	CUITalkWnd			TalkMenu;
	CUICarBodyWnd		UICarBodyMenu;
};