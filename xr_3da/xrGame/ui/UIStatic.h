// UIStatic.h: ����� ������������ ��������
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_STATIC_H_
#define _UI_STATIC_H_

#pragma once


#include "uiwindow.h"


#include "..\uistaticitem.h"



class CUIStatic : public CUIWindow  
{
public:
	CUIStatic();
	virtual ~ CUIStatic();


	virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);

	//���������� ����
	virtual void Draw();
	//���������� ����� �����������
	virtual void Update();


	void SetText(LPSTR str) {m_str = str;}
	LPSTR GetText() {return m_str;}


protected:
	

	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	CUIStaticItem m_UIStaticItem;


	//�����
	LPSTR m_str;

	
};

#endif // _UI_STATIC_H_
