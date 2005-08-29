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
	m_bHighlightText = false;
	m_iGroupID = -1;
	SetAutoDelete(true);
	SetTextAlignment(CGameFont::alLeft);
}

CUIListItem::~CUIListItem(void)
{
}

void CUIListItem::Init(float x, float y, float width, float height)
{
	inherited::Init(x, y, width, height);
	SetPressMode(CUIButton::DOWN_PRESS);
	SetPushOffsetX(0);
	SetPushOffsetY(0);
}

#pragma todo("SATAN -> ANDY : this is is the way to set icon to list item")
void CUIListItem::InitTexture(LPCSTR tex_name){
	CUIButton::InitTexture(tex_name);
	SetTextX(m_UIStaticItem.GetRect().width());
}


void CUIListItem::Init(const char* str, float x, float y, float width, float height)
{
	Init(x,y,width, height);
	SetText(str);	
}

bool CUIListItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
	return CUIButton::OnMouse(x, y, mouse_action);
}	


void CUIListItem::Draw()
{
	CUIButton::Draw();
}

int CUIListItem::GetSignWidht()
{
	return (int)GetFont()->SizeOf(GetText());
}

bool CUIListItem::IsHighlightText()
{
	return CUIButton::IsHighlightText();
}
