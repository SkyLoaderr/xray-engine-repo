//////////////////////////////////////////////////////////////////////
// UITabControl.h: ����� ���� � ����������.
//////////////////////////////////////////////////////////////////////

#ifndef UI_TABCONTROL_H_
#define UI_TABCONTROL_H_

#pragma once

#include "uiwindow.h"
#include "UIButton.h"

DEF_VECTOR (TABS_VECTOR, CUIButton*)

class CUITabControl: public CUIWindow
{
public:
	// Ctor and Dtor
	CUITabControl();
	virtual ~CUITabControl();

	// �������������  �� XML
	virtual void Init(int x, int y, int width, int height);

	// ��������� ������������ ������������� ����, ����� ��������� ����� �������� ���������
	typedef enum{TAB_CHANGED} E_MESSAGE;

	// ���������� ������-�������� � ������ �������� ��������
	bool AddItem(const char *pItemName, const char *pTexName, int x, int y, int width, int height);
	bool AddItem(CUIButton *pButton);

	// �������� ���������
	void RemoveItem(const u32 Index);
	void RemoveAll();

	// ��� ������� �� ���� �� ������ �� ���������� ������������ ��������.
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	int GetActiveIndex() { return m_iPushedIndex; }

	// ����� ���������� ��������
	const int GetTabsCount() const { return m_TabsArr.size(); }

	// ������� ����� ������� �������� ��������
	void SetNewActiveTab(const int iNewTab);

protected:
	// ������ ������ - �������������� ��������
	TABS_VECTOR m_TabsArr;

	// ������� ������� ������. -1 - �� ����, 0 - ������, 1 - ������, � �.�.
	int m_iPushedIndex;

	// ���� ������� �� ���������� ���������
	u32 m_cGlobalColor;

	// ���� ������� �� �������� ��������
	u32 m_cActiveColor;
};

#endif