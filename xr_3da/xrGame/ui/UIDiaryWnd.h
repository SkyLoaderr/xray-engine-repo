// CUIDiaryWnd.h:  дневник и все что с ним связано
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "UITabControl.h"
#include "UIFrameWindow.h"

#include "UINewsWnd.h"
#include "UIJobsWnd.h"
#include "UINotesWnd.h"

///////////////////////////////////////
// Дневник
///////////////////////////////////////

class CUIDiaryWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	// Ctor and Dtor
	CUIDiaryWnd();
	virtual ~CUIDiaryWnd();

	virtual void Init();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	// Добавить новую новость
	void AddNewsItem(const char *sData);
private:
	// Дочерние окна входящие в окно информации
	CUINewsWnd		UINewsWnd;
	CUIJobsWnd		UIJobsWnd;
	CUINotesWnd		UINotesWnd;

	// Текущая активняа закладка
	CUIDialogWnd	*m_pActiveSubdialog;
	// Табконтрол-переключатель подзакладок
	CUITabControl	UITabControl;
};