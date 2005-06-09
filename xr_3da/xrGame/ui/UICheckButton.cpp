// UICheckButton.cpp: ����� ������, ������� 2 ���������:
// � �������� � ���
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uicheckbutton.h"
#include "../HUDManager.h"

CUICheckButton::CUICheckButton(void)
{	
	m_lines.SetTextAlignment(CGameFont::alLeft);
}

CUICheckButton::~CUICheckButton(void)
{
}

void CUICheckButton::Init(int x, int y, int width, int height){
	CUIWindow::Init(x,y,width,height);
	InitTexture();
	m_lines.Init(x,y,width,m_background.GetE()->GetStaticItem()->GetRect().height());
}

void CUICheckButton::InitTexture(){
	CUI3tButton::InitTexture("ui_checker");
	Irect r = m_background.GetE()->GetStaticItem()->GetOriginalRect();
	CUI3tButton::SetTextX(r.width());	
}

void CUICheckButton::OnMouse(int x, int y, EUIMessages mouse_action)
{
	CUIWindow::OnMouse(x, y, mouse_action);
}

void CUICheckButton::OnMouseDown(bool left_button){
	if (left_button)
	{
		if (m_eButtonState == BUTTON_NORMAL)
			m_eButtonState = BUTTON_PUSHED;
		else
			m_eButtonState = BUTTON_NORMAL;
	}
}
