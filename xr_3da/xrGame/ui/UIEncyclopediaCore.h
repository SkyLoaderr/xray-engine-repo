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
#include "UIFrameWindow.h"
#include "../encyclopedia_article.h"

//////////////////////////////////////////////////////////////////////////

class CEncyclopediaArticle;

//////////////////////////////////////////////////////////////////////////

class CUIEncyclopediaCore: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
					CUIEncyclopediaCore();
	void			Init			(CUIListWnd *infoList, CUIListWnd *idxList);
	// Контролы и алгоритмы которые используются для вывода информации
	// Добавляем статью и возвращаем путь к ней
	shared_str		SetCurrentArtice(CUITreeViewItem *pTVItem);
	// Добавлем 1 энциклопедиционную статью
	void			AddArticle		(ARTICLE_INDEX, bool bReaded);
	void			DeleteArticles	();
	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData);
	virtual void	Show			(bool status);
	// Opens tree at item with specified value
	void			OpenTree		(int value);


	// Хранилище статей
	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;

	CGameFont		*m_pTreeRootFont;
	u32				m_uTreeRootColor;
	CGameFont		*m_pTreeItemFont;
	u32				m_uTreeItemColor;

private:
	void			AdjustImagePos	(CUIStatic &s);
	void			OpenTree		(CUITreeViewItem *pItem);

	CUIListWnd		*pInfoList, *pIdxList;
	ArticlesDB		m_ArticlesDB;
	// Маска для изображения предмета текущей статьи
	CUIFrameWindow	UIImgMask;
	
	// Текущая выбранная статья
	CEncyclopediaArticle	*m_pCurrArticle;
	CUIStatic		*pItemImage;

	// Текущая позиция листа с информацией
    int				m_iCurrentInfoListPos;
};

#endif	//UI_ENCYCLOPEDIA_CORE_H_