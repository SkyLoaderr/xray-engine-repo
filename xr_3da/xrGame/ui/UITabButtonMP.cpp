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
	
	if (CUIButton::BUTTON_PUSHED == m_eButtonState)
        m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2 - TEXT_INDENTION;
	else
		m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2;

	if(m_eTextAlign == CGameFont::alCenter)
		m_iTextOffsetX = GetWidth()/2;
	else if(m_eTextAlign == CGameFont::alRight)
		m_iTextOffsetX = GetWidth();
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







