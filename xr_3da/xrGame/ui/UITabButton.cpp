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
		GetMessageTarget()->SendMessage(this, TAB_CHANGED);		
		return;
	}
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
			OnClick();
		}
		else		
		{
			m_eButtonState = BUTTON_NORMAL;
			ShowAssociatedWindow(false);
		}
		break;
	default:
		;
	}
}