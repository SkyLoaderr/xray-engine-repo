//////////////////////////////////////////////////////////////////////
// UIScrollBar.h: полоса прокрутки для окон
//////////////////////////////////////////////////////////////////////

#ifndef _UI_SCROLLBAR_H_
#define _UI_SCROLLBAR_H_

#pragma once
#include "uiwindow.h"

#include "UIButton.h"
#include "UIScrollBox.h"


class CUIScrollBar :public CUIWindow
{
private:
	typedef CUIWindow inherited;
protected:
	//передвинуть каретку, если возможно
	bool			ScrollInc();
	bool			ScrollDec();

	//обновить полосу
	void			UpdateScrollBar();


	//горизонтальный или вертикальный 
	bool			m_bIsHorizontal;

	//кнопки скролинга
	CUIButton		m_DecButton;
	CUIButton		m_IncButton;

	//каретка скролинга
	CUIScrollBox	m_ScrollBox;

	//подложка для скролинга
	CUIStaticItem	m_StaticBackground;

	//текущая позиция
	int				m_iScrollPos;

	// step size
	int				m_iStepSize;

	//границы отображения
	int				m_iMinPos;
	int				m_iMaxPos;

	//размер отображаемой страницы
	int				m_iPageSize;

	// internal use
	int				m_ScrollWorkArea;
protected:
	int				ScrollSize			(){return _max(0,m_iMaxPos-m_iMinPos-m_iPageSize);}
	void			ClampByViewRect		();
	void			SetPosScrollFromView(int view_pos, int view_width, int view_offs);
	int				PosViewFromScroll	(int view_size, int view_offs);
	void			SetScrollPosClamped	(int iPos) { m_iScrollPos = iPos; clamp(m_iScrollPos,m_iMinPos,m_iMaxPos-m_iPageSize); }
public:
					CUIScrollBar	(void);
	virtual			~CUIScrollBar	(void);


	virtual void	Init			(int x, int y, int length, bool bIsHorizontal);

	//сообщения, отправляемые родительскому окну
//	typedef enum{VSCROLL, HSCROLL} E_MESSAGE;

	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void	OnMouse(int x, int y, EUIMessages mouse_action);

	virtual void	Draw			();

	virtual void	SetWidth		(int width);
	virtual void	SetHeight		(int height);

	virtual void	Reset			();
	// В листбоксе скроллбар глючит если мы удаляем элементы снизу листа, а скроллбар
	// в это время был в позиции не 0. Эта функция - фикс
	void			Refresh			();
	// скролинг
	// величина шага при нажатии кнопок или колеса мыши
	void			SetStepSize		(int step);
	// диапазон значений 
	void 			SetRange		(int iMin, int iMax);
	void 			GetRange		(int& iMin, int& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}
	int 			GetMaxRange		() {return m_iMaxPos;}
	int 			GetMinRange		() {return m_iMinPos;}
	// размер страницы (влияет на диапазон каретки от (m_iMinPos .. m_iMaxPos-m_iPageSize) )
	void			SetPageSize		(int iPage) { m_iPageSize = _max(0,iPage); UpdateScrollBar();}
	int				GetPageSize		() {return m_iPageSize;}
	// положение каретки
	void			SetScrollPos	(int iPos) { SetScrollPosClamped(iPos); UpdateScrollBar();}
	int				GetScrollPos	() {return m_iScrollPos;}
	// базовые размеры для кнопок
	enum {SCROLLBAR_WIDTH = 16, SCROLLBAR_HEIGHT = 16};
	void			TryScrollInc	();
	void			TryScrollDec	();
};


#endif