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

#include "UIWindow.h"
class CUIScrollView;
//////////////////////////////////////////////////////////////////////////

class CUINewsWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
					CUINewsWnd	();
	virtual			~CUINewsWnd	();

			void	Init		();
			void	Init		(LPCSTR xml_name, LPCSTR start_from);
	void			AddNews		();
	virtual void	Show		(bool status);

	// Окно вывода информации
//	CUIListWnd		UIListWnd;
	CUIScrollView*	UIScrollWnd;


private:
	void			AddNewsItem	(const shared_str &text);
};

//////////////////////////////////////////////////////////////////////////

#endif