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
	this->m_bTextureEnable = false;
	this->m_bUseDisabledTextColor = true;
	this->m_dwDisabledTextColor = 0xFFAAAAAA;
	this->m_dwEnabledTextColor  = 0xFFFFFFFF;

	AttachChild(&m_background);
}

CUI3tButton::~CUI3tButton(){

}

void CUI3tButton::Init(int x, int y, int width, int height){
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;
	m_bButtonClicked = false;
	m_bCursorOverWindow = false;
	m_background.Init(0, 0, width, height);

    CUIWindow::Init(x, y, width, height);
}

void CUI3tButton::Init(LPCSTR tex_name, int x, int y, int width, int height)
{
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;
	m_bButtonClicked = false;
	m_bCursorOverWindow = false;

	this->Init(x, y, width, height);
	this->InitTexture(tex_name);
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

void CUI3tButton::SetColor(u32 color_enabled, u32 color_disabled, u32 color_touched){
	;
	;
	;
}

void CUI3tButton::SetTextColor(u32 color){
    this->m_dwEnabledTextColor = color;
}

void CUI3tButton::SetDisabledTextColor(u32 color){
	this->m_dwDisabledTextColor = color;
}

void CUI3tButton::Draw(){
	Irect rect = GetAbsoluteRect();

	m_background.Draw();

	if (GetFont())
	{
		UpdateTextAlign();
		GetFont()->SetAligment(GetTextAlign());

		if(IsHighlightText() && m_str && xr_strlen(m_str)>0 && m_bEnableTextHighlighting)
		{
			CGameFont* F = GetFont();
			Irect clip_rect = GetSelfClipRect();
			F->SetColor(m_HighlightColor);

			UI()->OutText(F, clip_rect, 
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1  +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(F, clip_rect, 
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(F, clip_rect,
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(F, clip_rect, 
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(F, clip_rect,
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 0 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(F, clip_rect,
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 0 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(F, clip_rect,
				(float)rect.left  - 0 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(F, clip_rect,
				(float)rect.left  + 0 +m_iTextOffsetX + m_iShadowOffsetX,  
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
		}

		// set text color
		if (this->m_bUseDisabledTextColor && !this->m_bIsEnabled)
		{
			R_ASSERT2(m_dwDisabledTextColor,"UITabButton::Draw() - m_dwDisabledTextColor == NULL");
            GetFont()->SetColor(m_dwDisabledTextColor);
		}
		else
			GetFont()->SetColor(this->m_dwEnabledTextColor);

		if (!m_bNewRenderMethod)
		{
			if(m_str && xr_strlen(m_str)>0)
				UI()->OutText(GetFont(), GetSelfClipRect(), 
				(float)rect.left   +  m_iTextOffsetX, 
				(float)rect.top   + m_iTextOffsetY,
				m_str);
			GetFont()->OnRender();
		}
		else
			CUIStatic::DrawString(rect);		
	}
}

void CUI3tButton::Update(){
	CUIButton::Update();

	if(m_bTextureEnable)
	{
		if (!m_bIsEnabled)
			m_background.SetState(S_Disabled);
		else if (CUIButton::BUTTON_PUSHED == m_eButtonState)
			m_background.SetState(S_Touched);
		else if (this->IsHighlightText())
			m_background.SetState(S_Highlighted);		
		else
			m_background.SetState(S_Enabled);		
	}	
}

void CUI3tButton::SendMessage(CUIWindow *pWnd, s16 msg, void *pData){
	CUIButton::SendMessage(pWnd, msg, pData);
}

