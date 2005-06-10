//=============================================================================
//  Filename:   UIGameLog.h
//	Created by Vitaly 'Mad Max' Maximov, mad-max@gsc-game.kiev.ua
//	Copyright 2005. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer game log window
//=============================================================================

#ifndef UI_GAME_LOG_H_
#define UI_GAME_LOG_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
#include "UIListWnd.h"
#include "KillMessageStruct.h"

class CUIXml;

//////////////////////////////////////////////////////////////////////////

class CUIGameLog: public CUIListWnd
{
	typedef CUIDialogWnd inherited;

public:
	CUIGameLog();
	virtual ~CUIGameLog();
	void					AddLogMessage	(const shared_str& msg);
	void					AddLogMessage	(KillMessageStruct& msg);
//	CUIListWnd &			GetLogList		()	{ return UILogList; }
	virtual void			Init			(float x, float y, float width, float height);
//	virtual void			SetFont			(CGameFont* pFont);
	virtual void			Update			();
//	virtual void			Draw			();
//	CUIListWnd*				operator*() {return &UILogList;};

private:
	// Log list
//	CUIListWnd				UILogList;
	// Indexes to delete
	typedef xr_set<int, std::greater<int> > ToDelIndexes;
	typedef ToDelIndexes::iterator			ToDelIndexes_it;
	ToDelIndexes			toDelIndexes;
};

//////////////////////////////////////////////////////////////////////////

#endif