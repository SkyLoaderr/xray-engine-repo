//-----------------------------------------------------------------------------/
//  Окно выбора скина в сетевой игре
//-----------------------------------------------------------------------------/

#ifndef UI_SKIN_SELECTOR_H_
#define UI_SKIN_SELECTOR_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIButton.h"
#include "UISkinWindow.h"

const u32			SKIN_TEX_HEIGHT			= 341;
const u32			SKIN_TEX_WIDTH			= 128;

class CUISkinSelectorWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:	
	CUISkinSelectorWnd(const char* strSectionName);
	CUISkinSelectorWnd();
	~CUISkinSelectorWnd();
	
	virtual void	Init(const char *strSectionName);
	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData = NULL);
	virtual void	OnMouse(int x, int y, EUIMessages mouse_action);
	virtual bool	OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void	Draw();
	// event handlers
	virtual void	OnBtnOK();
	virtual void	OnBtnCancel();

	int				GetActiveIndex()		{ return m_uActiveIndex; } 	// Получаем индекс (0 <= Index < SKINS_COUNT) выбранного скина	
	int				SwitchSkin(const int idx);  // returns previous skin
protected:
	shared_str		m_strSection;    // Запоминаем имя секции откуда читать инфу о скинах
	int				m_uActiveIndex;  // Индекс текущего выбранного скина
	CUIButton		UIOkBtn, UICancelBtn;
	void			DrawKBAccelerators();	// Отобразить подписи-клавиатурные акселераторы, для каждого скина
	
	CUISkinWindow	m_vSkinWindows[SKINS_COUNT];	// Массив окошек со скинами
};

#endif