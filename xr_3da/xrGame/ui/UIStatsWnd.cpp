#include "stdafx.h"

#include "UIStatsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
//-----------------------------------------------------------------------------/
//  Ctor and Dtor
//-----------------------------------------------------------------------------/

const char * const STATS_XML = "stats.xml";

CUIStatsWnd::CUIStatsWnd()
	: m_uHighlightedItem(0)
{
	///Init();
}

CUIStatsWnd::~CUIStatsWnd()
{

}

void CUIStatsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", STATS_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	// ������ �� xml ����� ��������� ���� � ���������
	AttachChild(&UIFrameWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWnd);

	UIFrameWnd.AttachChild(&UIStatsList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIStatsList);
	UIStatsList.SetMessageTarget(this);
	UIStatsList.EnableScrollBar(true);
	UIStatsList.EnableActiveBackground(true);

//	UIFrameWnd.AttachChild(&UIBtn);
	xml_init.InitButton(uiXml, "button", 0, &UIBtn);

	// TEST!!! ������� ��������� ��������
	/*
	CUIStatsListItem *pSLItem;

	pSLItem = AddItem();
	pSLItem->FieldsVector[0]->SetText("NAME: ");
	pSLItem->FieldsVector[1]->SetText("DreamCatcher");
	pSLItem->FieldsVector[2]->SetText("Frags: 0");

	pSLItem = AddItem();
	pSLItem->FieldsVector[0]->SetText("NAME: ");
	pSLItem->FieldsVector[1]->SetText("Piggy");
	pSLItem->FieldsVector[2]->SetText("Frags: 5");

	pSLItem = AddItem();
	pSLItem->FieldsVector[0]->SetText("NAME: ");
	pSLItem->FieldsVector[1]->SetText("Hubba-Bubba");
	pSLItem->FieldsVector[2]->SetText("Frags: 6");

	HighlightItem(1);
	*/

}

CUIStatsListItem * CUIStatsWnd::AddItem()
{
	CUIStatsListItem *pNewItem = xr_new<CUIStatsListItem>();
	UIStatsList.AddItem(pNewItem); 

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", STATS_XML);
	R_ASSERT(xml_result);

	pNewItem->XmlInit("list", uiXml);
//	pNewItem->SetMessageTarget(this);
	return pNewItem;
}

CUIStatsListItem * CUIStatsWnd::FindFrom(const u32 beg_pos, const char *strCaption)
{
	for (int i = 0; i < UIStatsList.GetSize(); ++i)
	{
		CUIStatsListItem *pSLItem = dynamic_cast<CUIStatsListItem*>(UIStatsList.GetItem(i));
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

void CUIStatsWnd::RemoveItemFrom(const u32 beg_pos, const char *strCaption)
{
	if (CUIStatsListItem *pSLItem = FindFrom(beg_pos, strCaption))
	{
		UIStatsList.RemoveItem(pSLItem->GetIndex());
	}
}

//void CUIStatsWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
//{
//	if (CUIListItem::LBUTTON_DOWN == msg)
//	{
//		if (dynamic_cast<CUIStatsListItem*>(pWnd))
//			HighlightItem(dynamic_cast<CUIStatsListItem*>(pWnd)->GetIndex());
//		
//			RemoveItemFrom(1, "Piggy");
//			CUIStatsListItem *pSLItem2 = ReFrom(0, "DreamCatcher");
//			CUIStatsListItem *pSLItem = AddItem();
//
//			pSLItem->FieldsVector[0]->SetText(pSLItem2->FieldsVector[0]->GetText());
//			pSLItem->FieldsVector[1]->SetText(pSLItem2->FieldsVector[1]->GetText());
//			pSLItem->FieldsVector[2]->SetText(pSLItem2->FieldsVector[2]->GetText());
//	}
//	inherited::SendMessage(pWnd, msg, pData);
//}

void CUIStatsWnd::HighlightItem(const u32 uItem)
{
	R_ASSERT(static_cast<int>(uItem) < UIStatsList.GetSize());
	if (m_uHighlightedItem != uItem)
	{
		dynamic_cast<CUIStatsListItem*>(UIStatsList.GetItem(m_uHighlightedItem))->Highlight(false);
		dynamic_cast<CUIStatsListItem*>(UIStatsList.GetItem(uItem))->Highlight(true);
		m_uHighlightedItem = uItem;
	}
}

//-----------------------------------------------------------------------------/
//  CUIStatsListItem - ��������� ����� �������� ������ � �����
//-----------------------------------------------------------------------------/
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
		AttachChild(pButton);
		FieldsVector.push_back(pButton);
	}
}

void CUIStatsListItem::Highlight(bool bHighlight)
{
	for (FIELDS_VECTOR_it it = FieldsVector.begin(); it != FieldsVector.end(); ++it)
	{
		(*it)->HighlightItem(bHighlight);
	}
}