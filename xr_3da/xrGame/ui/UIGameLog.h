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

//////////////////////////////////////////////////////////////////////////

class CUIGameLog: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;

public:
	void					AddLogMessage	(const shared_str &msg);
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