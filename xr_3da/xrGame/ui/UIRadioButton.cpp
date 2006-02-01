//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: ����� ������, ������� 2 ���������
// � ���������� � ������ � ������ �� ��������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiobutton.h"
#include "UILines.h"


void CUIRadioButton::Init(float x, float y, float width, float height){
	if (!m_pLines)
		m_pLines = xr_new<CUILines>();
	m_pLines->SetTextAlignment(CGameFont::alLeft);
    CUI3tButton::InitTexture("ui_radio");
	Frect r = m_background.GetE()->GetStaticItem()->GetRect(); 
	CUI3tButton::SetTextX(r.width());
    CUI3tButton::Init(x,y, width, r.height() - 5);
	m_pLines->Init(x,y,width,m_background.GetE()->GetStaticItem()->GetRect().height());
}

void CUIRadioButton::InitTexture(LPCSTR tex_name){
	// do nothing
}
