
#ifndef UISPAWNWND_H_
#define UISPAWNWND_H_

#pragma once

#include "UIDialogWnd.h"
//#include "UIFrameWindow.h"
//#include "UIButton.h"

class CUIStatix;
class CUIStatic;

//typedef	void (*ButtonClickCallback) (int);

class CUISpawnWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUISpawnWnd();
	virtual ~CUISpawnWnd();

	virtual void Init();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);

	// ��������� ������ ������ �����������
	void SetDisplayMode(bool bDual = false);

	// -1 - ��� �� ����������, 0 - primary (�����), 1 - secondary (������)
	int GetPressingResult() { return 1; }

//	void	SetCallbackFunc (ButtonClickCallback pFunc);

protected:
	CUIStatic*		m_pCaption;
	CUIStatic*		m_pBackground;
	CUIStatic*		m_pFrames[3];
	CUIStatix*		m_pImage1;
	CUIStatix*		m_pImage2;
};

#endif