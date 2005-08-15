#pragma once
#include "uigamecustom.h"
#include "ui/UIDialogWnd.h"
#include "net_utils.h"

class CUIInventoryWnd;
class CUITradeWnd;			
class CUIPdaWnd;			
class CUITalkWnd;			
class CUICarBodyWnd;
class CInventory;

class game_cl_Single;
class CChangeLevelWnd;
class CUIMessageBox;

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
	void				ChangeLevel				(u32 game_vert_id, u32 level_vert_id, Fvector pos, Fvector ang);

	CUIInventoryWnd*	InventoryMenu;
	CUITradeWnd*		TradeMenu;
	CUIPdaWnd*			PdaMenu;
	CUITalkWnd*			TalkMenu;
	CUICarBodyWnd*		UICarBodyMenu;
	CChangeLevelWnd*	UIChangeLevelWnd;
};

class CChangeLevelWnd :public CUIDialogWnd
{
	CUIMessageBox*		m_messageBox;
public:
	u32						m_game_vertex_id;
	u32						m_level_vertex_id;
	Fvector					m_position;
	Fvector					m_angles;

						CChangeLevelWnd		();
	virtual				~CChangeLevelWnd	()									{};
	virtual void		SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
};