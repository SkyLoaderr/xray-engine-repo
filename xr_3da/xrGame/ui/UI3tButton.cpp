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
#include "../Level.h"
#include "../HUDManager.h"

CUI3tButton::CUI3tButton(){
	this->m_psiCurrentState = NULL;
	this->m_bTextureEnable = false;
	this->m_bUseDisabledTextColor = true;
	this->m_dwDisabledTextColor = 0xFFAAAAAA;
	this->m_dwEnabledTextColor  = 0xFFFFFFFF;
}

CUI3tButton::~CUI3tButton(){

}

void CUI3tButton::Init(int x, int y, int width, int height){
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;
	m_bButtonClicked = false;
	m_bCursorOverWindow = false;

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

	// enabled state texture
	strcpy(tex_enabled,    tex_name);
	strcat(tex_enabled,   "_e");

	// pressed state texture
	strcpy(tex_disabled,   tex_name);
	strcat(tex_disabled,   "_d");

	// touched state texture
	strcpy(tex_touched, tex_name);
	strcat(tex_touched, "_t");

	RECT absRect = this->GetAbsoluteRect();

	this->m_siEnabledState.Init (tex_enabled,  "hud\\default", absRect.left, absRect.top, alNone);
	this->m_siDisabledState.Init(tex_disabled, "hud\\default", absRect.left, absRect.top, alNone);
	this->m_siTouchedState.Init (tex_touched,  "hud\\default", absRect.left, absRect.top, alNone);

	this->m_psiCurrentState = &this->m_siEnabledState;
	this->m_bTextureEnable = true;
}

void CUI3tButton::InitTexture(LPCSTR tex_enabled, LPCSTR tex_disabled, LPCSTR tex_touched){
	RECT rect = this->GetAbsoluteRect();
	this->m_siEnabledState.Init (tex_enabled,  "hud\\default", rect.left, rect.top, alNone);
	this->m_siDisabledState.Init(tex_disabled, "hud\\default", rect.left, rect.top, alNone);
	this->m_siTouchedState.Init (tex_touched,  "hud\\default", rect.left, rect.top, alNone);

	this->m_psiCurrentState = &this->m_siEnabledState;
	this->m_bTextureEnable = true;
}

void CUI3tButton::SetColor(u32 color_enabled, u32 color_disabled, u32 color_touched){
	this->m_siEnabledState.SetColor(color_enabled);
	this->m_siDisabledState.SetColor(color_disabled);
	this->m_siTouchedState.SetColor(color_touched);
}

void CUI3tButton::SetTextColor(u32 color){
    this->m_dwEnabledTextColor = color;
}

void CUI3tButton::SetDisabledTextColor(u32 color){
	this->m_dwDisabledTextColor = color;
}

void CUI3tButton::Draw(){
    RECT rect = GetAbsoluteRect();

	if(m_psiCurrentState && m_bTextureEnable)//m_bAvailableTexture && m_bTextureEnable)
	{		
		if (this->m_psiCurrentState->GetShader())
		{
			switch (this->m_eButtonState)
			{		
			case CUIButton::BUTTON_PUSHED :
				this->m_psiCurrentState = &this->m_siTouchedState;
				break;
			default :
				this->m_psiCurrentState = &this->m_siEnabledState;
				break;
			}

			if (!this->m_bIsEnabled)
				this->m_psiCurrentState = &this->m_siDisabledState;

			m_UIStaticItem.SetPos(rect.left, rect.top);
			this->m_psiCurrentState->SetPos(rect.left, rect.top);

			if(m_bStretchTexture)
				m_psiCurrentState->Render(0, 0, rect.right-rect.left, rect.bottom-rect.top);
			else
				m_psiCurrentState->Render();
		}
	}	

	if (GetFont())
	{
		UpdateTextAlign();
		GetFont()->SetAligment(GetTextAlign());

		if(IsHighlightText() && m_str && xr_strlen(m_str)>0 && m_bEnableTextHighlighting)
		{
			GetFont()->SetColor(m_HighlightColor);
			UI()->OutText(GetFont(), GetClipRect(), 
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1  +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(GetFont(), GetClipRect(), 
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(GetFont(), GetClipRect(),
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(GetFont(), GetClipRect(), 
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(GetFont(), GetClipRect(),
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 0 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(GetFont(), GetClipRect(),
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 0 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(GetFont(), GetClipRect(),
				(float)rect.left  - 0 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			UI()->OutText(GetFont(), GetClipRect(),
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
				UI()->OutText(GetFont(), GetClipRect(), 
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
}

void CUI3tButton::SendMessage(CUIWindow *pWnd, s16 msg, void *pData){
	CUIButton::SendMessage(pWnd, msg, pData);
}


