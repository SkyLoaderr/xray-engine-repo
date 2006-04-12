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

//	for (int i = 0; i<fields.size(); i++)
//		fields[i].Draw();
}

void CUIListBoxItem::InitDefault(){
	InitTexture("ui_listline");
}

bool CUIListBoxItem::OnMouseDown(bool left_button){
	if (left_button){
		smart_cast<CUIScrollView*>(GetParent()->GetParent())->SetSelected(this);
		GetMessageTarget()->SendMessage(this,LIST_ITEM_SELECT, &uid);
		return true;
	}else
		return false;
}

void CUIListBoxItem::SetSelected(bool b){
	CUISelectable::SetSelected(b);
	u32 col;
	if (b)
		col = txt_color_s;	
	else
		col = txt_color;

	SetTextColor(col);
	for (u32 i = 0; i<fields.size(); i++)
		fields[i].SetTextColor(col);
}

void CUIListBoxItem::SetTextColor(u32 color, u32 color_s){
	txt_color = color;
	txt_color_s = color_s;
	SetTextColor(color);
}

float CUIListBoxItem::FieldsLength(){
	float c = 0;
	for (u32 i = 0; i<fields.size(); i++)
		c += fields[i].GetWidth();
	return c;
}

CGameFont* CUIListBoxItem::GetFont(){
	return CUILinesOwner::GetFont();
}

CUIStatic* CUIListBoxItem::AddField(LPCSTR txt, float len, LPCSTR key){
	fields.resize(fields.size()+1);
	CUIStatic& st = fields.back();
	AttachChild(&st);
	st.Init(FieldsLength(),0, GetWidth(), len);
	st.SetFont(GetFont());
	st.SetTextAlignment(GetTextAlignment());
	st.SetVTextAlignment(m_lines.GetVTextAlignment());
	st.SetTextColor(GetTextColor());
	st.SetText(txt);	
	st.SetWindowName(key);

	return &st;
}

LPCSTR CUIListBoxItem::GetField(LPCSTR key){
	for (u32 i = 0; i<fields.size(); i++)
	{
		if (0 == xr_strcmp(fields[i].WindowName(),key))
			return fields[i].GetText();
	}
	return NULL;
}
