//=============================================================================
//  Filename:   CUIEncyclopediaCore.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс для представления ядра функциональности энциклопедии
//=============================================================================

#include "stdafx.h"
#include "UIEncyclopediaCore.h"
#include "../UI.h"
#include "../encyclopedia_article.h"
#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

const char * const		ENCYCLOPEDIA_CORE_XML	= "encyclopedia_core.xml";

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaCore::CUIEncyclopediaCore()
	:	m_pTreeItemFont		(NULL),
		m_pTreeRootFont		(NULL),
		m_uTreeRootColor	(0xffffffff),
		m_uTreeItemColor	(0xffffffff)
{
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::Init(CUIListWnd *infoList, CUIListWnd *idxList)
{
	R_ASSERT(infoList);
	R_ASSERT(idxList);

	pIdxList	= idxList;
	pInfoList	= infoList;

	inherited::Init(0,0, UI_BASE_WIDTH,	UI_BASE_HEIGHT);
}

//////////////////////////////////////////////////////////////////////////

ref_str CUIEncyclopediaCore::SetCurrentArtice(CUITreeViewItem *pTVItem)
{
	R_ASSERT(pTVItem);

	pInfoList->RemoveAll();

	// для начала проверим, что нажатый элемент не рутовый
	if (!pTVItem->IsRoot())
	{
		// Удаляем текущую картинку и текст
//		if (m_pCurrArticle)
//		{
//			UIEncyclopediaInfoBkg.DetachChild(&m_pCurrArticle->data()->image);
//			m_pCurrArticle->data()->image.SetMask(NULL);
//		}
		pInfoList->RemoveAll();

		// Отображаем новые
		CUIString str;
		str.SetText(*CStringTable()(*m_ArticlesDB[pTVItem->GetValue()]->data()->text));
		CUIStatic::PreprocessText(str.m_str, pInfoList->GetItemWidth() - 5, pInfoList->GetFont());
		pInfoList->AddParsedItem<CUIListItem>(str, 0, 0xffffffff);
//		UIEncyclopediaInfoBkg.AttachChild(&m_ArticlesDB[pTVItem->GetValue()]->data()->image);
//		m_ArticlesDB[pTVItem->GetValue()]->data()->image.SetMask(&UIImgMask);

//		if(!m_ArticlesDB[pTVItem->GetValue()]->data()->image.GetStaticItem()->GetShader())
//			UIImgMask.Show(false);
//		else
//			UIImgMask.Show(true);

		// Запоминаем текущий эдемент
//		m_pCurrArticle = m_ArticlesDB[pTVItem->GetValue()];
	}
	return pTVItem->GetHierarchyAsText().c_str();
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::DeleteArticles()
{
	for(size_t i = 0; i<m_ArticlesDB.size(); i++)
	{
		xr_delete(m_ArticlesDB[i]);
	}
	m_ArticlesDB.clear();

//	m_pCurrArticle = NULL;
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::AddArticle(ARTICLE_INDEX article_index)
{
	for(std::size_t i = 0; i<m_ArticlesDB.size(); i++)
	{
		if(m_ArticlesDB[i]->Index() == article_index) return;
	}

	// Добавляем элемент
	m_ArticlesDB.resize(m_ArticlesDB.size() + 1);
	CEncyclopediaArticle*& a = m_ArticlesDB.back();
	a = xr_new<CEncyclopediaArticle>();
	a->Load(article_index);
//	RescaleStatic(a->data()->image);
//	a->data()->image.MoveWindow(m_iItemX, m_iItemY);

	// Теперь создаем иерархию вещи по заданному пути

	CreateTreeBranch(a->data()->group, a->data()->name, pIdxList, m_ArticlesDB.size() - 1, 
		m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor);
}

//////////////////////////////////////////////////////////////////////////
