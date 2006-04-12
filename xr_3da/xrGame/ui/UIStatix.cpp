#include "stdafx.h"

#include "UIStatix.h"
#include "UIColorAnimatorWrapper.h"

CUIStatix::CUIStatix(){
	m_anim = xr_new<CUIColorAnimatorWrapper>("ui_slow_blinking");
	m_anim->Cyclic(true);

	m_bSelected = false;
}

CUIStatix::~CUIStatix(){
	xr_delete(m_anim);
}

void CUIStatix::Update(){
	CUIStatic::Update();
	if (m_bCursorOverWindow)
	{
		SetColor(0xff349F06);
	}
	if (m_bSelected)
	{
		m_anim->Update();
		SetColor(subst_alpha(GetColor(), color_get_A(m_anim->GetColor())));
	}
}

void CUIStatix::OnFocusLost()
{
	CUIStatic::OnFocusLost	();
	SetColor				(0xffffffff);
}

void CUIStatix::OnFocusReceive()
{
	CUIStatic::OnFocusReceive	();
	m_anim->Reset();
}

bool CUIStatix::OnMouseDown(bool left_button /* = true */){
	GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
	return true;
}

void CUIStatix::SetSelectedState(bool state){
	m_bSelected = state;
	if (!state)
		OnFocusLost();
}

bool CUIStatix::GetSelectedState(){
	return m_bSelected;
}
