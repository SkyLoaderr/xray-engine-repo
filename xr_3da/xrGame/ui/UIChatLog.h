//=============================================================================
//  Filename:   UIChatLog.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer chat log window
//=============================================================================

#ifndef UI_CHAT_LOG_H_
#define UI_CHAT_LOG_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
#include "UIListWnd.h"

//////////////////////////////////////////////////////////////////////////

class CUIChatLog: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;

public:
	void					AddLogMessage	(const ref_str &msg, const ref_str &author);
	CUIListWnd &			GetLogList		()	{ return UILogList; }
	void					Init			();
	virtual void			Update			();

private:
	// Log list
	CUIListWnd				UILogList;
	// Indexes to delete
	typedef xr_set<int, std::greater<int> > ToDelIndexes;
	typedef ToDelIndexes::iterator			ToDelIndexes_it;
	ToDelIndexes			toDelIndexes;
};

//////////////////////////////////////////////////////////////////////////

#endif