///////////////////////////////////////////////////////////
/// —писок контактов PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"
#include "UIPdaListItem.h"

#include "../Pda.h"
#include "../HUDManager.h"



#define PDA_CONTACT_HEIGHT 70

const char * const PDA_CONTACTS_XML	= "pda_contacts.xml";


CUIPdaContactsWnd::CUIPdaContactsWnd()
{
}

CUIPdaContactsWnd::~CUIPdaContactsWnd()
{
}

void CUIPdaContactsWnd::Show()
{
	inherited::Show(true);
	inherited::Enable(true);
	UIListWnd.Reset();
}
void CUIPdaContactsWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}


void CUIPdaContactsWnd::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result =uiXml.Init("$game_data$", PDA_CONTACTS_XML);
	R_ASSERT2(xml_result, "xml file not found");
	CUIXmlInit xml_init;

	AttachChild(&UIFrameContacts);
	xml_init.InitFrameWindow(uiXml, "contacts_frame_window", 0, &UIFrameContacts);

	AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.EnableActiveBackground(true);
	UIListWnd.EnableScrollBar(true);
}


void CUIPdaContactsWnd::Update()
{
	inherited::Update();
}


void CUIPdaContactsWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIListWnd)
	{
		if(msg == CUIListWnd::LIST_ITEM_CLICKED)
		{
			CPda* pda = (CPda*)((CUIListItem*)pData)->GetData();
			R_ASSERT(pda);
			m_idContact = pda->GetOriginalOwnerID();

			GetTop()->SendMessage(this, CONTACT_SELECTED);
		}
	}
	inherited::SendMessage(pWnd, msg, pData);
}

void CUIPdaContactsWnd::AddContact(CPda* pda)
{
	VERIFY(pda);

	CUIPdaListItem* pItem = NULL;
	pItem = xr_new<CUIPdaListItem>();
	UIListWnd.AddItem(pItem); 
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