//-----------------------------------------------------------------------------/
//  ���� ������ ����� � ������� ����
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

	int				GetActiveIndex()		{ return m_uActiveIndex; } 	// �������� ������ (0 <= Index < SKINS_COUNT) ���������� �����	
	int				SwitchSkin(const int idx);  // returns previous skin
protected:
	shared_str		m_strSection;    // ���������� ��� ������ ������ ������ ���� � ������
	int				m_uActiveIndex;  // ������ �������� ���������� �����
	CUIButton		UIOkBtn, UICancelBtn;
	void			DrawKBAccelerators();	// ���������� �������-������������ ������������, ��� ������� �����
	
	CUISkinWindow	m_vSkinWindows[SKINS_COUNT];	// ������ ������ �� �������
};

#endif