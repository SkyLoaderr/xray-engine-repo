//=============================================================================
//  Filename:   UIChatWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Simple chat window for multiplayer purposes
//=============================================================================

#ifndef UI_CHAT_WND_H_
#define UI_CHAT_WND_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
#include "UIEditBox.h"

//////////////////////////////////////////////////////////////////////////

class CUIChatLog;
class game_cl_GameState;

extern const char * const	CHAT_MP_WND_XML;
extern const int			fadeDelay;

//////////////////////////////////////////////////////////////////////////

class CUIChatWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;

public:
						CUIChatWnd			(CUIChatLog *pList);
	virtual				~CUIChatWnd			();
	virtual void		Show				();
	virtual void		Hide				();
	virtual void		SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);
	void				Init				();
	void				Say					(const shared_str &phrase);
	void				SetEditBoxPrefix	(const shared_str &prefix);
	void				TeamChat			() { sendNextMessageToTeam = true; }
	void				AllChat				() { sendNextMessageToTeam = false; }
	void				SetOwner			(game_cl_GameState *pO) { pOwner = pO; }

	CUIEditBox			UIEditBox;

protected:
	CUIChatLog			*pUILogList;
	CUIStatic			UIPrefix;
	shared_str				m_AuthorName;
	bool				sendNextMessageToTeam;
	game_cl_GameState	*pOwner;
};

//////////////////////////////////////////////////////////////////////////

#endif
