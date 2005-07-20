#pragma once
#include "uigamecustom.h"


class CUIInventoryWnd;
class CUITradeWnd;			
class CUIPdaWnd;			
class CUITalkWnd;			
class CUICarBodyWnd;
class CInventory;

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
	virtual void		ReInitInventoryWnd		();

	CUIInventoryWnd*	InventoryMenu;
	CUITradeWnd*		TradeMenu;
	CUIPdaWnd*			PdaMenu;
	CUITalkWnd*			TalkMenu;
	CUICarBodyWnd*		UICarBodyMenu;
	
};