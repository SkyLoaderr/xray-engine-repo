
#ifndef UISPAWNWND_H_
#define UISPAWNWND_H_

#pragma once

#include "UIDialogWnd.h"
#include "UIFrameWindow.h"
#include "UIButton.h"

//typedef	void (*ButtonClickCallback) (int);

class CUISpawnWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUISpawnWnd();
	virtual ~CUISpawnWnd();

	virtual void Init(	const char *strCaptionPrimary, const u32 ColorPrimary,
						const char *strCaptionSecondary, const u32 ColorSecondary, 
						bool bDual = false);
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);

	// ��������� ������ ������ �����������
	void SetDisplayMode(bool bDual = false);

	// -1 - ��� �� ����������, 0 - primary (�����), 1 - secondary (������)
	int GetPressingResult() { return m_iResult; }

//	void	SetCallbackFunc (ButtonClickCallback pFunc);

protected:
	// �������� ����
	CUIFrameWindow		UIFrameWndPrimary;
	CUIFrameWindow		UIFrameWndSecondary;
	// ������ �������� ��� ������ ������
	CUIStatic			UIStaticTextPrimary;
	CUIStatic			UIStaticTextSecondary;
	CUIStatic			UITeamSign1;
	CUIStatic			UITeamSign2;

	// ������
	CUIButton			UIButtonPrimary;
	CUIButton			UIButtonSecondary;

	// ����� �������: �������, ���������
	bool				m_bDual;
	// ��������� ������� ������: -1 - ��� �� ����������, 0 - primary (�����), 1 - secondary (������)
	int					m_iResult;

//	ButtonClickCallback	pCallbackFunc;
};

#endif