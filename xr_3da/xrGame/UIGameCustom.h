#ifndef __XR_UIGAMECUSTOM_H__
#define __XR_UIGAMECUSTOM_H__
#pragma once


//класс абстрактного диалога
#include "ui/UIDialogWnd.h"

#include "ui/UIMultiTextStatic.h"
#include "script_export_space.h"
// refs
class CUI;
class CTeamBaseZone;
class game_cl_GameState;

class CUIGameCustom :public DLL_Pure, public ISheduled
{
	typedef ISheduled inherited;
protected:
	u32					uFlags;

	void				SetFlag					(u32 mask, BOOL flag){if (flag) uFlags|=mask; else uFlags&=~mask; }
	void				InvertFlag				(u32 mask){if (uFlags&mask) uFlags&=~mask; else uFlags|=mask; }
	BOOL				GetFlag					(u32 mask){return uFlags&mask;}
	CUICaption*			GameCaptions			() {return &m_gameCaptions;}
	CUICaption			m_gameCaptions;
	CUIDialogWnd*		m_pMainInputReceiver;
public:

	virtual void		SetClGame				(game_cl_GameState* g){};

	virtual				float					shedule_Scale		();
	virtual				void					shedule_Update		(u32 dt);
	
						CUIGameCustom			();
	virtual				~CUIGameCustom			();

	virtual	void		Init					()	{};
	
	virtual void		Render					();
	virtual void		OnFrame					();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);
	virtual bool		IR_OnMouseMove			(int dx, int dy);

//	virtual void		OnBuyMenu_Ok			()	{};
//	virtual void		OnBuyMenu_Cancel		()	{};

//	virtual void		OnSkinMenu_Ok			()	{};
//	virtual void		OnSkinMenu_Cancel		()	{};

//	virtual void		OnObjectEnterTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone)	{};
//	virtual void		OnObjectLeaveTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone)	{};

//	virtual void		OnTeamSelect			(int Result)	{};
//	virtual bool		CanBeReady				()	{ return true; };

//	virtual CUIDialogWnd*	GetBuyWnd			()	{ return NULL; };


	//для остановки и запуска дополнительных меню
//	virtual void		StartStopMenu			(CUIDialogWnd* pDialog);
	
	//текущее меню пользователя показанное на экране
	//NULL если такого сейчас нет


	xr_vector<CUIWindow*>					m_dialogsToRender;
	void					AddDialogToRender	(CUIWindow* pDialog);
	void					RemoveDialogToRender(CUIWindow* pDialog);
	
	void					SetMainInputReceiver(CUIDialogWnd* ir){ m_pMainInputReceiver = ir;};
	CUIDialogWnd*			MainInputReceiver	(){ return m_pMainInputReceiver; };

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CUIGameCustom)
#undef script_type_list
#define script_type_list save_type_list(CUIGameCustom)

#endif // __XR_UIGAMECUSTOM_H__
