//=============================================================================
//  Filename:   CUIEncyclopediaCore.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс для представления ядра функциональности энциклопедии
//=============================================================================

#ifndef	UI_ENCYCLOPEDIA_CORE_H_
#define	UI_ENCYCLOPEDIA_CORE_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
#include "UIListWnd.h"
#include "UITreeViewItem.h"
#include "../encyclopedia_article.h"

//////////////////////////////////////////////////////////////////////////

class CEncyclopediaArticle;

//////////////////////////////////////////////////////////////////////////

class CUIEncyclopediaCore: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIEncyclopediaCore();

	void			Init(CUIListWnd *infoList, CUIListWnd *idxList);

	// Контролы и алгоритмы которые используются для вывода информации
	// Добавляем статью и возвращаем путь к ней
	ref_str			SetCurrentArtice(CUITreeViewItem *pTVItem);
	// Добавлем 1 энциклопедиционную статью
	void			AddArticle(ARTICLE_INDEX);
	void			DeleteArticles();


	// Хранилище статей
	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;

	CGameFont		*m_pTreeRootFont;
	u32				m_uTreeRootColor;
	CGameFont		*m_pTreeItemFont;
	u32				m_uTreeItemColor;

private:
	CUIListWnd		*pInfoList, *pIdxList;
	ArticlesDB		m_ArticlesDB;
};

#endif	//UI_ENCYCLOPEDIA_CORE_H_