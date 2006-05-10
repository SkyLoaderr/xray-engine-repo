// File:		UITabButtonMP.cpp
// Description:	Tab Button designed special for TabControl in CUIBuyWeaponWnd
// Created:		17.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UITabButtonMP.h"
#include "../MainMenu.h"
#include "../HUDManager.h"

const int TEXT_INDENTION = 43;

CUITabButtonMP::CUITabButtonMP(){
	m_orientation = O_VERTICAL;
}

CUITabButtonMP::~CUITabButtonMP(){

}


void CUITabButtonMP::SendMessage(CUIWindow* pWnd, s16 msg, void* pData /* = 0 */){
	if (this == pWnd)
		m_bIsEnabled = true;

	CUITabButton::SendMessage(pWnd, msg, pData);
}

void CUITabButtonMP::UpdateTextAlign(){
	switch (m_orientation)
	{
	case O_VERTICAL:
        if (CUIButton::BUTTON_PUSHED == m_eButtonState)
            m_iTextOffsetY =  - TEXT_INDENTION;
		else
			m_iTextOffsetY = 0;

//		if(m_eTextAlign == CGameFont::alCenter)
//			m_iTextOffsetX = GetWidth()/2;
//		else if(m_eTextAlign == CGameFont::alRight)
//			m_iTextOffsetX = GetWidth();

		break;
	case O_HORIZONTAL:
//   			m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2;

			if (m_bCursorOverWindow)
				m_iTextOffsetX = 100;
			else
				m_iTextOffsetX = 0;

		break;
	default:
		NODEFAULT;
	}


}

void CUITabButtonMP::SetOrientation(int orientation){
	m_orientation = orientation;

	R_ASSERT2(orientation == O_HORIZONTAL || orientation == O_VERTICAL, "CUITabButtonMP - invalid orientation value");
}

void CUITabButtonMP::Update(){

	// cunning code : use Highlighting even if is disabled

	bool tempEnabled = m_bIsEnabled;
	m_bIsEnabled = m_bCursorOverWindow ? true : m_bIsEnabled;
	CUITabButton::Update();
	m_bIsEnabled = tempEnabled;
	UpdateTextAlign();
}

void CUITabButtonMP::Draw(){
	UI()->PushScissor(UI()->ScreenRect(),true);

	CUITabButton::Draw();

	UI()->PopScissor();
}







