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
//	CUIDialogWnd*		m_pMainInputReceiver;
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
	virtual bool		IR_OnMouseWheel			(int direction);


	void				AddDialogToRender		(CUIWindow* pDialog);
	void				RemoveDialogToRender	(CUIWindow* pDialog);
	
	CUIDialogWnd*		MainInputReceiver		();
	virtual void		ReInitInventoryWnd		() = 0;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CUIGameCustom)
#undef script_type_list
#define script_type_list save_type_list(CUIGameCustom)

#endif // __XR_UIGAMECUSTOM_H__
