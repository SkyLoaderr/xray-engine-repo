///////////////////////////////////////////////////////////
/// —писок контактов PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"
#include "UIPdaListItem.h"

#include "../Pda.h"
#include "../HUDManager.h"



#define PDA_CONTACT_HEIGHT 70


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
			CObject* pObject = (CObject*)((CUIListItem*)pData)->GetData();
			R_ASSERT(pObject);
			m_idContact = pObject->ID();

			GetTop()->SendMessage(this, CONTACT_SELECTED);
		}
	}
	inherited::SendMessage(pWnd, msg, pData);
}

void CUIPdaContactsWnd::AddContact(CObject* pOwnerObject)
{
	VERIFY(pOwnerObject);

	CUIPdaListItem* pItem = NULL;
	pItem = xr_new<CUIPdaListItem>();
	UIListWnd.AddItem(pItem); 
	pItem->InitCharacter(dynamic_cast<CInventoryOwner*>(pOwnerObject));
	pItem->SetData(pOwnerObject);
}

void CUIPdaContactsWnd::RemoveContact(CObject* pOwnerObject)
{
	UIListWnd.RemoveItem(UIListWnd.FindItem(pOwnerObject));
}
//удалить все контакты из списка
void CUIPdaContactsWnd::RemoveAll()
{
	UIListWnd.RemoveAll();
}

bool CUIPdaContactsWnd::IsInList(CObject* pOwnerObject)
{
	if(UIListWnd.FindItem(pOwnerObject)==-1) 
		return false;
	else
		return true;
}