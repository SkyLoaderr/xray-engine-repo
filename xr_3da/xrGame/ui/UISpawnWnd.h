
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

	// Установка нового режима отображения
	void SetDisplayMode(bool bDual = false);

	// -1 - еще не нажималась, 0 - primary (левая), 1 - secondary (правая)
	int GetPressingResult() { return m_iResult; }

//	void	SetCallbackFunc (ButtonClickCallback pFunc);

protected:
	// Подложка окна
	CUIFrameWindow		UIFrameWndPrimary;
	CUIFrameWindow		UIFrameWndSecondary;
	// Статик контролы для вывода текста
	CUIStatic			UIStaticTextPrimary;
	CUIStatic			UIStaticTextSecondary;
	CUIStatic			UITeamSign1;
	CUIStatic			UITeamSign2;

	// Кнопки
	CUIButton			UIButtonPrimary;
	CUIButton			UIButtonSecondary;

	// режим диалога: двойной, одиночный
	bool				m_bDual;
	// Индикатор нажатой кнопки: -1 - еще не нажималась, 0 - primary (левая), 1 - secondary (правая)
	int					m_iResult;

//	ButtonClickCallback	pCallbackFunc;
};

#endif