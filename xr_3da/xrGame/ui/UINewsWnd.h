//=============================================================================
//  Filename:   UINewsWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  News subwindow in PDA dialog
//=============================================================================

#ifndef UI_NEWS_WND_H_
#define UI_NEWS_WND_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIListWnd.h"

//////////////////////////////////////////////////////////////////////////

class CUINewsWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
					CUINewsWnd	();
	virtual			~CUINewsWnd	();

	virtual void	Init		();
	void			AddNews		();
	virtual void	Show		(bool status);

	// Окно вывода информации
	CUIListWnd		UIListWnd;

private:
	void			AddNewsItem	(const ref_str &text);
};

//////////////////////////////////////////////////////////////////////////

#endif