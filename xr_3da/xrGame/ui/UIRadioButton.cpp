//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: ����� ������, ������� 2 ���������
// � ���������� � ������ � ������ �� ��������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiobutton.h"


void CUIRadioButton::Init(int x, int y, int width, int height){
    CUI3tButton::InitTexture("ui_radio");
	Irect r = m_background.GetE()->GetStaticItem()->GetRect(); 
	CUI3tButton::SetTextX(r.width());
    CUI3tButton::Init(x,y, width, r.height());
	m_lines.Init(x,y,width,m_background.GetE()->GetStaticItem()->GetRect().height());
}

void CUIRadioButton::InitTexture(LPCSTR tex_name){
	// do nothing
}
