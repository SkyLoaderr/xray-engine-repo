//=============================================================================
//  Filename:   CUIEncyclopediaCore.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс для представления ядра функциональности энциклопедии
//=============================================================================

#include "stdafx.h"
#include "UIEncyclopediaCore.h"
#include "UIIconedListItem.h"
#include "../UI.h"
#include "../encyclopedia_article.h"
#include "../string_table.h"
//#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

const char * const		ENCYCLOPEDIA_CORE_XML		= "encyclopedia_core.xml";
static int				MAX_PICTURE_WIDTH			= 0;
static const int		MIN_PICTURE_FRAME_WIDTH		= 64;
static const int		MIN_PICTURE_FRAME_HEIGHT	= 64;

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaCore::CUIEncyclopediaCore()
	:	m_pTreeItemFont			(NULL),
		m_pTreeRootFont			(NULL),
		m_uTreeRootColor		(0xffffffff),
		m_uTreeItemColor		(0xffffffff),
		m_pCurrArticle			(NULL),
		m_iCurrentInfoListPos	(0)
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

//	// mask
//	xml_init.InitFrameWindow(uiXml, "item_static:mask_frame_window", 0, &UIImgMask);
//	// Image position
//	m_iItemX = uiXml.ReadAttribInt("item_static", 0, "x", 0);
//	m_iItemY = uiXml.ReadAttribInt("item_static", 0, "y", 0);
	m_iItemY = 0;
	m_iItemX = 0;
	m_iCurrentInfoListPos = 0;
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
		if (m_pCurrArticle)
		{
			pInfoList->DetachChild(&m_pCurrArticle->data()->image);
			m_pCurrArticle->data()->image.SetMask(NULL);
		}
		pInfoList->RemoveAll();

		// Image
		CUIStatic &img = m_ArticlesDB[pTVItem->GetValue()]->data()->image;

//		img.ClipperOn();
//		img.Enable(false);
//		img.SetClipRect(pInfoList->GetWndRect());
//		pItemImage = xr_new<CUIStatic>();
//		pItemImage->Init("ui\\ui_inv_box_heavy_weapons", 0, 0, 1000, 1000);
//		pItemImage->Enable(false);
//		pItemImage->ClipperOn();

//		RECT r;
//		r.left		= 0;
//		r.top		= 0;
//		r.right		= pInfoList->GetWndRect().right;
//		r.bottom	= pInfoList->GetWndRect().bottom;

//		pItemImage->SetClipRect(r);
//		pInfoList->AttachChild(&m_ArticlesDB[pTVItem->GetValue()]->data()->image);

		pInfoList->AttachChild(&img);

		// Добавляем текст
		CUIString str;
		str.SetText(*CStringTable()(*m_ArticlesDB[pTVItem->GetValue()]->data()->text));
		CUIStatic::PreprocessText(str.m_str, pInfoList->GetItemWidth() - 5, pInfoList->GetFont());
		pInfoList->AddParsedItem<CUIListItem>(str, 0, 0xffffffff);
		
//		m_ArticlesDB[pTVItem->GetValue()]->data()->image.SetMask(&UIImgMask);
//
//		if(!m_ArticlesDB[pTVItem->GetValue()]->data()->image.GetStaticItem()->GetShader())
//			UIImgMask.Show(false);
//		else
//			UIImgMask.Show(true);

		// Запоминаем текущий эдемент
		m_pCurrArticle = m_ArticlesDB[pTVItem->GetValue()];
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
	a->data()->image.MoveWindow(m_iItemX, m_iItemY);

	// Теперь создаем иерархию вещи по заданному пути

	CreateTreeBranch(a->data()->group, a->data()->name, pIdxList, m_ArticlesDB.size() - 1, 
		m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor);
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::AdjustImagePos(CUIStatic &s)
{
	Irect	r		= s.GetUIStaticItem().GetOriginalRect();

	// Если картинка не пoмещается в максимальную допустимую длинну
	s.SetWidth(MAX_PICTURE_WIDTH);

	if (r.width() > MAX_PICTURE_WIDTH)
	{
		float scale = static_cast<float>(MAX_PICTURE_WIDTH) / r.width();
		s.SetTextureScale(scale);
		s.SetHeight(static_cast<int>(r.height() * scale));
	}
	// Если помещается, то центрируем ее в отведенной области
	else
	{
		s.SetHeight(r.height());
	}

	if (r.height() < MIN_PICTURE_FRAME_HEIGHT)
	{
		s.SetHeight(MIN_PICTURE_FRAME_HEIGHT);
		s.SetTextureOffset(0, (MIN_PICTURE_FRAME_HEIGHT - r.height()) / 2);
	}

	if (r.width() < MAX_PICTURE_WIDTH)
	{
		s.SetTextureOffset((MAX_PICTURE_WIDTH - r.width()) / 2, s.GetTextureOffeset()[1]);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pInfoList == pWnd && SCROLLBAR_VSCROLL == msg)
	{
		RECT r = m_pCurrArticle->data()->image.GetWndRect();
		m_pCurrArticle->data()->image.MoveWindow(r.left,
			r.top + (m_iCurrentInfoListPos - pInfoList->GetListPosition()) * pInfoList->GetItemHeight());
		m_iCurrentInfoListPos = pInfoList->GetListPosition();
//		RECT r = pItemImage->GetWndRect();
//		pItemImage->MoveWindow(r.left,
//			r.top + (m_iCurrentInfoListPos - pInfoList->GetListPosition()) * pInfoList->GetItemHeight());
//		m_iCurrentInfoListPos = pInfoList->GetListPosition();
	}
}