#include "stdafx.h"

#include "UIStatix.h"
#include "UIColorAnimatorWrapper.h"

CUIStatix::CUIStatix(){
	m_anim = xr_new<CUIColorAnimatorWrapper>("ui_map_area_anim");
	m_anim->Cyclic(true);
}

CUIStatix::~CUIStatix(){
	xr_delete(m_anim);
}

void CUIStatix::Update(){
	CUIStatic::Update();
	if (m_bCursorOverWindow)
	{
		m_anim->Update();
		SetColor(subst_alpha(0xffffffff, color_get_A(m_anim->GetColor())));
	}	
}

void CUIStatix::OnFocusLost(){
	SetColor(0xffffffff);
}

void CUIStatix::OnFocusReceive(){
	m_anim->Reset();
}

void CUIStatix::OnMouseDown(bool left_button /* = true */){
	GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
}

