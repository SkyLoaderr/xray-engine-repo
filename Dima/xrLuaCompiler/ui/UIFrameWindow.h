// UIFrameWindow.h: 
//
// окно осуществялющие граф. вывод через CUIFrameRect
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiwindow.h"

#include "..\uiframerect.h"
#include "..\uistaticitem.h"



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

	//для статических спрайтов, перекрывающий окно
	void InitLeftTop(LPCSTR tex_name, int left_offset, int up_offset);
	void InitLeftBottom(LPCSTR tex_name, int left_offset, int up_offset);


	////////////////////////////////////
	//прорисовка окна
	virtual void Draw();
	

protected:

	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	
	//основной фрейм 
	CUIFrameRect m_UIWndFrame;
	
	//заголовки поверх него

	bool m_bOverLeftTop; 
	bool m_bOverLeftBottom;

	int m_iLeftTopOffset;
	int m_iUpTopOffset;

	int m_iLeftBottomOffset;
	int m_iUpBottomOffset;

	CUIStaticItem m_UIStaticOverLeftTop;
	CUIStaticItem m_UIStaticOverLeftBottom;

};