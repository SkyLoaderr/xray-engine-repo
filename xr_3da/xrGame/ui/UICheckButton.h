// UICheckButton.h: ����� ������, ������� 2 ���������:
// � �������� � ���
//////////////////////////////////////////////////////////////////////

#ifndef _UI_CHECK_BUTTON_H_
#define _UI_CHECK_BUTTON_H_

#pragma once

#include "ui3tbutton.h"

class CUICheckButton :public CUI3tButton
{
public:
	CUICheckButton(void);
	virtual ~CUICheckButton(void);

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void Init(int x, int y, int width, int height);
	virtual void OnMouseDown(bool left_button = true);
	virtual void SetTextX(int x) {/*do nothing*/}
//	virtual void SetLines(LPCSTR str);

	//��������� ������
	bool GetCheck() {return m_eButtonState == BUTTON_PUSHED;}
	void SetCheck(bool ch) {m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL;}

private:
	virtual void InitTexture();
};


#endif // _UI_CHECK_BUTTON_H_