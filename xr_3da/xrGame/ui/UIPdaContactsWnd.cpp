///////////////////////////////////////////////////////////
/// —писок контактов PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"
#include "UIPdaListItem.h"

#include "../Pda.h"
#include "../HUDManager.h"

#define PDA_CONTACT_HEIGHT 70

const char * const PDA_CONTACTS_XML					= "pda_contacts_new.xml";
const char * const PDA_CONTACTS_HEADER_SUFFIX		= "/Contacts";

CUIPdaContactsWnd::CUIPdaContactsWnd()
{
}

CUIPdaContactsWnd::~CUIPdaContactsWnd()
{
}

void CUIPdaContactsWnd::Show(bool status)
{
	inherited::Show(status);
	inherited::Enable(status);

	if (status)
		UIListWnd.Reset();
}

void CUIPdaContactsWnd::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);

	CUIXml		uiXml;
	bool xml_result =uiXml.Init("$game_data$", PDA_CONTACTS_XML);
	R_ASSERT3(xml_result, "xml file not found", PDA_CONTACTS_XML);

	CUIXmlInit	xml_init;
	string128	buf;

	AttachChild(&UIFrameContacts);
	xml_init.InitFrameWindow(uiXml, "left_frame_window", 0, &UIFrameContacts);

	UIFrameContacts.AttachChild(&UIContactsHeader);
	xml_init.InitFrameLine(uiXml, "left_frame_line", 0, &UIContactsHeader);
	strconcat(buf, ALL_PDA_HEADER_PREFIX, PDA_CONTACTS_HEADER_SUFFIX);
	UIContactsHeader.UITitleText.SetText(buf);

	AttachChild(&UIRightFrame);
	xml_init.InitFrameWindow(uiXml, "right_frame_window", 0, &UIRightFrame);

	UIRightFrame.AttachChild(&UIRightFrameHeader);
	xml_init.InitFrameLine(uiXml, "right_frame_line", 0, &UIRightFrameHeader);

	UIFrameContacts.AttachChild(&UIArticleHeader);
	xml_init.InitStatic(uiXml, "article_header_static", 0, &UIArticleHeader);
	UIArticleHeader.SetText("Contacts");

	UIRightFrameHeader.AttachChild(&UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimation);

	UIFrameContacts.AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.SetMessageTarget(this);
	UIListWnd.EnableActiveBackground(false);
	UIListWnd.EnableScrollBar(true);

	xml_init.InitAutoStatic(uiXml, "left_auto_static", &UIFrameContacts);
	xml_init.InitAutoStatic(uiXml, "right_auto_static", &UIRightFrame);
}


void CUIPdaContactsWnd::Update()
{
	inherited::Update();
}

void CUIPdaContactsWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIListWnd)
	{
		if(msg == LIST_ITEM_CLICKED)
		{
			CPda* pda = (CPda*)((CUIListItem*)pData)->GetData();
			R_ASSERT(pda);
			m_idContact = pda->GetOriginalOwnerID();

			GetTop()->SendMessage(this, PDA_CONTACTS_WND_CONTACT_SELECTED);
		}
	}

	if (pWnd->GetParent() == &UIListWnd && BUTTON_FOCUS_RECEIVED)
	{
		CUIPdaListItem *pPLIItem = smart_cast<CUIPdaListItem*>(pWnd);
		R_ASSERT(pPLIItem);
	}

	inherited::SendMessage(pWnd, msg, pData);
}

void CUIPdaContactsWnd::AddContact(CPda* pda)
{
	VERIFY(pda);


	if(!pda->GetOwnerObject())
	{
		Msg("[PDA] pda %d, has contacted without a parent", pda->ID());
		return;
	}
		
	CUIPdaListItem* pItem = NULL;
	pItem = xr_new<CUIPdaListItem>();
	UIListWnd.AddItem<CUIListItem>(pItem); 
	pItem->InitCharacter(pda->GetOriginalOwner());
	pItem->SetData(pda);
}

void CUIPdaContactsWnd::RemoveContact(CPda* pda)
{
	UIListWnd.RemoveItem(UIListWnd.FindItem(pda));
}
//удалить все контакты из списка
void CUIPdaContactsWnd::RemoveAll()
{
	UIListWnd.RemoveAll();
}

bool CUIPdaContactsWnd::IsInList(CPda* pda)
{
	if(UIListWnd.FindItem(pda)==-1) 
		return false;
	else
		return true;
}