// File:        UITabButton.cpp
// Description: 
// Created:     19.11.2004
// Last Change: 27.11.2004
// Author:      Serhiy Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright: 2004 GSC Game World

#include "StdAfx.h"
#include "UITabButton.h"
#include "../HUDManager.h"

CUITabButton::CUITabButton(){
	this->m_pAssociatedWindow = NULL;
}

CUITabButton::~CUITabButton(){

}

void CUITabButton::AssociateWindow(CUIFrameWindow* pWindow){
	this->m_pAssociatedWindow = pWindow;
}

CUIFrameWindow* CUITabButton::GetAssociatedWindow(){
	return this->m_pAssociatedWindow;
}

void CUITabButton::ShowAssociatedWindow(bool bShow){
	if (this->m_pAssociatedWindow)
        this->m_pAssociatedWindow->Show(bShow);
}

void CUITabButton::OnMouse(int x, int y, EUIMessages mouse_action){
	CUIWindow::OnMouse(x, y, mouse_action);

	if (WINDOW_LBUTTON_DOWN == mouse_action)
	{
		Msg("-- TAB_CHANGED for this=%o", this);
		GetMessageTarget()->SendMessage(this, TAB_CHANGED);		
		return;
	}

	
//	m_bCursorOverWindow = (0 <= x) && (GetWidth() >= x) && (0 <= y) && (GetHeight() >= y);
    
	// we ends capturing if cursor is out of window
//    GetParent()->SetCapture(this, m_bCursorOverWindow);	
}

void CUITabButton::Update(){
	CUI3tButton::Update();
}

void CUITabButton::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (!IsEnabled())
		return;

	switch (msg)
	{
	case TAB_CHANGED:
		if (this == pWnd)
		{
            m_eButtonState = BUTTON_PUSHED;			
			ShowAssociatedWindow(true);
			Msg("-- BUTTON_PUSHED for this=%o", this);
			OnClick();
		}
		else		
		{
			m_eButtonState = BUTTON_NORMAL;
			ShowAssociatedWindow(false);
			Msg ("-- BUTTON_NORMAL for this=%o", this);			
		}
		break;
	default:
		;
	}
}