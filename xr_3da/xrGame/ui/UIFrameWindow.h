// UIFrameWindow.h: 
//
// окно осуществялющие граф. вывод через CUIFrameRect
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiwindow.h"
#include "uistatic.h"

#include "../uiframerect.h"
#include "../uistaticitem.h"




class CUIFrameWindow: public CUIWindow
{
private:
	typedef CUIWindow inherited;
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

	virtual void SetWidth(int width);
	virtual void SetHeight(int height);
	
	// Устанавливаем цвет всего фрейма
	void SetColor(u32 cl);

	////////////////////////////////////
	//прорисовка окна
	virtual void Draw();
	
	//текст заголовка
	CUIStatic UITitleText;
	CUIStatic*	GetTitleStatic(){return &UITitleText;};

	bool		GetClipper()							{ return m_bClipper; }
	void		SetClipper(bool value, RECT clipRect)	{ m_bClipper = value; m_ClipRect = clipRect; }

protected:

	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	
	//основной фрейм 
	CUIFrameRect m_UIWndFrame;

	void		FrameClip(const RECT parentAbsR);
	
	//заголовки поверх него

	bool m_bOverLeftTop; 
	bool m_bOverLeftBottom;

	int m_iLeftTopOffset;
	int m_iUpTopOffset;

	int m_iLeftBottomOffset;
	int m_iUpBottomOffset;

	CUIStaticItem m_UIStaticOverLeftTop;
	CUIStaticItem m_UIStaticOverLeftBottom;

	// clipper
	bool	m_bClipper;
	RECT	m_ClipRect;

private:
	inline void ClampMax_Zero(Irect &r);

};
	