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