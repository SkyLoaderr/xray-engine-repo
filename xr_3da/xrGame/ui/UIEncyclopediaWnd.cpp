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
#include "../encyclopedia_article.h"
#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

const char * const	ENCYCLOPEDIA_DIALOG_XML		= "encyclopedia_new.xml";
static int			MAX_PICTURE_WIDTH			= 0;
static const int	MIN_PICTURE_FRAME_WIDTH		= 64;
static const int	MIN_PICTURE_FRAME_HEIGHT	= 64;

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaWnd::CUIEncyclopediaWnd()
	:	m_pCurrArticle		(NULL),
		m_pTreeItemFont		(NULL),
		m_pTreeRootFont		(NULL),
		m_uTreeRootColor	(0xffffffff),
		m_uTreeItemColor	(0xffffffff)
{
}

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaWnd::~CUIEncyclopediaWnd()
{
	if (m_pCurrArticle)
		UIEncyclopediaInfoBkg.DetachChild(&m_pCurrArticle->data()->image);

	DeleteArticles();
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::Init()
{
	CUIXml		uiXml;
	bool xml_result = uiXml.Init("$game_data$", ENCYCLOPEDIA_DIALOG_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit	xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	// Load xml data
	AttachChild(&UIEncyclopediaIdxBkg);
	xml_init.InitFrameWindow(uiXml, "right_frame_window", 0, &UIEncyclopediaIdxBkg);

	xml_init.InitFont(uiXml, "tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
	R_ASSERT(m_pTreeItemFont);
	xml_init.InitFont(uiXml, "tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
	R_ASSERT(m_pTreeRootFont);

	UIEncyclopediaIdxBkg.AttachChild(&UIEncyclopediaIdxHeader);
	xml_init.InitFrameLine(uiXml, "right_frame_line", 0, &UIEncyclopediaIdxHeader);

	UIEncyclopediaIdxHeader.AttachChild(&UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimation);

	AttachChild(&UIEncyclopediaInfoBkg);
	xml_init.InitFrameWindow(uiXml, "left_frame_window", 0, &UIEncyclopediaInfoBkg);
	UIEncyclopediaInfoBkg.AttachChild(&UIEncyclopediaInfoHeader);

	UIEncyclopediaInfoHeader.UITitleText.SetElipsis(CUIStatic::eepCenter, 15);
	xml_init.InitFrameLine(uiXml, "left_frame_line", 0, &UIEncyclopediaInfoHeader);
	UIEncyclopediaIdxBkg.AttachChild(&UIIdxList);

	UIEncyclopediaInfoBkg.AttachChild(&UIArticleHeader);
	xml_init.InitStatic(uiXml, "article_header_static", 0, &UIArticleHeader);

	xml_init.InitListWnd(uiXml, "idx_list", 0, &UIIdxList);
	UIIdxList.SetMessageTarget(this);
	UIIdxList.EnableScrollBar(true);

	UIEncyclopediaInfoBkg.AttachChild(&UIInfoList);
	UIIdxList.EnableScrollBar(true);
	UIInfoList.SetNewRenderMethod(true);

	xml_init.InitListWnd(uiXml, "info_list", 0, &UIInfoList);
	UIInfoList.ActivateList(false);
	// mask
	xml_init.InitFrameWindow(uiXml, "item_static:mask_frame_window", 0, &UIImgMask);
	m_iItemX = uiXml.ReadAttribInt("item_static", 0, "x", 0);
	m_iItemY = uiXml.ReadAttribInt("item_static", 0, "y", 0);

	// Читаем максимальную длинну картинки в энциклопедии
	MAX_PICTURE_WIDTH = uiXml.ReadAttribInt("item_static", 0, "width", 0);

	string256 header;
	strconcat(header, ALL_PDA_HEADER_PREFIX, "/Encyclopedia");
	m_InfosHeaderStr = header;

	xml_init.InitAutoStatic(uiXml, "left_auto_static", &UIEncyclopediaInfoBkg);
	xml_init.InitAutoStatic(uiXml, "right_auto_static", &UIEncyclopediaIdxBkg);

}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::DeleteArticles()
{
	for(size_t i = 0; i<m_ArticlesDB.size(); i++)
	{
		xr_delete(m_ArticlesDB[i]);
	}
	m_ArticlesDB.clear();

	UIIdxList.RemoveAll();

	m_pCurrArticle = NULL;
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::AddArticle(ARTICLE_INDEX article_index)
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
	RescaleStatic(a->data()->image);
	a->data()->image.MoveWindow(m_iItemX, m_iItemY);

	// Начинаем алгоритм определения группы вещи в иерархии энциклопедии
	std::string group = *a->data()->group;
	R_ASSERT(!group.empty());

	// Парсим строку группы для определения вложенности
	GroupTree					groupTree;

	std::string::size_type		pos;
	std::string					oneLevel;
	
	while (true)
	{
		pos = group.find('/');
		if (pos != std::string::npos)
		{
			oneLevel.assign(group, 0, pos);
			ref_str str(oneLevel.c_str());
			groupTree.push_back(CStringTable()(str));
			group.erase(0, pos + 1);
		}
		else
		{
			groupTree.push_back(CStringTable()(group.c_str()));
			break;
		}
	}

	// Теперь ищем нет ли затребованных групп уже в наличии
	CUITreeViewItem *pTVItem = NULL, *pTVItemChilds = NULL;
	bool status = false;

	// Для всех рутовых элементов
	for (int i = 0; i < UIIdxList.GetSize(); ++i)
	{
		pTVItem = dynamic_cast<CUITreeViewItem*>(UIIdxList.GetItem(i));
		R_ASSERT(pTVItem);

		pTVItem->Close();

		std::string	caption = pTVItem->GetText();
		// Remove "+" sign
		caption.erase(0, 1);

		// Ищем не содержит ли он данной иерархии и добавляем новые элементы если не найдено
		if (0 == xr_strcmp(caption.c_str(), *groupTree.front()))
		{
			// Уже содержит. Надо искать глубже
			pTVItemChilds = pTVItem;
			for (GroupTree_it it = groupTree.begin() + 1; it != groupTree.end(); ++it)
			{
				pTVItem = pTVItemChilds->Find(*(*it));
				// Не нашли, надо вставлять хвост списка вложенности
				if (!pTVItem)
				{
					pTVItemChilds = AddTreeTail(it, groupTree, pTVItemChilds);
					status = true;
					break;
				}
				pTVItemChilds = pTVItem;
			}
		}

		if (status) break;
	}

	// Прошли все существующее дерево, и не нашли? Тогда добавляем новую иерархию
	if (!pTVItemChilds)
	{
		pTVItemChilds = xr_new<CUITreeViewItem>();
		pTVItemChilds->SetFont(m_pTreeRootFont);
		pTVItemChilds->SetText(*groupTree.front());
		pTVItemChilds->SetTextColor(m_uTreeRootColor);
		pTVItemChilds->SetRoot(true);
		UIIdxList.AddItem<CUITreeViewItem>(pTVItemChilds);

		// Если в списке вложенности 1 элемент, то хвоста нет, и соответственно ничего не добавляем
		if (groupTree.size() > 1)
			pTVItemChilds = AddTreeTail(groupTree.begin() + 1, groupTree, pTVItemChilds);
	}


	// Теперь читаем имя в xml и добавляем последний элемент
	const ref_str name = a->data()->name;

	// К этому моменту pTVItemChilds обязательно должна быть не NULL
	R_ASSERT(pTVItemChilds);

	// Cначала проверяем нет ли записи с таким названием, и добавляем если нет
//	if (!pTVItemChilds->Find(*name))
//	{
	pTVItem		= xr_new<CUITreeViewItem>();
	pTVItem->SetFont(m_pTreeItemFont);
	pTVItem->SetTextColor(m_uTreeItemColor);
	pTVItem->SetText(*CStringTable()(*name));
	pTVItem->SetValue(m_ArticlesDB.size() - 1);
	pTVItemChilds->AddItem(pTVItem);
//	}
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem * CUIEncyclopediaWnd::AddTreeTail(GroupTree_it it, GroupTree &cont, CUITreeViewItem *pItemToIns)
{
	// Вставляем иерархию разделов в энциклопедию
	CUITreeViewItem *pNewItem = NULL;

	for (GroupTree_it it2 = it; it2 != cont.end(); ++it2)
	{
		pNewItem = xr_new<CUITreeViewItem>();
		pItemToIns->AddItem(pNewItem);
		pNewItem->SetFont(m_pTreeRootFont);
		pNewItem->SetText(*(*it2));
		pNewItem->SetTextColor(m_uTreeRootColor);
		pNewItem->SetRoot(true);
		pItemToIns = pNewItem;
	}

	return pNewItem;
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::SendMessage(CUIWindow *pWnd, s16 msg, void* pData)
{
	if (&UIIdxList == pWnd && CUIListWnd::LIST_ITEM_CLICKED == msg)
	{
		// для начала проверим, что нажатый элемент не рутовый
		CUITreeViewItem *pTVItem = static_cast<CUITreeViewItem*>(pData);
		R_ASSERT(pTVItem);

		if (!pTVItem->IsRoot())
		{
			// Удаляем текущую картинку и текст
			if (m_pCurrArticle)
			{
				UIEncyclopediaInfoBkg.DetachChild(&m_pCurrArticle->data()->image);
				m_pCurrArticle->data()->image.SetMask(NULL);
			}
			UIInfoList.RemoveAll();

			// Отображаем новые
			CUIString str;
			str.SetText(*CStringTable()(*m_ArticlesDB[pTVItem->GetValue()]->data()->text));
			CUIStatic::PreprocessText(str.m_str, UIInfoList.GetItemWidth() - 5, UIInfoList.GetFont());
			UIInfoList.AddParsedItem<CUIListItem>(str, 0, 0xffffffff);
			UIEncyclopediaInfoBkg.AttachChild(&m_ArticlesDB[pTVItem->GetValue()]->data()->image);
			m_ArticlesDB[pTVItem->GetValue()]->data()->image.SetMask(&UIImgMask);

			if(!m_ArticlesDB[pTVItem->GetValue()]->data()->image.GetStaticItem()->GetShader())
				UIImgMask.Show(false);
			else
				UIImgMask.Show(true);

			std::string caption = static_cast<std::string>(*m_InfosHeaderStr) + pTVItem->GetHierarchyAsText();
			UIEncyclopediaInfoHeader.UITitleText.SetText(caption.c_str());
			caption.erase(0, caption.find_last_of("/") + 1);
			UIArticleHeader.SetText(caption.c_str());

			// Запоминаем текущий эдемент
			m_pCurrArticle = m_ArticlesDB[pTVItem->GetValue()];
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::RescaleStatic(CUIStatic &s)
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

void CUIEncyclopediaWnd::Draw()
{
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::Show()
{
	DeleteArticles();

	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor && pActor->encyclopedia_registry.objects_ptr())
	{
		for(ARTICLE_VECTOR::const_iterator it = pActor->encyclopedia_registry.objects_ptr()->begin();
			it != pActor->encyclopedia_registry.objects_ptr()->end(); it++)
		{
			AddArticle((*it).index);
		}
	}

	inherited::Show();
}