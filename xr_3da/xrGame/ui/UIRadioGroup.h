/////////////////////////////////////////////////////////////////////
// CUIRadioGroup.h: ����� ��� ������ � ������� ����� ������
//////////////////////////////////////////////////////////////////////

#ifndef _UI_RADIO_GROUP_H_
#define _UI_RADIO_GROUP_H_





#pragma once

#include "uiwindow.h"
#include "uiradiobutton.h"


class CUIRadioGroup :
	public CUIWindow
{
public:
	CUIRadioGroup(void);
	virtual ~CUIRadioGroup(void);

	////////////////////////////////////
	//������ � ��������� � ������������� ������
	void AttachChild(CUIRadioButton* pChild);
	void DetachChild(CUIRadioButton* pChild);

	
	virtual void SendMessage(CUIWindow *pWnd, u16 msg, void *pData);

	//���������/��������� ������� � ������
	int GetCheckedPos();	
	void SetCheckedPos(int new_pos);

};


#endif