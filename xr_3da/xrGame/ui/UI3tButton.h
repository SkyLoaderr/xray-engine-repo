// File:        UI3tButton.cpp
// Description: Button with 3 texutres (for <enabled>, <disabled> and <touched> states)
// Created:     07.12.2004
// Author:      Serhiy 0. Vynnychenk0
// Mail:        narrator@gsc-game.kiev.ua
//
// copyright 2004 GSC Game World
//

#pragma once
#include "UIButton.h"
#include "UI_IB_Static.h"

class CUI3tButton : public CUIButton {
	friend class CUIXmlInit;
	using CUIButton::SetTextColor;
public:
	CUI3tButton();
	virtual ~CUI3tButton();
	// appearance
	using CUIButton::Init;

	virtual void Init(float x, float y, float width, float height);
	virtual void InitTexture(LPCSTR tex_name);
	virtual void InitTexture(LPCSTR tex_enabled, LPCSTR tex_disabled, LPCSTR tex_touched, LPCSTR tex_highlighted);	
			void InitTextureEnabled(LPCSTR texture);
			void InitTextureDisabled(LPCSTR texture);
			void InitTextureHighlighted(LPCSTR texture);
			void InitTextureTouched(LPCSTR texture);			
			void SetTextColor(u32 color);
			void SetTextColorH(u32 color);
			void SetTextColorD(u32 color);
			void SetTextColorT(u32 color);
	virtual void SetTextureOffset(float x, float y);	
			void InitSoundH(LPCSTR sound_file);
			void InitSoundT(LPCSTR sound_file);

	virtual void OnClick();
	virtual void OnFocusReceive();
	
	// behavior
	virtual void DrawTexture();
	virtual void Update();
	
	//virtual void Enable(bool bEnable);	
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);


	CUIStatic		m_hint;
	CUI_IB_Static	m_background;
private:	
			void PlaySoundH();
			void PlaySoundT();

	ref_sound		m_sound_h;
	ref_sound		m_sound_t;	

    // text color
//	bool m_bUseTextColorD;
//	bool m_bUseTextColorH;
//	bool m_bUseTextColorT;
//	bool m_bUseTextColor[4];
//	u32  m_dwTextColor[4];
//	u32  m_dwTextColorE;
//	u32  m_dwTextColorD;
//	u32  m_dwTextColorH;
//	u32  m_dwTextColorT;
};