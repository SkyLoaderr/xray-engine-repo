// CUIDiaryWnd.cpp:  дневник и все что с ним связано
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIDiaryWnd.h"
#include "UIInteractiveListItem.h"
#include "UIPdaWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

const char * const DIARY_XML = "diary.xml";

//-----------------------------------------------------------------------------/
//	Default constructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::CUIDiaryWnd()
	: m_pActiveSubdialog(NULL)
{
	Hide();
}

//-----------------------------------------------------------------------------/
//  Destructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::~CUIDiaryWnd()
{

}

void CUIDiaryWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$", DIARY_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	XML_NODE* tab_node;

	xml_init.InitWindow(uiXml, "news_window", 0, &UINewsWnd);
	tab_node = uiXml.NavigateToNode("news_window", 0);
	uiXml.SetLocalRoot(tab_node);
	UINewsWnd.Init(uiXml, this);
	UINewsWnd.Hide();
	AttachChild(&UINewsWnd);

	uiXml.SetLocalRoot(uiXml.GetRoot());
	xml_init.InitWindow(uiXml, "jobs_window", 0, &UIJobsWnd);
	tab_node = uiXml.NavigateToNode("jobs_window", 0);
	uiXml.SetLocalRoot(tab_node);
	UIJobsWnd.Init(uiXml, this);
	UIJobsWnd.Hide();
	AttachChild(&UIJobsWnd);

	uiXml.SetLocalRoot(uiXml.GetRoot());
	xml_init.InitWindow(uiXml, "notes_window", 0, &UINotesWnd);
	tab_node = uiXml.NavigateToNode("notes_window", 0);
	uiXml.SetLocalRoot(tab_node);
	UINotesWnd.Init(uiXml, this);
	UINotesWnd.Hide();
	AttachChild(&UINotesWnd);

	switch (UITabControl.GetActiveIndex())
	{
	case 0:
		m_pActiveSubdialog = &UINewsWnd;
		break;
	case 1:
		m_pActiveSubdialog = &UIJobsWnd;
		break;
	case 2:
		m_pActiveSubdialog = &UINotesWnd;
		break;
	}

	m_pActiveSubdialog->Show();

	// Табконтрол поддиалогов
	AttachChild(&UITabControl);
	uiXml.SetLocalRoot(uiXml.GetRoot());
	xml_init.InitTabControl(uiXml, "tab", 0, &UITabControl);
}

void CUIDiaryWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pWnd == &UITabControl && CUITabControl::TAB_CHANGED == msg)
	{
		m_pActiveSubdialog->Hide();

		switch (UITabControl.GetActiveIndex())
		{
		case 0:
			m_pActiveSubdialog = &UINewsWnd;
			break;
		case 1:
			m_pActiveSubdialog = &UIJobsWnd;
			break;
		case 2:
			m_pActiveSubdialog = &UINotesWnd;
			break;
		}

		m_pActiveSubdialog->Reset();
		m_pActiveSubdialog->Show();
	}
	else if (&UINewsWnd == pWnd && 
		CUIInteractiveListItem::INTERACTIVE_ITEM_CLICK == msg)
	{
//		u32 tmpi = *static_cast<u32*>(pData);

//		for (u32 i = 0; i < IDs.size(); ++i)
//		{
//			if (IDs[i] == tmpi)
//				pPda->FocusOnMap(100, 0, 500);
//		}
	}
	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::AddNewsItem(const char *sData)
{
	CUIString			str;
	str.SetText(sData);

	UINewsWnd.UIListWnd.AddParsedItem(str, 4, 0xffffffff);

	UINewsWnd.UIListWnd.AddItem("");
}
