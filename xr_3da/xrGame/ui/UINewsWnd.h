//=============================================================================
//  Filename:   UINewsWnd.h
//  News subwindow in PDA dialog
//=============================================================================

#ifndef UI_NEWS_WND_H_
#define UI_NEWS_WND_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "UITabControl.h"
#include "UIFrameWindow.h"

#include "xrXMLParser.h"

class CUINewsWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	CUINewsWnd();
	virtual ~CUINewsWnd();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);

	virtual void Init(CUIXml &uiXml, CUIWindow *pNewParent);

	// Окно вывода информации
	CUIListWnd	UIListWnd;
protected:
	// Название окна в верхнем правом углу...
	CUIStatic	UIStaticCaptionMain;
	// ...и в окне вывода информации
	CUIStatic	UIStaticCaptionCenter;
};

#endif