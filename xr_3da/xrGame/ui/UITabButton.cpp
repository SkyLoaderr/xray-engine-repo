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
#include "../Level.h"
#include "../HUDManager.h"

CUITabButton::CUITabButton(){
	this->m_psiCurrentState = NULL;
	this->m_bUseDisabledTextColor = false;
	this->m_pAssociatedWindow = NULL;
	this->m_bTextureEnable = false;
	this->m_dwEnabledTextColor = 0xFFFFFFFF;
}

CUITabButton::~CUITabButton(){

}

void CUITabButton::Init(int x, int y, int width, int height){
	CUIWindow::Init(x, y, width, height);
}


void CUITabButton::InitTexture(LPCSTR tex_norm, LPCSTR tex_press, LPCSTR tex_disable){
	RECT rect = this->GetAbsoluteRect();
	this->m_siEnabledNormalState.Init (tex_norm,    "hud\\default", rect.left, rect.top, alNone);
	this->m_siEnabledPressedState.Init(tex_press,   "hud\\default", rect.left, rect.top, alNone);
	this->m_siDisabledState.Init      (tex_disable, "hud\\default", rect.left, rect.top, alNone);

	this->m_psiCurrentState = &this->m_siEnabledNormalState;
	this->m_bTextureEnable = true;
}

void CUITabButton::InitTexture(LPCSTR tex_name){
	
	char tex_norm[256];
	char tex_press[256];
	char tex_disable[256];

	// normal state texture
	strcpy(tex_norm,    tex_name);

	// pressed state texture
	strcpy(tex_press,   tex_name);
	strcat(tex_press,   "p");

	// disabled state texture
	strcpy(tex_disable, tex_name);
	strcat(tex_disable, "d");


	this->m_siEnabledNormalState.Init (tex_norm,    "hud\\default", m_WndRect.left, m_WndRect.top, alNone);
	this->m_siEnabledPressedState.Init(tex_press,   "hud\\default", m_WndRect.left, m_WndRect.top, alNone);
	this->m_siDisabledState.Init      (tex_disable, "hud\\default", m_WndRect.left, m_WndRect.top, alNone);

	this->m_psiCurrentState = &this->m_siEnabledNormalState;
	this->m_bTextureEnable = true;
}

void CUITabButton::SetColor(u32 color_norm, u32 color_press, u32 color_disable){
	this->m_siEnabledNormalState.SetColor(color_norm);
	this->m_siEnabledPressedState.SetColor(color_press);
	this->m_siDisabledState.SetColor(color_disable);	
}

void CUITabButton::AssociateWindow(CUIFrameWindow* pWindow){
	this->m_pAssociatedWindow = pWindow;
}

CUIFrameWindow* CUITabButton::GetAssociatedWindow(){
	return this->m_pAssociatedWindow;
}

//void CUITabButton::Enable(bool bEnable){
//	inherited::Enable(bEnable);
//}

void CUITabButton::ShowAssociatedWindow(bool bShow){
#ifdef DEBUG
	if (!this->m_pAssociatedWindow)
		printf("Warning!: the window is not associated to TabButton");
#endif

	if (this->m_pAssociatedWindow)
        this->m_pAssociatedWindow->Show(bShow);
}

void CUITabButton::SetTextColor(u32 color){
	this->m_dwEnabledTextColor = color;
}

void CUITabButton::Draw(){
	RECT rect = GetAbsoluteRect();	

	if(m_psiCurrentState && m_bTextureEnable)//m_bAvailableTexture && m_bTextureEnable)
	{		
		if (this->m_psiCurrentState->GetShader())
		{
			m_UIStaticItem.SetPos(rect.left, rect.top);

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
			HUD().OutText(GetFont(), GetClipRect(), 
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1  +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			HUD().OutText(GetFont(), GetClipRect(), 
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			HUD().OutText(GetFont(), GetClipRect(),
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			HUD().OutText(GetFont(), GetClipRect(), 
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			HUD().OutText(GetFont(), GetClipRect(),
				(float)rect.left  + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 0 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			HUD().OutText(GetFont(), GetClipRect(),
				(float)rect.left  - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  - 0 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			HUD().OutText(GetFont(), GetClipRect(),
				(float)rect.left  - 0 +m_iTextOffsetX + m_iShadowOffsetX, 
				(float)rect.top  + 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
			HUD().OutText(GetFont(), GetClipRect(),
				(float)rect.left  + 0 +m_iTextOffsetX + m_iShadowOffsetX,  
				(float)rect.top  - 1 +m_iTextOffsetY + m_iShadowOffsetY,
				m_str);
		}

		// set text color
		if (this->m_bUseDisabledTextColor && !this->m_bIsEnabled)
		{
			R_ASSERT2(m_dwDisabledTextColor,"UITabButton::Draw() - m_dwDisabledTextColor == NULL");
            GetFont()->SetColor(m_dwFontColor);
		}
		else
			GetFont()->SetColor(this->m_dwEnabledTextColor);

		if (!m_bNewRenderMethod)
		{
			if(m_str && xr_strlen(m_str)>0)
				HUD().OutText(GetFont(), GetClipRect(), 
				(float)rect.left   +  m_iTextOffsetX, 
				(float)rect.top   + m_iTextOffsetY,
				m_str);
			GetFont()->OnRender();
		}
		else
			inherited::DrawString(rect);
		
	}
}

void CUITabButton::Update(){
}

void CUITabButton::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (!this->m_bIsEnabled)
		return;

	switch (msg)
	{
	case TAB_SELECT:
		this->ShowAssociatedWindow();
		this->m_psiCurrentState = &this->m_siEnabledPressedState;
		this->Draw();
		break;
	case TAB_DESELECT:
		this->ShowAssociatedWindow();
		this->m_psiCurrentState = &this->m_siEnabledNormalState;
		this->Draw();
		break;
	default:
		;
	}
}