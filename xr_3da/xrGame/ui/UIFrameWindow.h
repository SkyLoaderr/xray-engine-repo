// UIFrameWindow.h: 
//
// ���� �������������� ����. ����� ����� CUIFrameRect
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiwindow.h"

#include "..\uiframerect.h"



class CUIFrameWindow: public CUIWindow
{
public:
	////////////////////////////////////
	//�����������/����������
	CUIFrameWindow();
	virtual ~CUIFrameWindow();

	////////////////////////////////////
	//�������������
	virtual void Init(LPCSTR base_name, int x, int y, int width, int height);
	virtual void Init(LPCSTR base_name, RECT* pRect);

	////////////////////////////////////
	//���������� ����
	virtual void Draw();
	

protected:

	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	CUIFrameRect m_UIWndFrame;

};