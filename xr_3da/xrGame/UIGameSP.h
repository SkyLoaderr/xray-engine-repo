#pragma once
#include "uigamecustom.h"

#include "ui/UIInventoryWnd.h"
#include "ui/UITradeWnd.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIMapWnd.h"
#include "ui/UIDiaryWnd.h"
#include "ui/UITalkWnd.h"
#include "ui/UICarBodyWnd.h"

#include "ui/UIBuyWeaponWnd.h"
#include "ui/UISpawnWnd.h"
#include "ui/UIStatsWnd.h"



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
	CUIPdaWnd			PdaMenu;
	CUIMapWnd			MapMenu;
	CUIDiaryWnd			DiaryMenu;
	CUITalkWnd			TalkMenu;
	CUICarBodyWnd		UICarBodyMenu;
	
	//!!! temporary
	CUIBuyWeaponWnd*	pUIBuyWeaponWnd;
	CUISpawnWnd			UISpawnWnd;
	CUIStatsWnd			UIStatsWnd;
};