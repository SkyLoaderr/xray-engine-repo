// UIDialogWnd.h: ����� �������� �������, ����� ��� ������������ �������
// ������ ������� ����� ������ ����������� Show() � Hide()
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
	virtual bool IR_OnMouseWheel				(int direction);
	virtual bool OnKeyboard						(int dik, EUIMessages keyboard_action);

	CDialogHolder* GetHolder					()					{return m_pHolder;};
			void SetHolder						(CDialogHolder* h)	{m_pHolder = h;};
	virtual bool StopAnyMove					(){return true;}
};

#endif // _UI_DIALOG_WND_H_
