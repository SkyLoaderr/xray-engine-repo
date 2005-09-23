//-----------------------------------------------------------------------------/
//  Окно выбора скина в сетевой игре
//-----------------------------------------------------------------------------/

#ifndef UI_SKIN_SELECTOR_H_
#define UI_SKIN_SELECTOR_H_

// #pragma once

#include "UIDialogWnd.h"
//#include "UIButton.h"
//#include "UISkinWindow.h"

const u32			SKIN_TEX_HEIGHT			= 341;
const u32			SKIN_TEX_WIDTH			= 128;

class CUIStatic;
class CUIStatix;
class CUI3tButton;
class CUIAnimatedStatic;

class CUISkinSelectorWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:	
	CUISkinSelectorWnd(const char* strSectionName);
	~CUISkinSelectorWnd();
	
	virtual void	Init(const char* strSectionName);
	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData = NULL);
	virtual bool	OnMouse(float x, float y, EUIMessages mouse_action);
	virtual bool	OnKeyboard(int dik, EUIMessages keyboard_action);
//	virtual void	Draw();
	// event handlers


	int				GetActiveIndex()		{ return m_iActiveIndex + m_fristSkin; } 	
protected:
			void	OnBtnOK();
			void	OnBtnCancel();
			void	OnKeyLeft();
			void	OnKeyRight();

			void	InitSkins();
			void	UpdateSkins();
	CUIStatic*		m_pCaption;
	CUIStatic*		m_pBackground;
	CUIStatic*		m_pFrames;
	CUIStatix*		m_pImage[4];
	CUI3tButton*	m_pButtons[2];
	CUIAnimatedStatic* m_pAnims[2];
	
	shared_str		m_strSection;
	shared_str		m_shader;
	int				m_iActiveIndex;  
	xr_vector<xr_string> m_skins;
	int				m_fristSkin;
};

#endif