// File:        UIComboBox.cpp
// Description: guess :)
// Created:     10.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
// 
// Copyright 2004 GSC Game World
//

#include "StdAfx.h"
#include "UIComboBox.h"
#include "UITextureMaster.h"

#define CB_HEIGHT 23.0f
#define BTN_SIZE  23.0f

CUIComboBox::CUIComboBox(){
	AttachChild(&m_frameLine);
	AttachChild(&m_text);

	AttachChild(&m_btn);

	AttachChild(&m_frameWnd);
	AttachChild(&m_list);

	m_iListHeight = 0;
	m_bInited = false;
	m_eState = LIST_FONDED;

	m_textColor[0] = 0xff00ff00;
}

CUIComboBox::~CUIComboBox(){

}

void CUIComboBox::SetListLength(int length){
	R_ASSERT(0 == m_iListHeight);
	m_iListHeight = length;
}

void CUIComboBox::SetVertScroll(bool bVScroll){
	m_list.SetFixedScrollBar(bVScroll);
}

void CUIComboBox::Init(float x, float y, float width){
	m_bInited = true;
	if (0 == m_iListHeight)
		m_iListHeight = 4;

	CUIWindow::Init(x, y, width, CB_HEIGHT);
	// Frame Line
	m_frameLine.Init(0, 0, width /*- BTN_SIZE*/, CB_HEIGHT);
	m_frameLine.InitEnabledState("ui_cb_linetext_e"); // horizontal by default
	m_frameLine.InitHighlightedState("ui_cb_linetext_h");


	// Edit Box on left side of frame line
	m_text.Init(0, 0, width - BTN_SIZE, CB_HEIGHT); 
	m_text.SetTextColor(m_textColor[0]);
	m_text.Enable(false);
	// Button on right side of frame line
	m_btn.Init("ui_cb_button", width - BTN_SIZE, 0, BTN_SIZE, BTN_SIZE);


	// height of list equal to height of ONE element
	float item_height = CUITextureMaster::GetTextureHeight("ui_cb_listline_b");
	m_list.Init(0, CB_HEIGHT, width - BTN_SIZE, item_height*m_iListHeight);
	m_list.Init();
	m_list.SetTextColor(m_textColor[0]);
	m_list.SetSelectionTexture("ui_cb_listline");
	m_list.SetItemHeight(CUITextureMaster::GetTextureHeight("ui_cb_listline_b"));
	// frame(texture) for list
	m_frameWnd.Init(0,  CB_HEIGHT, width - BTN_SIZE, m_list.GetItemHeight()*m_iListHeight);
	m_frameWnd.InitTexture("ui_cb_listbox");

	m_list.Show(false);
	m_frameWnd.Show(false);
}

void CUIComboBox::Init(float x, float y, float width, float height){
	this->Init(x, y, width);
}

void CUIComboBox::AddItem(LPCSTR str, bool bSelected){
	R_ASSERT2(m_bInited, "Can't add item to ComboBox before Initialization");
	m_list.AddItem(str);
	if (bSelected)
		m_text.SetText(str);
}

void CUIComboBox::AddItem(LPCSTR str){
    R_ASSERT2(m_bInited, "Can't add item to ComboBox before Initialization");
	m_list.AddItem(str);	
}

void CUIComboBox::Draw(){
	CUIWindow::Draw();
}

void CUIComboBox::Update(){
	CUIWindow::Update();
	if (!m_bIsEnabled)
	{
		SetState(S_Disabled);
		m_text.SetTextColor(m_textColor[1]);
	}
	else
		m_text.SetTextColor(m_textColor[0]);

}

void CUIComboBox::OnFocusLost(){
	CUIWindow::OnFocusLost();
	if (m_bIsEnabled)
        SetState(S_Enabled);

}

void CUIComboBox::OnFocusReceive(){
	CUIWindow::OnFocusReceive();
    if (m_bIsEnabled)
        SetState(S_Highlighted);
}

bool CUIComboBox::OnMouse(float x, float y, EUIMessages mouse_action){
	if(CUIWindow::OnMouse(x, y, mouse_action)) 
		return true;

	bool bCursorOverWindow = false;
//	Frect r = GetWndRect();
	bCursorOverWindow |= (0 <= x) && (GetWidth() >= x) && (0 <= y) && (GetHeight() >= y);
	/*r = m_frameWnd.GetWndRect();
	r.right += m_btn.GetWidth();
	bCursorOverWindow |= (r.left <= x) && (r.right >= x) && (r.top <= y) && (r.bottom >= y);		*/

	switch (m_eState){
		case LIST_EXPANDED:			

			if ((!bCursorOverWindow) && mouse_action == WINDOW_LBUTTON_DOWN)
			{
                ShowList(false);
				return true;
			}
			break;
		case LIST_FONDED:
			break;
		default:
			break;
	}	
	 

        return false;
}

void CUIComboBox::SetState(UIState state){
	m_frameLine.SetState(state);
}

void CUIComboBox::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	CUIWindow::SendMessage(pWnd, msg, pData);

	switch (msg){
		case BUTTON_CLICKED:
			if (pWnd == &m_btn || pWnd == &m_list)
				OnBtnClicked();
			break;
		case LIST_ITEM_CLICKED:
			if (pWnd == &m_list)
				OnListItemSelect();	
			break;
		default:
			break;
	}
}

void CUIComboBox::OnBtnClicked(){
	ShowList(!m_list.IsShown());
}

void CUIComboBox::ShowList(bool bShow){
    if (bShow)
	{
		SetHeight(m_text.GetHeight() + m_list.GetHeight());

		m_list.Show(true);
		m_frameWnd.Show(true);

		m_eState = LIST_EXPANDED;

		GetParent()->SetCapture(this, true);
	}
	else
	{
		m_list.Show(false);
		m_frameWnd.Show(false);
		SetHeight(m_frameLine.GetHeight());
		GetParent()->SetCapture(this, false);

		m_eState = LIST_FONDED;
	}
}

void CUIComboBox::OnListItemSelect(){
	m_text.SetText(m_list.GetSelectedText());    
	ShowList(false);
}


CUIListBox* CUIComboBox::GetListWnd(){
	return &m_list;
}

void CUIComboBox::SetCurrentValue(){
	m_list.Clear();
	xr_token* tok = GetOptToken();

	while (tok->name){		
		AddItem(tok->name);
		tok++;
	}

	m_text.SetText(GetOptTokenValue());
	m_list.SetSelected(m_text.GetText());
}

void CUIComboBox::SaveValue(){
	CUIOptionsItem::SaveValue();
	SaveOptTokenValue(m_text.GetText());
}

bool CUIComboBox::IsChanged(){
	return 0 != xr_strcmp(GetOptTokenValue(), m_text.GetText());
}

LPCSTR CUIComboBox::GetText(){
	return m_text.GetText();
}

void CUIComboBox::SetItem(int i){
	m_list.SetSelected(i);
	m_text.SetText(m_list.GetSelectedText());
	
}

void CUIComboBox::SetTextColor(u32 color){
	m_textColor[0] = color;
}

void CUIComboBox::SetTextColorD(u32 color){
	m_textColor[1] = color;
}