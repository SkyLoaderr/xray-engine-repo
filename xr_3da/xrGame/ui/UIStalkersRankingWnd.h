//=============================================================================
//  Filename:   UIStalkersRankingWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Подокно дневника ПДА - рейтинг сталкеров всей игры
//=============================================================================

#ifndef UI_STALKERS_RANKING_WND_H_
#define UI_STALKERS_RANKING_WND_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UICharacterInfo.h"
#include "UIListWnd.h"

//////////////////////////////////////////////////////////////////////////

class CUIStalkersRankingWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
	virtual void		Init				();
	void				AddStalkerInfo		();
	void				RemoveStalkers		();
	virtual void		Show				(bool status);
	virtual void		SendMessage			(CUIWindow* pWnd, s16 msg, void* pData /* = NULL */);
	
protected:
	// Фреймы дикоративного оформления
	CUIFrameWindow		UIInfoFrame;
	CUIFrameWindow		UICharIconFrame;
	CUIFrameLineWnd		UIInfoHeader;
	CUIFrameLineWnd		UICharIconHeader;
	CUIAnimatedStatic	UIAnimatedIcon;
	CUIStatic			UIArticleHeader;

	// информация о персонаже
	CUIWindow			UICharacterWindow;
	CUICharacterInfo	UICharacterInfo;
	CUIFrameWindow		UIMask;
	// Список сталкеров
	CUIListWnd			UIStalkersList;
};

#endif