#include "StdAfx.h"
#include "UIListBoxItem.h"
#include "UIScrollView.h"

u32 CUIListBoxItem::uid_counter = 0;

CUIListBoxItem::CUIListBoxItem(){
	txt_color = 0xffffffff;
	txt_color_s = 0xffffffff;
	uid = uid_counter++;
	m_bTextureAvailable = false;
}

void CUIListBoxItem::SetID(u32 id){
	uid = id;
}

u32 CUIListBoxItem::GetID(){
	return uid;
}

void CUIListBoxItem::Draw(){
	m_bTextureAvailable = m_bSelected;
	CUILabel::Draw();	
}

void CUIListBoxItem::InitDefault(){
	InitTexture("ui_listline");
}

void CUIListBoxItem::OnMouseDown(bool left_button){
	if (left_button){
		smart_cast<CUIScrollView*>(GetParent()->GetParent())->SetSelected(this);
		GetMessageTarget()->SendMessage(this,LIST_ITEM_SELECT, &uid);
	}
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
