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

CUIComboBox::CUIComboBox(){
	// edit box
	m_frameLine.SetAutoDelete(true);
	AttachChild(&this->m_frameLine);

	m_editBox.SetAutoDelete(true);	
	AttachChild(&this->m_editBox);

	// button
	m_btn.SetAutoDelete(true);
	AttachChild(&this->m_btn);

	// list
	m_frameWnd.SetAutoDelete(true);
	AttachChild(&this->m_frameWnd);

	m_list.SetAutoDelete(true);	
	AttachChild(&this->m_list);

	this->m_iListHeight = 3;
}

CUIComboBox::~CUIComboBox(){

}

void CUIComboBox::Init(int x, int y, int width){
	CUIWindow::Init(x, y, width, 32);
	// Frame Line
	m_frameLine.Init("ui\\ui_cb_test", 0, 0, width, 32, true /*horizontal*/);	
	// Edit Box on left side of frame line
	m_editBox.Init(0, 0, width - 32, 32);
	m_editBox.SetTextColor(0xff00ff00);
	// Button on right side of frame line
	m_btn.Init("ui\\ui_cb_btn_test", width - 32, 0, 32, 32);
	// frame(texture) for list
	m_frameWnd.Init("ui\\ui_frame_very_small", 0,  32, width, 32);
	// height of list equal to height of ONE element
	m_list.Init(0, 32, width, 32);
	m_list.SetItemHeight(32);
	m_list.SetTextColor(0xff00ff00);

	m_list.Show(false);
	m_frameWnd.Show(false);

	AddItem("item1");
	AddItem("item2", true);
}

void CUIComboBox::AddItem(LPCSTR str, bool bSelected){
    CUIListItemEx *item = new CUIListItemEx;
	item->SetText(str);
	m_list.AddItem(item);
	if (bSelected)
		m_editBox.SetText(str);
}

void CUIComboBox::Draw(){
	CUIWindow::Draw();
}

void CUIComboBox::Update(){
	CUIWindow::Update();
}

void CUIComboBox::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	CUIWindow::SendMessage(pWnd, msg, pData);

	switch (msg){
		case BUTTON_CLICKED:
			if (pWnd == &this->m_btn)
				OnBtnClicked();			
			break;
		case LIST_ITEM_SELECT:
			if (pWnd == &this->m_list)
				this->OnListItemSelect();	
			break;
		default:
			break;
	}
}

void CUIComboBox::OnBtnClicked(){
	if (m_list.IsShown())
	{
		m_list.Show(false);
		m_frameWnd.Show(false);
		this->SetHeight(this->m_frameLine.GetHeight());
	}
	else
	{
		m_list.Show(true);
		m_frameWnd.Show(true);

		int iCurHeight;
		if (m_list.GetChildNum() >= this->m_iListHeight)
			iCurHeight = m_iListHeight;
		else
			iCurHeight = m_list.GetChildNum();

		//this->m_frameWnd.SetHeight(iCurHeight);
		this->m_list.SetHeight(iCurHeight);
	}
}

void CUIComboBox::OnListItemSelect(){

}


CUIListWndEx* CUIComboBox::GetListWnd(){
	return &m_list;
}