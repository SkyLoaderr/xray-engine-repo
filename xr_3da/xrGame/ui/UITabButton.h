// File:        UITabButton.cpp
// Description: 
// Created:     19.11.2004
// Last Change: 19.11.2004
// Author:      Serhiy Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright: 2004 GSC Game World

#pragma once
#include "UIButton.h"
#include "UIFrameWindow.h"

class CUITabButton : public CUIButton {
	typedef CUIButton inherited;
public:
	CUITabButton();
	virtual ~CUITabButton();
	// appearence
	virtual void InitTexture(char* tex_name);
	virtual void InitTexture(char* tex_norm, char* tex_press, char* tex_disable);	
	virtual void SetColor(u32 color_norm, u32 color_press, u32 color_disable);	
	virtual void SetTextColor(u32 color);

	// behavior
	virtual void Draw();
	virtual void AssociateWindow(CUIFrameWindow* pWindow);
	virtual CUIFrameWindow* GetAssociatedWindow();
	//virtual void Enable(bool bEnable);
	virtual void ShowAssociatedWindow(bool bShow = true);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);

protected:
	// textures
	CUIStaticItem* m_psiCurrentState;
	CUIStaticItem  m_siEnabledNormalState;
	CUIStaticItem  m_siEnabledPressedState;
	CUIStaticItem  m_siDisabledState;

    // text color
	bool m_bUseDisabledTextColor;
	u32  m_dwEnabledTextColor;
	u32  m_dwDisabledTextColor;

	// asociated window
	CUIFrameWindow* m_pAssociatedWindow;
};