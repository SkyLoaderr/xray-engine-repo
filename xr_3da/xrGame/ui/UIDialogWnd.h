// UIDialogWnd.h: ����� �������� �������, ����� ��� ������������ �������
// ������ ������� ����� ������ ����������� Show() � Hide()
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DIALOG_WND_H_
#define _UI_DIALOG_WND_H_

#pragma once

#include "uiwindow.h"


class CUIDialogWnd : public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIDialogWnd();
	virtual ~ CUIDialogWnd();

	virtual void Show();
	virtual void Hide();

	virtual bool IR_OnKeyboardPress(int dik);
	virtual bool IR_OnKeyboardRelease(int dik);
	virtual bool IR_OnMouseMove(int dx, int dy);
protected:
	//��� �� ������� ������-������ HUD ����� ������� ����
	bool m_bCrosshair;
};

#endif // _UI_DIALOG_WND_H_
