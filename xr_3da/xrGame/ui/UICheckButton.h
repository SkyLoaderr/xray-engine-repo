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

	virtual void OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void Init(float x, float y, float width, float height);
	virtual void OnMouseDown(bool left_button = true);
	virtual void SetTextX(float x) {/*do nothing*/}
//	virtual void SetLines(LPCSTR str);

	//��������� ������
	bool GetCheck() {return m_eButtonState == BUTTON_PUSHED;}
	void SetCheck(bool ch) {m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL;}

private:
	virtual void InitTexture();
};


#endif // _UI_CHECK_BUTTON_H_