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
#include "UITreeViewItem.h"

#include "xrXMLParser.h"

class CUINewsWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
	CUINewsWnd();
	virtual ~CUINewsWnd();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);

	virtual void Init();

	// Окно вывода информации
	CUIListWnd	UIListWnd;

	// Получить заголовок раздела
	virtual ref_str		WindowName();
};

#endif