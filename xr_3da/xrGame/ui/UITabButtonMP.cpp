// File:		UITabButtonMP.cpp
// Description:	Tab Button designed special for TabControl in CUIBuyWeaponWnd
// Created:		17.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UITabButtonMP.h"
#include "../MainUI.h"
#include "../HUDManager.h"

const int TEXT_INDENTION = 43;

CUITabButtonMP::CUITabButtonMP(){
	AttachChild(&m_staticNumber);
	m_staticNumber.SetFont(UI()->Font()->pFontGraffiti32Russian);
	m_orientation = O_VERTICAL;
}

CUITabButtonMP::~CUITabButtonMP(){

}

void CUITabButtonMP::Init(int x, int y, int width, int height){
	CUITabButton::Init(x, y, width, height);
	m_staticNumber.Init(15, -26, 29, 29);
	m_staticNumber.SetTextAlign(CGameFont::alCenter);
	m_staticNumber.SetTextX(13);
	m_staticNumber.SetTextY(-6);
}

void CUITabButtonMP::SetNumber(LPCSTR num){
    m_staticNumber.SetText(num);
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
            m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2 - TEXT_INDENTION;
		else
			m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2;

		if(m_eTextAlign == CGameFont::alCenter)
			m_iTextOffsetX = GetWidth()/2;
		else if(m_eTextAlign == CGameFont::alRight)
			m_iTextOffsetX = GetWidth();

		break;
	case O_HORIZONTAL:
   			m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2;

			if (m_bCursorOverWindow)
				m_iTextOffsetX = GetWidth()/2 + 100;
			else
				m_iTextOffsetX = GetWidth()/2;

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
	
	if (!m_bIsEnabled)
		m_staticNumber.SetTextColor(color_argb(255, 87, 87, 72));

	else if (CUIButton::BUTTON_PUSHED == m_eButtonState)
		;
	else if (m_bCursorOverWindow)
        m_staticNumber.SetTextColor(color_argb(255, 255, 226, 79));
	else
		m_staticNumber.SetTextColor(color_argb(255, 255, 255, 0));

	m_staticNumber.Show(!(CUIButton::BUTTON_PUSHED == m_eButtonState));


	m_bIsEnabled = tempEnabled;
}







