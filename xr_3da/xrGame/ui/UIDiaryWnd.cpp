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

	AttachChild(&UINewsWnd);
	UINewsWnd.Init();
	UINewsWnd.Hide();

	AttachChild(&UIJobsWnd);
	UIJobsWnd.Init();
	UIJobsWnd.Hide();

	AttachChild(&UINotesWnd);
	UINotesWnd.Init();
	UINotesWnd.Hide();

	switch (UITabControl.GetActiveIndex())
	{
	case 0:
		m_pActiveSubdialog = &UIJobsWnd;
		break;
	case 1:
		m_pActiveSubdialog = &UINewsWnd;
		break;
	case 2:
		m_pActiveSubdialog = &UINotesWnd;
		break;
	}

#pragma todo("Dandy to Vortex: Call Show() in Init is meaning less!")
	//m_pActiveSubdialog->Show();

	// Табконтрол поддиалогов
	AttachChild(&UITabControl);
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

	UINewsWnd.UIListWnd.AddParsedItem<CUIListItem>(str, 4, 0xffffffff);

	UINewsWnd.UIListWnd.AddItem<CUIListItem>("");
}
