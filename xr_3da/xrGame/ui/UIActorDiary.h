//=============================================================================
//  Filename:   UIActorDiary.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Диалог дневника актера
//=============================================================================

#ifndef UI_ACTOR_DIARY_H_
#define UI_ACTOR_DIARY_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
#include "UIListWnd.h"
#include "UIEncyclopediaCore.h"
#include "../encyclopedia_article.h"

//////////////////////////////////////////////////////////////////////////

class CUIActorDiaryWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
	CUIActorDiaryWnd();
	~CUIActorDiaryWnd();
	void	Init(CUIListWnd *idxList);

	void	AddArticle(ARTICLE_INDEX idx)			{ m_pCore->AddArticle(idx); }
	void	DeleteArticles(CUITreeViewItem *pRoot);
	void	ShowArticle(CUITreeViewItem *pItem);

	CUIEncyclopediaCore *m_pCore;
private:
	CUIListWnd			UIInfoList;
};

//////////////////////////////////////////////////////////////////////////

#endif	// UI_ACTOR_DIARY_H_