// UICheckButton.h: ����� ������, ������� 2 ���������:
// � �������� � ���
//////////////////////////////////////////////////////////////////////

#ifndef _UI_CHECK_BUTTON_H_
#define _UI_CHECK_BUTTON_H_

#pragma once

#include "uibutton.h"

class CUICheckButton :public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUICheckButton(void);
	virtual ~CUICheckButton(void);

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//���������, ������������ ������������� ����
	typedef enum{BUTTON_CLICKED, CHECK_BUTTON_SET, CHECK_BUTTON_RESET} E_MESSAGE;

	//���������� ����
	virtual void Draw();
	virtual void Update();


	//��������� ������
	bool GetCheck() {return m_bIsChecked;}
	void SetCheck() {m_bIsChecked = true;}
	void ResetCheck() {m_bIsChecked = false;}


protected:
	//���� ����������� ������� 
	bool m_bIsChecked;
};


#endif // _UI_CHECK_BUTTON_H_