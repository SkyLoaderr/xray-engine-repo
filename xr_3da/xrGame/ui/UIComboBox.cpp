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

#define CB_HEIGHT 19
#define BTN_SIZE  23

CUIComboBox::CUIComboBox(){
	// edit box
	AttachChild(&this->m_frameLine);
	AttachChild(&this->m_editBox);

	// button
	AttachChild(&this->m_btn);

	// list
	AttachChild(&this->m_frameWnd);
	AttachChild(&this->m_list);

	this->m_iListHeight = 0;
	this->m_bInited = false;
	this->m_eState = LIST_FONDED;
}

CUIComboBox::~CUIComboBox(){

}

bool CUIComboBox::SetListLength(int length){
	if (0 == m_iListHeight)
	{
		m_iListHeight = length;
		return true;
	}
	else
		return false;
}

void CUIComboBox::SetVertScroll(bool bVScroll){
	this->m_list.EnableScrollBar(bVScroll);
}

void CUIComboBox::Init(int x, int y, int width){
	m_bInited = true;
	if (0 == m_iListHeight)
		m_iListHeight = 4;

	CUIWindow::Init(x, y, width, CB_HEIGHT);
	// Frame Line
	m_frameLine.Init(0, 0, width - BTN_SIZE, CB_HEIGHT);
	m_frameLine.InitEnabledState("ui\\ui_cb_string"); // horizontal by default
	m_frameLine.InitDisabledState("ui\\ui_cb_string_d");
	m_frameLine.InitHighlightedState("ui\\ui_cb_string_h");


	// Edit Box on left side of frame line
	m_editBox.Init(0, 0, width - BTN_SIZE, CB_HEIGHT); 
	m_editBox.SetTextColor(0xff00ff00);
	m_editBox.Enable(false);
	// Button on right side of frame line
	m_btn.Init("ui\\ui_cb_button", width - BTN_SIZE, 0, BTN_SIZE, BTN_SIZE);
	// frame(texture) for list
	m_frameWnd.Init(0,  CB_HEIGHT, width - BTN_SIZE, CB_HEIGHT*m_iListHeight);
	m_frameWnd.InitEnabledState("ui\\ui_cb_frm_lst02");
	//m_frameWnd.InitDisabledState("ui\\ui_cb_frm_lst02_d");
	//m_frameWnd.InitHighlightedState("ui\\ui_cb_frm_lst02_h");

	// height of list equal to height of ONE element
	m_list.Init(0, CB_HEIGHT, width - BTN_SIZE, CB_HEIGHT*m_iListHeight);
	m_list.EnableScrollBar(true);
	m_list.SetItemHeight(CB_HEIGHT);
	m_list.SetTextColor(0xff00ff00);

	m_list.Show(false);
	m_frameWnd.Show(false);
}

void CUIComboBox::Init(int x, int y, int width, int height){
	this->Init(x, y, width);
}

void CUIComboBox::AddItem(LPCSTR str, bool bSelected){
	R_ASSERT2(m_bInited, "Can't add item to ComboBox before Initialization");
        	
	CUIListItem *item = new CUIListItem;
	item->SetText(str);
	item->SetAutoDelete(true);
	m_list.AddItem(item);
	if (bSelected)
		m_editBox.SetText(str);
}

void CUIComboBox::AddItem(LPCSTR str){
    R_ASSERT2(m_bInited, "Can't add item to ComboBox before Initialization");

	CUIListItem *item = new CUIListItem;
	item->SetAutoDelete(true);
	item->SetText(str);
	m_list.AddItem(item);	
}

void CUIComboBox::Draw(){
	CUIWindow::Draw();
}

void CUIComboBox::Update(){
	CUIWindow::Update();
	if (!m_bIsEnabled)
		SetState(S_Disabled);
}

void CUIComboBox::OnMouse(int x, int y, EUIMessages mouse_action){
    CUIWindow::OnMouse(x, y, mouse_action);

	m_bCursorOverWindow = (0 <= x) && (GetWidth() >= x) && (0 <= y) && (GetHeight() >= y);

	SetState(m_bCursorOverWindow ? S_Highlighted : S_Enabled);
	
	if(mouse_action == WINDOW_MOUSE_MOVE && m_eState == LIST_FONDED)
		GetParent()->SetCapture(this, m_bCursorOverWindow);

	switch (m_eState){
		case LIST_EXPANDED:			

			if ((!m_bCursorOverWindow) && mouse_action == WINDOW_LBUTTON_DOWN)
			{
                ShowList(false);
                GetParent()->SetCapture(this, false);
			}
			else
				GetParent()->SetCapture(this, true);
			break;
		case LIST_FONDED:
			break;
		default:
			break;
	}	
}

void CUIComboBox::SetState(UIState state){
	this->m_frameLine.SetState(state);
	this->m_frameWnd.SetState(state);	
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
		int iCurHeight;

		if (m_list.GetChildNum() <= this->m_iListHeight)
			iCurHeight = m_iListHeight*CB_HEIGHT;
		else
			iCurHeight = m_list.GetChildNum()*CB_HEIGHT;

		SetHeight(m_editBox.GetHeight() + iCurHeight);

		m_list.Show(true);
		m_list.Draw();
		m_frameWnd.Show(true);
		m_frameWnd.Draw();

		m_eState = LIST_EXPANDED;
	}
	else
	{
		m_list.Show(false);
		m_frameWnd.Show(false);
		this->SetHeight(this->m_frameLine.GetHeight());

		m_eState = LIST_FONDED;
	}
}

void CUIComboBox::OnListItemSelect(){
	int selItem = m_list.GetFocusedItem();
	CUIListItem* item = m_list.GetItem(selItem);
	if (item)
        m_editBox.SetText(item->GetText());    

	this->ShowList(false);    
}


CUIListWnd* CUIComboBox::GetListWnd(){
	return &m_list;
}