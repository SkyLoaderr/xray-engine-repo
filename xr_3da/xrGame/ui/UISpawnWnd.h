
#ifndef UISPAWNWND_H_
#define UISPAWNWND_H_

#pragma once

#include "UIDialogWnd.h"
#include "UIFrameWindow.h"
#include "UIButton.h"

typedef	void (*ButtonClickCallback) (int);

class CUISpawnWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUISpawnWnd();
	virtual ~CUISpawnWnd();

	virtual void Init(const char *strCaptionPrimary, const char *strCaptionSecondary, bool bDual = false);
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	// ��������� ������ ������ �����������
	void SetDisplayMode(bool bDual = false);

	// -1 - ��� �� ����������, 0 - primary (�����), 1 - secondary (������)
	int GetPressingResult() { return m_iResult; }

	void	SetCallbackFunc (ButtonClickCallback pFunc);

protected:
	// �������� ����
	CUIFrameWindow		UIFrameWndPrimary;
	CUIFrameWindow		UIFrameWndSecondary;
	// ������ �������� ��� ������ ������
	CUIStatic			UIStaticTextPrimary;
	CUIStatic			UIStaticTextSecondary;
	// ������
	CUIButton			UIButtonPrimary;
	CUIButton			UIButtonSecondary;

	// ����� �������: �������, ���������
	bool				m_bDual;
	// ��������� ������� ������: -1 - ��� �� ����������, 0 - primary (�����), 1 - secondary (������)
	int					m_iResult;

	ButtonClickCallback	pCallbackFunc;
};

#endif