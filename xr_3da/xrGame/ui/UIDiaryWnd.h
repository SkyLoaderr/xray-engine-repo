// CUIDiaryWnd.h:  дневник и все что с ним связано
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "UITabControl.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"

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
protected:
	// Дочерние окна входящие в окно информации
	CUINewsWnd			UINewsWnd;
	CUIJobsWnd			UIJobsWnd;
	CUINotesWnd			UINotesWnd;

	// Текущий активный поддиалог
	CUIDialogWnd		*m_pActiveSubdialog;
	// TreeView
	CUIListWnd			UITreeView;
	// Подложка под TreeView
	CUIFrameWindow		UITreeViewBg;
	CUIFrameLineWnd		UITreeViewHeader;
	// Анисационная иконка
	CUIAnimatedStatic	UIAnimation;

	// Подложка под основное поле
	CUIFrameWindow		UIFrameWnd;
	// Хидер
	CUIFrameLineWnd		UIFrameWndHeader;

	// Инициализируем TreeView
	void				InitTreeView();
};