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
		// TEST!!!!
		UINewsWnd.UIListWnd.AddInteractiveItem("Hello from Haiti! %SOSO% %Interactive element%");
		UINewsWnd.UIListWnd.AddInteractiveItem("Hello from Haiti! %Test2% test %Ab%", 5, 0xffaa1230);
		// END TEST!!!
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
		// -- This is only for test purposes --
		CUIString str;
		u32 tmpi = *static_cast<u32*>(pData);
		str.SetText("This message should not to appear");

		CUIPdaWnd *pPda = dynamic_cast<CUIPdaWnd*>(GetMessageTarget());

		R_ASSERT(pPda);

		if (tmpi == 0)
		{
			str.SetText("SOSO pressed");
			pPda->FocusOnMap(100, 0, 500);
		}
		else if (tmpi == 1)
			str.SetText("jsd pressed");
		else if (tmpi == 2)
			str.SetText("Test2 PRESSED!!!!");
		else if (tmpi == 3)
			str.SetText("Ababalamaga OHAYO!");
		//		UINewInfoWnd.UIListWnd.AddInteractiveItem(str);
		// -- End <fortest> block --
	}
	inherited::SendMessage(pWnd, msg, pData);
}