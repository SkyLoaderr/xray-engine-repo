//=============================================================================
//  Filename:   UIJobsWnd.h
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================

#ifndef UI_JOBS_WND_H_
#define UI_JOBS_WND_H_

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIListWnd.h"
#include "xrXMLParser.h"

#include "../GameTask.h"

class CUIJobsWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	CUIJobsWnd();
	virtual ~CUIJobsWnd();
	virtual void Init();
	// Добавить 1 таск
	void AddTask(CGameTask * const task);
//	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);
	// Нет смысла скрывать листбокс, так как все равно пока это практически все, что есть, и
	// придется писать обертки над всеми управляющими функциями листбокса
	CUIListWnd		UIList;
protected:
	// Добавляем заголовок крупным шрифтом
	void AddHeader(ref_str strHeader);
	// Добавляем надпись с иконкой
	// Фрейи с описанием задач задачами
	CUIFrameWindow	UIMainJobsFrame;
	// Декоративный заголовок
	CUIFrameLineWnd	UIMJFHeader;

	CGameFont			*pHeaderFnt;
	CGameFont			*pSubTasksFnt;

};

#endif