//-----------------------------------------------------------------------------/
//  ���� ������ ����� � ������� ����
//-----------------------------------------------------------------------------/

#ifndef UI_SKIN_SELECTOR_H_
#define UI_SKIN_SELECTOR_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIFrameWindow.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIInventoryUtilities.h"

using namespace InventoryUtilities;

// ���������� ������ 
const u8				SKINS_COUNT			= 4;
// ������� ������ ��������� � ��������
extern const u16		SKIN_TEX_HEIGHT;
extern const u16		SKIN_TEX_WIDTH;

//-----------------------------------------------------------------------------/
//  ����� ���� ��� ������ ������
//-----------------------------------------------------------------------------/

class CUISkinSelectorWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	// Ctor and Dtor
	CUISkinSelectorWnd(const char* strSectionName);
	CUISkinSelectorWnd();
	~CUISkinSelectorWnd();

	// �������������
	virtual void	Init(const char *strSectionName);
	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData = NULL);
	virtual void	OnMouse(int x, int y, EUIMessages mouse_action);
	virtual bool	OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void	Draw();

	// �������� ������ (0 <= Index < SKINS_COUNT) ���������� �����
	u8				GetActiveIndex()		{ return m_uActiveIndex; }
	// ����������� ����
	// Params:	idx - ������ ������ �����
	// Return:	������ ����������� �����
	u8				SwitchSkin(const u8 idx);

protected:
	// ���������� ��� ������ ������ ������ ���� � ������
	shared_str		m_strSection;

	// ������ �������� ���������� �����
	u8				m_uActiveIndex;
	
	// ���������� ������ ���� �� ������� � ���������� ����� ������ � �����������
	// �� ���������� ������.
	// Return:	������ ������ ����
	u32				CalculateSkinWindowWidth() const;

	// ������������� ������.
	void			InitializeSkins();

	// ���������� �������-������������ ������������, ��� ������� �����
	void			DrawKBAccelerators();

	// ������ �� � ������
	CUIButton		UIOkBtn, UICancelBtn;

	// C��������� - ����� ����������� ��������� ������ �� ������
	typedef struct tagSkinWindow
	{
		CUIStatic		UIHighlight;
		CUIFrameWindow	UIBackground;
	} SkinWindow;

	// ������ ������ �� �������
	SkinWindow	m_vSkinWindows[SKINS_COUNT];

};

#endif