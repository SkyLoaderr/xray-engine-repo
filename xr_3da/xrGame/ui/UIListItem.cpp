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
	m_iValue = 0;
	m_bHighlightText = false;
	m_iGroupID = -1;
	m_bManualDelete = false;

	SetTextAlign(CGameFont::alLeft);
}

CUIListItem::~CUIListItem(void)
{
}

void CUIListItem::Init(int x, int y, int width, int height)
{
	inherited::Init(NULL, x, y, width, height);


	SetPressMode(CUIButton::DOWN_PRESS);
	SetPushOffsetX(0);
	SetPushOffsetY(0);
}


void CUIListItem::Init(const char* str, int x, int y, int width, int height)
{
	SetText(str);
	Init(x,y,width, height);
}

void CUIListItem::OnMouse(int x, int y, EUIMessages mouse_action)
{
	CUIButton::OnMouse(x, y, mouse_action);
}	


void CUIListItem::Draw()
{
	CUIButton::Draw();
}

int CUIListItem::GetSignWidht()
{
	return (int)GetFont()->SizeOf(m_str);
}

bool CUIListItem::IsHighlightText()
{
	return m_bHighlightText;
}
