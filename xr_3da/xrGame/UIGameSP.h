#pragma once
#include "uigamecustom.h"

#include "ui/UIInventoryWnd.h"
#include "ui/UITradeWnd.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIDiaryWnd.h"
#include "ui/UITalkWnd.h"
#include "ui/UICarBodyWnd.h"

//#include "ui/UIBuyWeaponWnd.h"
//#include "ui/UISpawnWnd.h"
//#include "ui/UIStatsWnd.h"

class game_cl_Single;

class CUIGameSP : public CUIGameCustom
{
private:
	game_cl_Single*		m_game;
	typedef CUIGameCustom inherited;
public:
	CUIGameSP									();
	virtual				~CUIGameSP				();

	virtual void		SetClGame				(game_cl_GameState* g);
	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	void				StartTalk				();
	void				StartCarBody			(CInventory* pOurInv,    CGameObject* pOurObject,
												 CInventory* pOthersInv, CGameObject* pOthersObject);
	virtual void		ReInitInventoryWnd		() { if (InventoryMenu.IsShown()) InventoryMenu.InitInventory(); };

	CUIInventoryWnd		InventoryMenu;
	CUITradeWnd			TradeMenu;
	CUIPdaWnd			PdaMenu;
	CUIDiaryWnd			DiaryMenu;
	CUITalkWnd			TalkMenu;
	CUICarBodyWnd		UICarBodyMenu;
	
	//!!! temporary
//	CUIBuyWeaponWnd*	pUIBuyWeaponWnd;
//	CUISpawnWnd			UISpawnWnd;
//	CUIStatsWnd			UIStatsWnd;
};