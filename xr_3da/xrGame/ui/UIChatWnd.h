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
	void				Say					(const ref_str &phrase);
	void				SetEditBoxPrefix	(const ref_str &prefix);
	void				TeamChat			() { sendNextMessageToTeam = true; }
	void				AllChat				() { sendNextMessageToTeam = false; }

	CUIEditBox			UIEditBox;

protected:
	CUIChatLog			*pUILogList;
	CUIStatic			UIPrefix;
	ref_str				m_AuthorName;
	bool				sendNextMessageToTeam;
};

//////////////////////////////////////////////////////////////////////////

#endif
