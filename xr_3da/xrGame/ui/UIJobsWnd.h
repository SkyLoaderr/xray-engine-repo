//=============================================================================
//  Filename:   UIJobsWnd.h
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================

#ifndef UI_JOBS_WND_H_
#define UI_JOBS_WND_H_

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "xrXMLParser.h"

class CUIJobsWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	CUIJobsWnd() {}
	virtual ~CUIJobsWnd() {}
	virtual void Init(CUIXml &uiXml, CUIWindow *pNewParent);
	// Нет смысла скрывать листбокс, так как все равно пока это практически все, что есть, и
	// придется писать обертки над всеми управляющими функциями листбокса
	CUIListWnd		UIListWnd;
protected:
	// Название окна
	CUIStatic		UIStaticCaption;
};

#endif