//=============================================================================
//  Filename:   CUIEncyclopediaCore.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� ��� ������������� ���� ���������������� ������������
//=============================================================================

#include "stdafx.h"
#include "UIEncyclopediaCore.h"
#include "UIIconedListItem.h"
#include "../UI.h"
#include "../encyclopedia_article.h"
#include "../string_table.h"
#include "../level.h"
#include "../HUDManager.h"
#include "../alife_registry_wrappers.h"

//////////////////////////////////////////////////////////////////////////

const char * const		ENCYCLOPEDIA_CORE_XML		= "encyclopedia.xml";
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
	pInfoList->SetRightIndention(static_cast<int>(10 * HUD().GetScale()));

	CUIXml		uiXml;
	bool xml_result = uiXml.Init("$game_data$", ENCYCLOPEDIA_CORE_XML);
	R_ASSERT3(xml_result, "xml file not found", ENCYCLOPEDIA_CORE_XML);

	CUIXmlInit	xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH,	UI_BASE_HEIGHT);

	// mask
	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIImgMask);
	m_iCurrentInfoListPos = 0;
}

//////////////////////////////////////////////////////////////////////////

shared_str CUIEncyclopediaCore::SetCurrentArtice(CUITreeViewItem *pTVItem)
{
	R_ASSERT(pTVItem);

	pInfoList->ScrollToBegin();
	pInfoList->RemoveAll();
	m_iCurrentInfoListPos = 0;
	// ������� ������� ��������
	if (m_pCurrArticle && m_pCurrArticle->data())
	{
		pInfoList->DetachChild(&m_pCurrArticle->data()->image);
//		m_pCurrArticle->data()->image.MoveWindow(0, 0);
	}

	// ��� ������ ��������, ��� ������� ������� �� �������
	if (!pTVItem->IsRoot())
	{
		// Image
		CUIStatic &img = m_ArticlesDB[pTVItem->GetValue()]->data()->image;
		img.MoveWindow(0, 0);
		RECT r = pInfoList->GetAbsoluteRect();

		img.SetClipRect(r);
		UIImgMask.SetClipper(true, r);
		AdjustImagePos(img);
		pInfoList->AttachChild(&img);
		img.SetMask(&UIImgMask);

		// ��������� �����
		CUIString str;
		str.SetText(*CStringTable()(*m_ArticlesDB[pTVItem->GetValue()]->data()->text));
//		CUIStatic::PreprocessText(str.m_str, pInfoList->GetItemWidth() - 5, pInfoList->GetFont());
		pInfoList->AddParsedItem<CUIListItem>(str, 0, 0xffffffff);

//		m_ArticlesDB[pTVItem->GetValue()]->data()->image.SetMask(&UIImgMask);
//
//		if(!m_ArticlesDB[pTVItem->GetValue()]->data()->image.GetStaticItem()->GetShader())
//			UIImgMask.Show(false);
//		else

		// ���������� ������� �������
		m_pCurrArticle = m_ArticlesDB[pTVItem->GetValue()];

		// ������� ��� �����������
		if (!pTVItem->IsArticleReaded())
		{
			CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

			if(pActor && pActor->encyclopedia_registry->registry().objects_ptr())
			{
				for(ARTICLE_VECTOR::iterator it = pActor->encyclopedia_registry->registry().objects().begin();
					it != pActor->encyclopedia_registry->registry().objects().end(); it++)
				{
					if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type &&
						m_pCurrArticle->Index() == it->index)
					{
						it->readed = true;
						break;
					}
				}
			}
		}
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

void CUIEncyclopediaCore::AddArticle(ARTICLE_INDEX article_index, bool bReaded)
{
	for(std::size_t i = 0; i<m_ArticlesDB.size(); i++)
	{
		if(m_ArticlesDB[i]->Index() == article_index) return;
	}

	// ��������� �������
	m_ArticlesDB.resize(m_ArticlesDB.size() + 1);
	CEncyclopediaArticle*& a = m_ArticlesDB.back();
	a = xr_new<CEncyclopediaArticle>();
	a->Load(article_index);

	CUIStatic &img = a->data()->image;

	img.ClipperOn();
	img.Enable(false);

	// ������ ������� �������� ���� �� ��������� ����

	CreateTreeBranch(a->data()->group, a->data()->name, pIdxList, m_ArticlesDB.size() - 1, 
		m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor, bReaded);
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::AdjustImagePos(CUIStatic &s)
{
	// ����������� ��������� �� ������ �����
	s.MoveWindow(((pInfoList->GetWndRect().right - pInfoList->GetWndRect().left) - (s.GetWndRect().right - s.GetWndRect().left) - 15) / 2, s.GetWndRect().top);

	// ������ ��������� � ���� ������� ������ �������, ����� ��� ��������� ��������� ���� ��������
	int emptyItemsCnt = iCeil(static_cast<float>(s.GetWndRect().bottom) / pInfoList->GetItemHeight());

	for (int i = 0; i < emptyItemsCnt; ++i)
	{
		pInfoList->AddItem<CUIListItem>("");
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pInfoList == pWnd && SCROLLBAR_VSCROLL == msg && m_pCurrArticle)
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

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::Show(bool status)
{
	if (!status)
	{
		if (m_pCurrArticle)
		{
			pInfoList->DetachChild(&m_pCurrArticle->data()->image);
			m_pCurrArticle->data()->image.MoveWindow(0, 0);

			m_pCurrArticle = NULL;
		}
		pInfoList->RemoveAll();
	}

	m_iCurrentInfoListPos = 0;
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::OpenTree(int value)
{
	R_ASSERT(pIdxList);
	// Find tree item

	CUITreeViewItem *pItem	= NULL;
	CUITreeViewItem *result	= NULL;

	for (int i = 0; i < pIdxList->GetSize(); ++i)
	{
		pItem = smart_cast<CUITreeViewItem*>(pIdxList->GetItem(i));
		if (pItem)
		{
			result = pItem->Find(value);
			if (result) break;
		}
	}

	if (result)
	{
		OpenTree(result);
		pIdxList->SendMessage(result, BUTTON_CLICKED, NULL);
		result->SendMessage(result, BUTTON_CLICKED, NULL);
		pIdxList->ScrollToPos(result->GetIndex());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::OpenTree(CUITreeViewItem *pItem)
{
	R_ASSERT(pItem);
	if (pItem->GetOwner())
	{
		OpenTree(pItem->GetOwner());
	}

	if (pItem->IsRoot())
		pItem->Open();
}