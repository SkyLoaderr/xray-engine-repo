// UIButton.h: ����� ���������� ������
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_BUTTON_H_
#define _UI_BUTTON_H_

#pragma once


#include "UIWindow.h"


#include "..\uistaticitem.h"


class CUIButton : public CUIWindow  
{
public:
	 CUIButton();
	virtual ~ CUIButton();


	virtual void Init(int x, int y, int width, int height);
	
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//���������, ������������ ������������� ����
	typedef enum{BUTTON_CLICKED} E_MESSAGE;

	//���������� ����
	virtual void Draw();


	//������ � ������� ����� �������� ������
	typedef enum{NORMAL_PRESS, //������ ���������� ��� 
							   //������� � ���������� �� ��� ����
				 DOWN_PRESS,   //����� ��� �������
				 UP_PRESS      //����� ��� ����������
			} E_PRESS_MODE;

	void SetPressMode(E_PRESS_MODE ePressMode) {m_ePressMode = ePressMode;}
	E_PRESS_MODE GetPressMode() {return m_ePressMode;}

protected:
	
	//��������� � ������� ���������� ������
	typedef enum{BUTTON_NORMAL, //������ ����� �� �������������
				 BUTTON_PUSHED,   //� ������� ��������
				 BUTTON_UP      //��� ������������ ������ ���� 
			} E_BUTTON_STATE;
	
	E_BUTTON_STATE m_eButtonState;

	//���� ������ ���� ������ ��� ������
	bool m_bButtonClicked;

	//����� � ������� ���������� ������
	E_PRESS_MODE m_ePressMode;


	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	CUIStaticItem m_UIStaticNormal;
	CUIStaticItem m_UIStaticPushed;

};

#endif // _UI_BUTTON_H_
