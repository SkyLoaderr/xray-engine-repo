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
#include "UIInteractiveBackground.h"

class CUI3tButton : public CUIButton {
public:
	CUI3tButton();
	virtual ~CUI3tButton();
	// appearence
	virtual void Init(int x, int y, int width, int height);
    virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);
	virtual void InitTexture(LPCSTR tex_name);
	virtual void InitTexture(LPCSTR tex_enabled, LPCSTR tex_disabled, LPCSTR tex_touched);	
	virtual void SetColor(u32 color_enabled, u32 color_disabled, u32 color_touched);	
	virtual void SetTextColor(u32 color);
	virtual void SetDisabledTextColor(u32 color);

	// behavior
	virtual void Draw();
	virtual void Update();
	
	//virtual void Enable(bool bEnable);	
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);

private:	
	CUI_IB_Static  m_background;

    // text color
	bool m_bUseDisabledTextColor;
	u32  m_dwEnabledTextColor;
	u32  m_dwDisabledTextColor;
};