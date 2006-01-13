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
#include "../../sound.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"

CUI3tButton::CUI3tButton(){
	m_iPushOffsetX = 0;
	m_iPushOffsetY = 0;
	m_bTextureEnable = false;
	m_bUseTextColor[D] = true;
	m_bUseTextColor[H] = false;
	m_bUseTextColor[T] = false;	

	m_dwTextColor[E] = 0xFFFFFFFF;
	m_dwTextColor[D] = 0xFFAAAAAA;
	m_dwTextColor[H] = 0xFFFFFFFF;
	m_dwTextColor[T] = 0xFFFFFFFF;

	AttachChild(&m_background);
	AttachChild(&m_background_frame);	
	AttachChild(&m_hint);

	m_bEnableTextHighlighting = false;
	m_bCheckMode = false;
	m_b_frameMode = false;
}

CUI3tButton::~CUI3tButton(){

}

void CUI3tButton::OnClick(){
    CUIButton::OnClick();
    PlaySoundT();
}

bool CUI3tButton::OnMouse(float x, float y, EUIMessages mouse_action){
	if (m_bCheckMode)
		return CUIWindow::OnMouse(x,y,mouse_action);
	else
		return CUIButton::OnMouse(x,y,mouse_action);
}

void CUI3tButton::OnMouseDown(bool left_button /* = true */){
	if (m_bCheckMode)
	{
		if (left_button)
		{
			if (m_eButtonState == BUTTON_NORMAL)
				m_eButtonState = BUTTON_PUSHED;
			else
				m_eButtonState = BUTTON_NORMAL;
		}
		GetMessageTarget()->SendMessage(this, BUTTON_CLICKED, NULL);
	}
	else
		CUIButton::OnMouseDown(left_button);
}

void CUI3tButton::OnFocusReceive(){
	CUIButton::OnFocusReceive();
	PlaySoundH();
}

void CUI3tButton::InitSoundH(LPCSTR sound_file){
	::Sound->create(m_sound_h, true, sound_file);
}

void CUI3tButton::InitSoundT(LPCSTR sound_file){
	::Sound->create(m_sound_t, true, sound_file); 
}

void CUI3tButton::PlaySoundT(){
	if (m_sound_t._handle())
        m_sound_t.play(NULL, sm_2D);
}

void CUI3tButton::PlaySoundH(){
	if (m_sound_h._handle())
		m_sound_h.play(NULL, sm_2D);
}

void CUI3tButton::Init(float x, float y, float width, float height){
	m_background.Init(0, 0, width, height);
	m_background_frame.Init(0, 0, width, height);
    CUIButton::Init(x, y, width, height);
}

void CUI3tButton::SetWidth(float width){
	CUIButton::SetWidth(width);
	m_background.SetWidth(width);
	m_background_frame.SetWidth(width);
}

void CUI3tButton::SetHeight(float height){
	CUIButton::SetHeight(height);
	m_background.SetHeight(height);
	m_background_frame.SetHeight(height);
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
	if (!m_b_frameMode)
	{
		m_background.InitEnabledState(tex_enabled);
		m_background.InitDisabledState(tex_disabled);
		m_background.InitTouchedState(tex_touched);
		m_background.InitHighlightedState(tex_highlighted);
	}else{
		m_background_frame.InitEnabledState(tex_enabled);
		m_background_frame.InitDisabledState(tex_disabled);
		m_background_frame.InitTouchedState(tex_touched);
		m_background_frame.InitHighlightedState(tex_highlighted);
	}
	this->m_bTextureEnable = true;
}

void CUI3tButton::InitTextureEnabled(LPCSTR texture){
	if (!m_b_frameMode)
        m_background.InitEnabledState(texture);
	else
		m_background_frame.InitEnabledState(texture);
	m_bTextureEnable = true;
}

void CUI3tButton::InitTextureDisabled(LPCSTR texture){
	if (!m_b_frameMode)
        m_background.InitDisabledState(texture);
	else
		m_background_frame.InitDisabledState(texture);
}

void CUI3tButton::InitTextureHighlighted(LPCSTR texture){
	if (!m_b_frameMode)
        m_background.InitHighlightedState(texture);
	else
		m_background_frame.InitHighlightedState(texture);
}

void CUI3tButton::InitTextureTouched(LPCSTR texture){
	if (!m_b_frameMode)
        m_background.InitTouchedState(texture);
	else
		m_background_frame.InitTouchedState(texture);
}

void CUI3tButton::SetTextColor(u32 color){
    m_dwTextColor[E] = color;
}

void CUI3tButton::SetTextColorD(u32 color){
	SetTextColor(color, CUIStatic::D);
}

void CUI3tButton::SetTextColorH(u32 color){
	SetTextColor(color, CUIStatic::H);
}

void CUI3tButton::SetTextColorT(u32 color){
	SetTextColor(color, CUIStatic::T);
}

void CUI3tButton::SetTextureOffset(float x, float y){
	this->m_background.SetTextureOffset(x, y);
}

void CUI3tButton::DrawTexture(){
	if(m_bTextureEnable)
	{
		if (!m_b_frameMode)
            m_background.Draw();
		else
			m_background_frame.Draw();
	}
}

void CUI3tButton::Update(){
	CUIButton::Update();

	if(m_bTextureEnable)
	{
		if (!m_bIsEnabled){
            m_background.SetState(S_Disabled);
			m_background_frame.SetState(S_Disabled);
		}
		else if (CUIButton::BUTTON_PUSHED == m_eButtonState){
			m_background.SetState(S_Touched);
			m_background_frame.SetState(S_Touched);
		}
		else if (m_bCursorOverWindow){
			m_background.SetState(S_Highlighted);
			m_background_frame.SetState(S_Highlighted);
		}
		else{
			m_background.SetState(S_Enabled);
			m_background_frame.SetState(S_Enabled);
		}
	}

	u32 textColor;
	u32 hintColor;

	if (!m_bIsEnabled)
	{
		textColor = m_bUseTextColor[D] ? m_dwTextColor[D] : m_dwTextColor[E];
		hintColor = m_hint.m_bUseTextColor[D] ? m_hint.m_dwTextColor[D] : m_hint.m_dwTextColor[E];
	}
	else if (CUIButton::BUTTON_PUSHED == m_eButtonState)
	{
		textColor = m_bUseTextColor[T] ? m_dwTextColor[T] : m_dwTextColor[E];
		hintColor = m_hint.m_bUseTextColor[T] ? m_hint.m_dwTextColor[T] : m_hint.m_dwTextColor[E];
	}
	else if (m_bCursorOverWindow)
	{
		textColor = m_bUseTextColor[H] ? m_dwTextColor[H] : m_dwTextColor[E];
		hintColor = m_hint.m_bUseTextColor[H] ? m_hint.m_dwTextColor[H] : m_hint.m_dwTextColor[E];
	}
	else
	{
		textColor = m_dwTextColor[E];
		hintColor = m_hint.m_dwTextColor[E];
	}

	CUIStatic::SetTextColor(textColor);
	m_hint.SetTextColor(hintColor);
}

void CUI3tButton::SendMessage(CUIWindow *pWnd, s16 msg, void *pData){
	CUIButton::SendMessage(pWnd, msg, pData);
}

void CUI3tButton::SetFrameMode(bool mode){
	m_b_frameMode = mode;
}