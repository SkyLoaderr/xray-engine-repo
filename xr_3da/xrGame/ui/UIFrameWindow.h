// UIFrameWindow.h: 
//
// окно осуществялющие граф. вывод через CUIFrameRect
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiwindow.h"

#include "..\uiframerect.h"



class CUIFrameWindow: public CUIWindow
{
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIFrameWindow();
	virtual ~CUIFrameWindow();

	////////////////////////////////////
	//инициализация
	virtual void Init(LPCSTR base_name, int x, int y, int width, int height);
	virtual void Init(LPCSTR base_name, RECT* pRect);

	////////////////////////////////////
	//прорисовка окна
	virtual void Draw();
	

protected:

	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	CUIFrameRect m_UIWndFrame;

};