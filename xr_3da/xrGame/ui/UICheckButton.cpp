// UICheckButton.cpp: ����� ������, ������� 2 ���������:
// � �������� � ���
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uicheckbutton.h"
#include "../HUDManager.h"
#include "UILines.h"

CUICheckButton::CUICheckButton(void)
{	
	SetTextAlignment(CGameFont::alLeft);
	m_bCheckMode = true;
	m_pDependControl = NULL;
}

CUICheckButton::~CUICheckButton(void)
{
}

void CUICheckButton::SetDependControl(CUIWindow* pWnd){
	m_pDependControl = pWnd;
}

void CUICheckButton::Update(){
	CUI3tButton::Update();

	if (m_pDependControl)
		m_pDependControl->Enable(GetCheck());
}


void CUICheckButton::SetCurrentValue(){
	SetCheck(GetOptBoolValue());
}

void CUICheckButton::SaveValue(){
	CUIOptionsItem::SaveValue();
	SaveOptBoolValue(GetCheck());
}

bool CUICheckButton::IsChanged(){
	return GetOptBoolValue() != GetCheck();
}

void CUICheckButton::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	InitTexture();
	m_pLines->Init(x,y,width,m_background.GetE()->GetStaticItem()->GetRect().height());
}

void CUICheckButton::InitTexture()
{
	CUI3tButton::InitTexture("ui_checker");
	Frect r = m_background.GetE()->GetStaticItem()->GetOriginalRect();
	CUI3tButton::SetTextX(r.width());	
}
