//=============================================================================
//  Filename:   UINewsWnd.cpp
//  News subwindow in PDA dialog
//=============================================================================


#include "stdafx.h"
#include "UINewsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIString.h"
#include "../UI.h"

const char * const	NEWS_XML			= "news.xml";

//////////////////////////////////////////////////////////////////////////

CUINewsWnd::CUINewsWnd()
{
	// Remove this for enable interactive behaviour of list
	UIListWnd.ActivateList(false);
	SetWindowName("News");
}

//////////////////////////////////////////////////////////////////////////

CUINewsWnd::~CUINewsWnd()
{

}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", NEWS_XML);
	R_ASSERT3(xml_result, "xml file not found", NEWS_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.ActivateList(true);
	UIListWnd.EnableScrollBar(true);
}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::SendMessage(CUIWindow *pWnd, s16 msg, void* pData)
{
//	CUITreeViewItem *pItem = dynamic_cast<CUITreeViewItem*>(pWnd);
//	if (pItem && CUIListWnd::LIST_ITEM_CLICKED == msg)
//	{
//		if (pItem->IsRoot())
//		{
//			pItem->IsOpened() ? pItem->Close() : pItem->Open();
//		}
//	}
//
//	inherited::SendMessage(pWnd, msg, pData);
}