//////////////////////////////////////////////////////////////////////
// UITabControl.h: ����� ���� � ����������.
//////////////////////////////////////////////////////////////////////

#ifndef UI_TABCONTROL_H_
#define UI_TABCONTROL_H_

#pragma once

#include "uiwindow.h"
#include "UITabButton.h"
#include "../script_export_space.h"
#include "UIOptionsItem.h"

DEF_VECTOR (TABS_VECTOR, CUITabButton*)

class CUITabControl: public CUIWindow, public CUIOptionsItem {
	typedef CUIWindow inherited;
public:
	CUITabControl();
	virtual ~CUITabControl();

	// options itme
	virtual void SetCurrentValue();
	virtual void SaveValue();

	virtual void Init(float x, float y, float width, float height);
	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void OnTabChange(int iCur, int iPrev);
	virtual void OnStaticFocusReceive(CUIWindow* pWnd);
	virtual void OnStaticFocusLost(CUIWindow* pWnd);
	virtual void Update();

	// ���������� ������-�������� � ������ �������� ��������
	bool AddItem(const char *pItemName, const char *pTexName, float x, float y, float width, float height);
	bool AddItem(CUITabButton *pButton);

	void RemoveItem(const u32 Index);
	void RemoveAll();

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

			int  GetActiveIndex		()						{ return m_iPushedIndex; }
			int  GetPrevActiveIndex	()						{ return m_iPrevPushedIndex; }
			void SetNewActiveTab	(const int iNewTab);	
	const	int  GetTabsCount		() const				{ return m_TabsArr.size(); }
	
	// ����� ������������� ������������� (���/����)
	bool GetAcceleratorsMode() const			{ return m_bAcceleratorsEnable; }
	void SetAcceleratorsMode(bool bEnable)		{ m_bAcceleratorsEnable = bEnable; }

	// �����
	void SetActiveTextColor(u32 cl)				{ m_cActiveTextColor = cl; m_bChangeColors = true; }
	u32 GetActiveTextColor() const 				{ return m_cActiveTextColor; }
	void SetGlobalTextColor(u32 cl)				{ m_cGlobalTextColor = cl; m_bChangeColors = true; }
	u32 GetGloablTextColor() const 				{ return m_cGlobalTextColor; }

	void SetActiveButtonColor(u32 cl)			{ m_cActiveButtonColor = cl; m_bChangeColors = true; }
	u32 GetActiveButtonColor() const 			{ return m_cActiveButtonColor; }
	void SetGlobalButtonColor(u32 cl)			{ m_cGlobalButtonColor = cl; m_bChangeColors = true; }
	u32 GetGlobalButtonColor() const 			{ return m_cGlobalButtonColor; }

	TABS_VECTOR *		GetButtonsVector()		{ return &m_TabsArr; }
	CUIButton*			GetButtonByIndex(int i)	{ R_ASSERT(i>=0 && i<(int)m_TabsArr.size()); return m_TabsArr[i];}
	const shared_str	GetCommandName(int i)	{ return (GetButtonByIndex(i))->WindowName();};
protected:
	// ������ ������ - �������������� ��������
	TABS_VECTOR		m_TabsArr;

	// ������� ������� ������. -1 - �� ����, 0 - ������, 1 - ������, � �.�.
	int				m_iPushedIndex;
	int				m_iPrevPushedIndex;

	// ���� ���������� ���������
	u32				m_cGlobalTextColor;
	u32				m_cGlobalButtonColor;

	// ���� ������� �� �������� ��������
	u32				m_cActiveTextColor;
	u32				m_cActiveButtonColor;

	// ���������/��������� ������������ ������������
	bool			m_bAcceleratorsEnable;
	bool			m_bChangeColors;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUITabControl)
#undef script_type_list
#define script_type_list save_type_list(CUITabControl)

#endif