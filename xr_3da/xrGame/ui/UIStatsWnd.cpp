#include "stdafx.h"

#include "UIStatsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../UI.h"
#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

const char * const STATS_XML = "stats.xml";

//////////////////////////////////////////////////////////////////////////

CUIStatsWnd::CUIStatsWnd()
	: m_uHighlightedItem(0xffffffff)
{
	Init();
}

//////////////////////////////////////////////////////////////////////////

CUIStatsWnd::~CUIStatsWnd()
{

}

//////////////////////////////////////////////////////////////////////////

void CUIStatsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", STATS_XML);
	R_ASSERT3(xml_result, "xml file not found", STATS_XML);

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	// Читаем из xml файла параметры окна и контролов
	AttachChild(&UIFrameWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWnd);

	UIFrameWnd.AttachChild(&UIStatsList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIStatsList);
	UIStatsList.SetMessageTarget(this);
	UIStatsList.EnableScrollBar(true);

	xml_init.InitMultiTextStatic(uiXml, "headers_mt_static", 0, &UIHeader);
	UIFrameWnd.AttachChild(&UIHeader);

	SetHeaderColumnText(0, "Name");
	SetHeaderColumnText(1, "Frags");
	SetHeaderColumnText(2, "Deaths");
	SetHeaderColumnText(3, "ping");
}

//////////////////////////////////////////////////////////////////////////

CUIStatsListItem * CUIStatsWnd::AddItem()
{
	CUIStatsListItem *pNewItem = xr_new<CUIStatsListItem>();
	UIStatsList.AddItem<CUIListItem>(pNewItem); 

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", STATS_XML);
	R_ASSERT(xml_result);

	pNewItem->XmlInit("list", uiXml);
//	pNewItem->SetMessageTarget(this);
	return pNewItem;
}

//////////////////////////////////////////////////////////////////////////

CUIStatsListItem * CUIStatsWnd::FindFrom(const u32 beg_pos, const char *strCaption)
{
	for (int i = 0; i < UIStatsList.GetSize(); ++i)
	{
		CUIStatsListItem *pSLItem = smart_cast<CUIStatsListItem*>(UIStatsList.GetItem(i));
		R_ASSERT(beg_pos < pSLItem->FieldsVector.size());
		for (FIELDS_VECTOR_it it = pSLItem->FieldsVector.begin() + beg_pos; it < pSLItem->FieldsVector.end(); ++it)
		{
			if (0 == xr_strcmp(strCaption, (*it)->GetText()))
			{
				return pSLItem;
			}
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatsWnd::RemoveItemFrom(const u32 beg_pos, const char *strCaption)
{
	if (CUIStatsListItem *pSLItem = FindFrom(beg_pos, strCaption))
	{
		UIStatsList.RemoveItem(pSLItem->GetIndex());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatsWnd::HighlightItem(const u32 uItem)
{
	R_ASSERT(static_cast<int>(uItem) < UIStatsList.GetSize());
	if (m_uHighlightedItem != uItem)
	{
		if (m_uHighlightedItem != 0xffffffff)
			smart_cast<CUIStatsListItem*>(UIStatsList.GetItem(m_uHighlightedItem))->Highlight(false);
		if (uItem != 0xffffffff)
			smart_cast<CUIStatsListItem*>(UIStatsList.GetItem(uItem))->Highlight(true);
		m_uHighlightedItem = uItem;
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatsWnd::SelectItem(const u32 uItem)
{
	R_ASSERT(static_cast<int>(uItem) < UIStatsList.GetSize());
	UIStatsList.SetFocusedItem(static_cast<signed int>(uItem));
}

//////////////////////////////////////////////////////////////////////////

void CUIStatsWnd::SetHeaderColumnText(const u32 headerItem, const shared_str &text)
{
	UIHeader.GetPhraseByIndex(headerItem)->str = CStringTable()(text);
}

//////////////////////////////////////////////////////////////////////////
//  CUIStatsListItem - класс элемента списка в листе
//////////////////////////////////////////////////////////////////////////

void CUIStatsListItem::XmlInit(const char *path, CUIXml &uiXml)
{
	CUIXmlInit	xml_init;
//	CUIStatic	*pStatic;
	CUIButton	*pButton;

	string256 buf;
	strconcat(buf, path, ":static");

	int tabsCount = uiXml.GetNodesNum(path, 0, "static");

	XML_NODE* tab_node = uiXml.NavigateToNode(path,0);
	uiXml.SetLocalRoot(tab_node);

	for (int i = 0; i < tabsCount; ++i)
	{
		pButton = xr_new<CUIButton>();
		xml_init.InitStatic(uiXml, "static", i, pButton);
		pButton->SetNewRenderMethod(true);
		AttachChild(pButton);
		FieldsVector.push_back(pButton);
	}

	FieldsVector[0]->SetElipsis(CUIStatic::eepEnd, 0);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatsListItem::Highlight(bool bHighlight)
{
	for (FIELDS_VECTOR_it it = FieldsVector.begin(); it != FieldsVector.end(); ++it)
	{
		(*it)->HighlightItem(bHighlight);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatsListItem::SetSubItemColor(u32 uItemIndex, u32 uColor)
{
	R_ASSERT(uItemIndex < FieldsVector.size());
	FieldsVector[uItemIndex]->SetTextColor(uColor);
}