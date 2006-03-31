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
	AttachChild(&this->m_frameLine);
	AttachChild(&this->m_text);

	AttachChild(&this->m_btn);

	AttachChild(&this->m_frameWnd);
	AttachChild(&this->m_list);

	m_iListHeight = 0;
	m_bInited = false;
	m_eState = LIST_FONDED;
}

CUIComboBox::~CUIComboBox(){

}

void CUIComboBox::SetListLength(int length){
	R_ASSERT(0 == m_iListHeight);
	m_iListHeight = length;
}

void CUIComboBox::SetVertScroll(bool bVScroll){
	this->m_list.SetFixedScrollBar(bVScroll);
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
	m_text.SetTextColor(0xff00ff00);
	m_text.Enable(false);
	// Button on right side of frame line
	m_btn.Init("ui_cb_button", width - BTN_SIZE, 0, BTN_SIZE, BTN_SIZE);


	// height of list equal to height of ONE element
	float item_height = CUITextureMaster::GetTextureHeight("ui_cb_listline_b");
	m_list.Init(0, CB_HEIGHT, width - BTN_SIZE, item_height*m_iListHeight);
	m_list.Init();
	m_list.SetTextColor(0xff00ff00);
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
        	
//	CUIListItem *item = xr_new<CUIListItem>();
//	item->SetText(str);
//	item->SetAutoDelete(true);
	m_list.AddItem(str);
	if (bSelected)
		m_text.SetText(str);
}

void CUIComboBox::AddItem(LPCSTR str){
    R_ASSERT2(m_bInited, "Can't add item to ComboBox before Initialization");

//	CUIListItem *item = xr_new<CUIListItem>();
//	item->SetAutoDelete(true);
//	item->SetText(str);
	m_list.AddItem(str);	
}

void CUIComboBox::Draw(){
	CUIWindow::Draw();
}

void CUIComboBox::Update(){
	CUIWindow::Update();
	if (!m_bIsEnabled)
		SetState(S_Disabled);
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
    if(CUIWindow::OnMouse(x, y, mouse_action)) return true;

	bool CursorOverWindow = (0 <= x) && (GetWidth() >= x) && (0 <= y) && (GetHeight() >= y);

//	SetState(m_bCursorOverWindow ? S_Highlighted : S_Enabled);
	
//	if(mouse_action == WINDOW_MOUSE_MOVE && m_eState == LIST_FONDED)
//		GetParent()->SetCapture(this, m_bCursorOverWindow);

	switch (m_eState){
		case LIST_EXPANDED:			

			if ((!CursorOverWindow) && mouse_action == WINDOW_LBUTTON_DOWN)
			{
                ShowList(false);
//                GetParent()->SetCapture(this, false);
				return true;
			}
//			else{
//				GetParent()->SetCapture(this, true);
//				return true;
//			}
			break;
		case LIST_FONDED:
			break;
		default:
			break;
	}	
	return false;
}

void CUIComboBox::SetState(UIState state){
	this->m_frameLine.SetState(state);
//	this->m_frameWnd.SetState(state);	
}

void CUIComboBox::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	CUIWindow::SendMessage(pWnd, msg, pData);

	switch (msg){
		case BUTTON_CLICKED:
			if (pWnd == &this->m_btn || pWnd == &this->m_list)
				OnBtnClicked();
			break;
		case LIST_ITEM_CLICKED:
			if (pWnd == &this->m_list)
				this->OnListItemSelect();	
			break;
		default:
			break;
	}
}

void CUIComboBox::OnBtnClicked(){
	ShowList(!this->m_list.IsShown());
}

void CUIComboBox::ShowList(bool bShow){
    if (bShow)
	{
//		int iCurHeight;

//		if (m_list.GetChildNum() <= this->m_iListHeight)
//			iCurHeight = iFloor(m_iListHeight*CB_HEIGHT);
//		else
//			iCurHeight = iFloor(m_list.GetChildNum()*CB_HEIGHT);

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
		this->SetHeight(this->m_frameLine.GetHeight());
		GetParent()->SetCapture(this, false);

		m_eState = LIST_FONDED;
	}
}

void CUIComboBox::OnListItemSelect(){
//	int selItem = m_list.GetFocusedItem();
//	CUIListItem* item = m_list.GetItem(selItem);
//	if (item)
	m_text.SetText(m_list.GetSelectedText());    

	ShowList(false);
//	SetCapture(&m_list, false);
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