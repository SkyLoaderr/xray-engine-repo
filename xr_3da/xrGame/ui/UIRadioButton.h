//////////////////////////////////////////////////////////////////////
// UIRadioButton.h: ����� ������, ������� 2 ���������
// � ���������� � ������ � ������ �� ��������
//////////////////////////////////////////////////////////////////////

#ifndef _UI_RADIO_BUTTON_H_
#define _UI_RADIO_BUTTON_H_


#pragma once
#include "uicheckbutton.h"

class CUIRadioButton :
	public CUICheckButton
{
public:
	CUIRadioButton(void);
	virtual ~CUIRadioButton(void);

	//���������, ������������ ������������� ����
//	typedef enum{BUTTON_CLICKED, CHECK_BUTTON_SET, CHECK_BUTTON_RESET, 
//					RADIO_BUTTON_SET} E_MESSAGE;
	
	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	//���������� ����
	virtual void Draw();
	virtual void Update();

};


#endif