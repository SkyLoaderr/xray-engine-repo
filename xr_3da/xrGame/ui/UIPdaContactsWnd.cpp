///////////////////////////////////////////////////////////
/// Список контактов PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"

#include "..\\Pda.h"
#include "..\\HUDManager.h"


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

	AttachChild(&UIListWnd);
	UIListWnd.Init(10,10, width-10,height-10);

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
				m_idContact = pObject->ID();

				GetTop()->SendMessage(this, CONTACT_SELECTED);
			}
	}
	inherited::SendMessage(pWnd, msg, pData);
}
	