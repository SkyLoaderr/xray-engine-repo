//////////////////////////////////////////////////////////////////////
// UIListItem.cpp: элемент окна списка CListWnd
//////////////////////////////////////////////////////////////////////

#include"stdafx.h"

#include "UIlistitem.h"

CUIListItem::CUIListItem(void)
{
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;

	m_bButtonClicked = false;

	m_pData = NULL;

	m_iIndex = -1;
}

CUIListItem::~CUIListItem(void)
{
}

void CUIListItem::Init(char* str, int x, int y, int width, int height)
{
	CUIButton::Init(NULL, x,y, width, height);
	SetText(str);

	m_pData = NULL;
}

void CUIListItem::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	CUIButton::OnMouse(x, y, mouse_action);

	if(m_bButtonClicked)
		GetParent()->SendMessage(this, BUTTON_CLICKED);

}	


void CUIListItem::Draw()
{
	CUIButton::Draw();
}
