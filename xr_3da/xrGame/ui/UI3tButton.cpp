// File:        UI3tButton.cpp
// Description: Button with 3 texutres (for <enabled>, <disabled> and <touched> states)
// Created:     07.12.2004
// Author:      Serhiy 0. Vynnychenk0
// Mail:        narrator@gsc-game.kiev.ua
//
// copyright 2004 GSC Game World
//

#include "StdAfx.h"
#include "UI3tButton.h"
#include "../MainUI.h"

CUI3tButton::CUI3tButton(){
	m_bTextureEnable = false;
	m_bUseTextColorD = true;
	m_bUseTextColorH = false;
	m_bUseTextColorT = false;	

	m_dwTextColorE = 0xFFFFFFFF;
	m_dwTextColorD = 0xFFAAAAAA;
	m_dwTextColorH = 0xFFFFFFFF;
	m_dwTextColorT = 0xFFFFFFFF;

	AttachChild(&m_background);
}

CUI3tButton::~CUI3tButton(){

}

void CUI3tButton::Init(int x, int y, int width, int height){
	m_background.Init(0, 0, width, height);
    CUIButton::Init(x, y, width, height);
}

void CUI3tButton::InitTexture(LPCSTR tex_name){
	char tex_enabled[256];
	char tex_disabled[256];
	char tex_touched[256];
	char tex_highlighted[256];

	// enabled state texture
	strcpy(tex_enabled,    tex_name);
	strcat(tex_enabled,   "_e");

	// pressed state texture
	strcpy(tex_disabled,   tex_name);
	strcat(tex_disabled,   "_d");

	// touched state texture
	strcpy(tex_touched, tex_name);
	strcat(tex_touched, "_t");

	// touched state texture
	strcpy(tex_highlighted, tex_name);
	strcat(tex_highlighted, "_h");

	this->InitTexture(tex_enabled, tex_disabled, tex_touched, tex_highlighted);		
}

void CUI3tButton::InitTexture(LPCSTR tex_enabled, LPCSTR tex_disabled, LPCSTR tex_touched, LPCSTR tex_highlighted){
	m_background.InitEnabledState(tex_enabled);
	m_background.InitDisabledState(tex_disabled);
	m_background.InitTouchedState(tex_touched);	
	m_background.InitHighlightedState(tex_highlighted);
	this->m_bTextureEnable = true;
}

void CUI3tButton::InitTextureEnabled(LPCSTR texture){
    m_background.InitEnabledState(texture);
	m_bTextureEnable = true;
}

void CUI3tButton::InitTextureDisabled(LPCSTR texture){
    m_background.InitDisabledState(texture);
}

void CUI3tButton::InitTextureHighlighted(LPCSTR texture){
    m_background.InitHighlightedState(texture);
}

void CUI3tButton::InitTextureTouched(LPCSTR texture){
    m_background.InitTouchedState(texture);
}

void CUI3tButton::SetTextColor(u32 color){
    this->m_dwTextColorE = color;
}

void CUI3tButton::SetTextColorD(u32 color){
	m_dwTextColorD = color;
	m_bUseTextColorD = true;
}

void CUI3tButton::SetTextColorH(u32 color){
	m_dwTextColorH = color;
	m_bUseTextColorH = true;
}

void CUI3tButton::SetTextColorT(u32 color){
	m_dwTextColorT = color;
	m_bUseTextColorT = true;
}

void CUI3tButton::SetTextureOffset(int x, int y){
	this->m_background.SetTextureOffset(x, y);
}

void CUI3tButton::Draw(){
	CUIButton::Draw();
}

void CUI3tButton::DrawTexture(){
	m_background.Draw();
}

void CUI3tButton::Update(){
	CUIButton::Update();

	if(m_bTextureEnable)
	{
		if (!m_bIsEnabled)
            m_background.SetState(S_Disabled);
		else if (CUIButton::BUTTON_PUSHED == m_eButtonState)
			m_background.SetState(S_Touched);
		else if (m_bCursorOverWindow)
			m_background.SetState(S_Highlighted);		
		else
			m_background.SetState(S_Enabled);		
	}

	u32 textColor;

	if (!m_bIsEnabled)
		textColor = m_bUseTextColorD ? m_dwTextColorD : m_dwTextColorE;
	else if (CUIButton::BUTTON_PUSHED == m_eButtonState)
		textColor = m_bUseTextColorT ? m_dwTextColorT : m_dwTextColorE;
	else if (m_bCursorOverWindow)
		textColor = m_bUseTextColorH ? m_dwTextColorH : m_dwTextColorE;
	else
		textColor = m_dwTextColorE;

	CUIStatic::SetTextColor(textColor);
}


void CUI3tButton::SendMessage(CUIWindow *pWnd, s16 msg, void *pData){
	CUIButton::SendMessage(pWnd, msg, pData);
}

