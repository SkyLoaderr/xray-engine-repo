// UIDialogWnd.h: класс простого диалога, нужен для стандартного запуска
// разным менюшек путем вызова виртуальных Show() И Hide()
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DIALOG_WND_H_
#define _UI_DIALOG_WND_H_

#pragma once

#include "uiwindow.h"
class CDialogHolder;

class CUIDialogWnd : public CUIWindow  
{
private:
	typedef CUIWindow inherited;
	CDialogHolder*	m_pHolder;
public:
	CUIDialogWnd								();
	virtual ~ CUIDialogWnd						();

	virtual void Show							();
	virtual void Hide							();

	virtual bool IR_OnKeyboardPress				(int dik);
	virtual bool IR_OnKeyboardRelease			(int dik);
	virtual bool IR_OnMouseMove					(int dx, int dy);

	CDialogHolder* GetHolder					()					{return m_pHolder;};
			void SetHolder						(CDialogHolder* h)	{m_pHolder = h;};

};

#endif // _UI_DIALOG_WND_H_
