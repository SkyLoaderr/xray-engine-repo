//////////////////////////////////////////////////////////////////////
// UIListItem.cpp: ������� ���� ������ CListWnd
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

	SetTextAlign(CGameFont::alLeft);
}

CUIListItem::~CUIListItem(void)
{
}

void CUIListItem::Init(char* str, int x, int y, int width, int height)
{
	CUIButton::Init(NULL, x,y, width, height);
	SetText(str);

	m_pData = NULL;

	SetPressMode(CUIButton::DOWN_PRESS);
	this->SetPushOffsetX(0);
	this->SetPushOffsetY(0);
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

int CUIListItem::GetSignWidht()
{
	return (int)GetFont()->SizeOf(m_str);
}
