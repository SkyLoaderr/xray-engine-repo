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
	virtual void Show();
	// Добавить 1 таск
	void AddTask(CGameTask * const task);
	// Нет смысла скрывать листбокс, так как все равно пока это практически все, что есть, и
	// придется писать обертки над всеми управляющими функциями листбокса
	CUIListWnd		UIList;
	// Загрузить jobs
	void ReloadJobs();

	// Установить фильтр
	void SetFilter(ETaskState newFilter);
	// Получить заголовок раздела
	virtual ref_str		DialogName();
protected:
	// Добавляем заголовок крупным шрифтом
	void AddHeader(ref_str strHeader);

	CGameFont			*pHeaderFnt;
	CGameFont			*pSubTasksFnt;

	// CurrentFilter
	ETaskState			filter;
};

#endif