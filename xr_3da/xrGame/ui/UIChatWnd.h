//=============================================================================
//  Filename:   UIChatWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Simple chat window for multiplayer purposes
//=============================================================================

#include "UIDialogWnd.h"
#include "UIEditBox.h"

//////////////////////////////////////////////////////////////////////////

class CUIListWnd;
class game_cl_GameState;

//////////////////////////////////////////////////////////////////////////

class CUIChatWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;

public:
						CUIChatWnd			(CUIListWnd *pList);
	virtual				~CUIChatWnd			();
	virtual void		Show				();
	virtual void		Hide				();
	virtual void		SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);
	void				Init				();
	void				Say					(const ref_str &phrase);
	void				SetEditBoxPrefix	(const ref_str &prefix);
	void				SetReplicaAuthor	(const ref_str &authorName);
	void				SetOwner			(game_cl_GameState *pO) { pOwner = pO; }

	CUIEditBox			UIEditBox;

protected:
	CUIListWnd			*pUILogList;
	CUIStatic			UIPrefix;
	ref_str				m_AuthorName;
	game_cl_GameState	*pOwner;
};