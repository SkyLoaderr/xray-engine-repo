//=============================================================================
//  Filename:   UIActorDiary.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ������ �������� ������
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
					CUIActorDiaryWnd	();
	virtual			~CUIActorDiaryWnd	();
	void			Init				(CUIListWnd *idxList);

	void			AddArticle			(ARTICLE_ID id, bool bReaded)	{ m_pCore->AddArticle(id, bReaded); }
	void			DeleteArticles		(CUITreeViewItem *pRoot);
	void			ShowArticle			(CUITreeViewItem *pItem);
	virtual void	Show				(bool status);

	void			ReloadArticles		();

	CUIEncyclopediaCore *m_pCore;
private:
	CUIListWnd		UIInfoList;
};

//////////////////////////////////////////////////////////////////////////

#endif	// UI_ACTOR_DIARY_H_