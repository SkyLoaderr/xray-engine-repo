//=============================================================================
//  Filename:   UIEncyclopediaWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#include "StdAfx.h"
#include "UIEncyclopediaWnd.h"
#include "UIPdaWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../HUDManager.h"
#include "../level.h"
#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

const char * const	ENCYCLOPEDIA_DIALOG_XML		= "encyclopedia.xml";

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaWnd::CUIEncyclopediaWnd()
{
}

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaWnd::~CUIEncyclopediaWnd()
{
//	if (m_pCurrArticle)
//		UIEncyclopediaInfoBkg.DetachChild(&m_pCurrArticle->data()->image);

	DeleteArticles();
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::Init()
{
	CUIXml		uiXml;
	bool xml_result = uiXml.Init("$game_data$", ENCYCLOPEDIA_DIALOG_XML);
	R_ASSERT3(xml_result, "xml file not found", ENCYCLOPEDIA_DIALOG_XML);

	CUIXmlInit	xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	// Load xml data
	AttachChild(&UIEncyclopediaIdxBkg);
	xml_init.InitFrameWindow(uiXml, "right_frame_window", 0, &UIEncyclopediaIdxBkg);

	xml_init.InitFont(uiXml, "tree_item_font", 0, UIInfo.m_uTreeItemColor, UIInfo.m_pTreeItemFont);
	R_ASSERT(UIInfo.m_pTreeItemFont);
	xml_init.InitFont(uiXml, "tree_root_font", 0, UIInfo.m_uTreeRootColor, UIInfo.m_pTreeRootFont);
	R_ASSERT(UIInfo.m_pTreeRootFont);


	UIEncyclopediaIdxBkg.AttachChild(&UIEncyclopediaIdxHeader);
	xml_init.InitFrameLine(uiXml, "right_frame_line", 0, &UIEncyclopediaIdxHeader);

	UIEncyclopediaIdxHeader.AttachChild(&UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimation);

	AttachChild(&UIEncyclopediaInfoBkg);
	xml_init.InitFrameWindow(uiXml, "left_frame_window", 0, &UIEncyclopediaInfoBkg);
	UIEncyclopediaInfoBkg.AttachChild(&UIEncyclopediaInfoHeader);

	UIEncyclopediaInfoHeader.UITitleText.SetElipsis(CUIStatic::eepBegin, 20);
	xml_init.InitFrameLine(uiXml, "left_frame_line", 0, &UIEncyclopediaInfoHeader);

	UIEncyclopediaInfoBkg.AttachChild(&UIArticleHeader);
	xml_init.InitStatic(uiXml, "article_header_static", 0, &UIArticleHeader);


	UIEncyclopediaIdxBkg.AttachChild(&UIIdxList);
	xml_init.InitListWnd(uiXml, "idx_list", 0, &UIIdxList);
	UIIdxList.SetMessageTarget(this);
	UIIdxList.EnableScrollBar(true);

	UIEncyclopediaInfoBkg.AttachChild(&UIInfoList);
	xml_init.InitListWnd(uiXml, "info_list", 0, &UIInfoList);
	UIInfoList.EnableScrollBar(true);
	UIInfoList.SetNewRenderMethod(true);
	UIInfoList.ActivateList(false);
	UIInfoList.SetMessageTarget(&UIInfo);

	UIInfo.Init(&UIInfoList, &UIIdxList);

	// Читаем максимальную длинну картинки в энциклопедии
//	MAX_PICTURE_WIDTH = uiXml.ReadAttribInt("item_static", 0, "width", 0);

	string256 header;
	strconcat(header, ALL_PDA_HEADER_PREFIX, "/Encyclopedia");
	m_InfosHeaderStr = header;

	xml_init.InitAutoStatic(uiXml, "left_auto_static", &UIEncyclopediaInfoBkg);
	xml_init.InitAutoStatic(uiXml, "right_auto_static", &UIEncyclopediaIdxBkg);

	// Кнопка возврата
	AttachChild(&UIBack);
	xml_init.InitButton(uiXml, "back_button", 0, &UIBack);
	UIBack.Show(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::SendMessage(CUIWindow *pWnd, s16 msg, void* pData)
{
	if (&UIIdxList == pWnd && LIST_ITEM_CLICKED == msg)
	{
		CUITreeViewItem *pTVItem = static_cast<CUITreeViewItem*>(pData);
		R_ASSERT(pTVItem);

//		if (!pTVItem->IsRoot())
//		{
//			// Удаляем текущую картинку и текст
//			if (m_pCurrArticle)
//			{
//				UIEncyclopediaInfoBkg.DetachChild(&m_pCurrArticle->data()->image);
//				m_pCurrArticle->data()->image.SetMask(NULL);
//			}
//			UIInfo.UIInfoDisplay.RemoveAll();
//
//			// Отображаем новые
//			CUIString str;
//			str.SetText(*CStringTable()(*m_ArticlesDB[pTVItem->GetValue()]->data()->text));
//			CUIStatic::PreprocessText(str.m_str, UIInfo.UIInfoDisplay.GetItemWidth() - 5, UIInfo.UIInfoDisplay.GetFont());
//			UIInfo.UIInfoDisplay.AddParsedItem<CUIListItem>(str, 0, 0xffffffff);
//			UIEncyclopediaInfoBkg.AttachChild(&m_ArticlesDB[pTVItem->GetValue()]->data()->image);
//			m_ArticlesDB[pTVItem->GetValue()]->data()->image.SetMask(&UIImgMask);
//
//			if(!m_ArticlesDB[pTVItem->GetValue()]->data()->image.GetStaticItem()->GetShader())
//				UIImgMask.Show(false);
//			else
//				UIImgMask.Show(true);
		std::string caption = static_cast<std::string>(*m_InfosHeaderStr) + *UIInfo.SetCurrentArtice(pTVItem);
		UIEncyclopediaInfoHeader.UITitleText.SetText(caption.c_str());
		caption.erase(0, caption.find_last_of("/") + 1);
		UIArticleHeader.SetText(caption.c_str());

			// Запоминаем текущий эдемент
//			m_pCurrArticle = m_ArticlesDB[pTVItem->GetValue()];
//		}
	}
	else if (&UIBack == pWnd && BUTTON_CLICKED == msg)
	{
		UIBack.Show(false);
		GetTop()->SendMessage(this, PDA_GO_BACK, NULL);
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::Draw()
{
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::Show(bool status)
{
	if (status)
	{
//		DeleteArticles();

		static u32 prevArticlesCount = 0;
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(pActor && pActor->encyclopedia_registry.objects_ptr() && pActor->encyclopedia_registry.objects_ptr()->size() > prevArticlesCount)
		{
			ARTICLE_VECTOR::const_iterator it = pActor->encyclopedia_registry.objects_ptr()->begin();
			std::advance(it, prevArticlesCount);
			for(; it != pActor->encyclopedia_registry.objects_ptr()->end(); it++)
			{
				if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type)
				{
					UIInfo.AddArticle(it->index, it->readed);
				}
			}

			prevArticlesCount = pActor->encyclopedia_registry.objects_ptr()->size();
		}
	}
	else
	{
		UIBack.Show(false);
	}

	UIInfo.Show(status);
	inherited::Show(status);
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::AddArticle(ARTICLE_INDEX idx, bool bReaded)
{
	UIInfo.AddArticle(idx, bReaded);
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::DeleteArticles()
{
	UIInfo.DeleteArticles();
	UIIdxList.RemoveAll();
}