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
	virtual void Init(int x, int y, int width, int height);
	
	//���������� ����
	virtual void Draw();
	//���������� ����� �����������
	virtual void Update();


	void SetText(LPSTR str) {m_str = str;}
	LPSTR GetText() {return m_str;}

	void SetColor(u32 color) {m_UIStaticItem.SetColor(color);}

protected:
	

	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	bool m_bAvailableTexture;
	CUIStaticItem m_UIStaticItem;


	//�����
	LPSTR m_str;


	/////////////////////////////////////
	//��������������� ����� ������
	/////////////////////////////////////
	void WordOut();
	void AddLetter(char letter);
	u32 ReadColor(int pos, int& r, int& g, int& b);
	
	//��������� ������� �������
	int curretX;
	int curretY;
	//��������� �����
	int outX;
	int outY;
	char* buf_str;	

	bool new_word;
	int word_length;
	
	int space_width;
	int word_width;
	
};

#endif // _UI_STATIC_H_
