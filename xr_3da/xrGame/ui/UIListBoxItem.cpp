#include "StdAfx.h"
#include "UIListBoxItem.h"
#include "UIScrollView.h"

CUIListBoxItem::CUIListBoxItem(){
	txt_color = 0xffffffff;
	txt_color_s = 0xffffffff;
}

void CUIListBoxItem::Update(){
	CUILabel::Update();
	m_bTextureAvailable = m_bSelected;
}

void CUIListBoxItem::InitDefault(){
	InitTexture("ui_listline");
}

void CUIListBoxItem::OnMouseDown(bool left_button){
	if (left_button)
		smart_cast<CUIScrollView*>(GetParent()->GetParent())->SetSelected(this);
}

void CUIListBoxItem::SetSelected(bool b){
	CUISelectable::SetSelected(b);
	if (b)
		SetTextColor(txt_color_s);
	else
		SetTextColor(txt_color);
}

void CUIListBoxItem::SetTextColor(u32 color, u32 color_s){
	txt_color = color;
	txt_color_s = color_s;
	SetTextColor(color);
}
