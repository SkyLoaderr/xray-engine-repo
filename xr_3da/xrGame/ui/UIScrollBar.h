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
	u32				m_iScrollPos;

	// step size
	u32				m_iStepSize;

	//границы отображения
	u32				m_iMinPos;
	u32				m_iMaxPos;

	//размер отображаемой страницы
	u32				m_iPageSize;

	// internal use
	u32				m_ScrollWorkArea;
	bool			m_b_enabled;
protected:
	u32				ScrollSize			(){return _max(0,m_iMaxPos-m_iMinPos-m_iPageSize+1);}
	void			ClampByViewRect		();
	void			SetPosScrollFromView(int view_pos, int view_width, int view_offs);
	int				PosViewFromScroll	(int view_size, int view_offs);
	void			SetScrollPosClamped	(u32 iPos) { 
														m_iScrollPos = iPos; 
														clamp(m_iScrollPos,m_iMinPos,m_iMaxPos-m_iPageSize+1); }
public:
					CUIScrollBar	(void);
	virtual			~CUIScrollBar	(void);

			void	SetEnabled		(bool b)			{m_b_enabled = b;if(!m_b_enabled)Show(m_b_enabled);}
			bool	GetEnabled		()					{return m_b_enabled;}
	virtual void	Show			(bool b);
	virtual void	Enable			(bool b);
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
	void			SetStepSize		(u32 step);
	// диапазон значений 
	void 			SetRange		(u32 iMin, u32 iMax);
	void 			GetRange		(u32& iMin, u32& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}
	u32 			GetMaxRange		() {return m_iMaxPos;}
	u32 			GetMinRange		() {return m_iMinPos;}
	// размер страницы (влияет на диапазон каретки от (m_iMinPos .. m_iMaxPos-m_iPageSize) )
	void			SetPageSize		(u32 iPage) { m_iPageSize = _max(0,iPage); UpdateScrollBar();}
	u32				GetPageSize		() {return m_iPageSize;}
	// положение каретки
	void			SetScrollPos	(u32 iPos) { SetScrollPosClamped(iPos); UpdateScrollBar();}
	int				GetScrollPos	() {return _max(m_iMinPos,m_iScrollPos);}
	// базовые размеры для кнопок
	enum {SCROLLBAR_WIDTH = 16, SCROLLBAR_HEIGHT = 16};
	void			TryScrollInc	();
	void			TryScrollDec	();
};


#endif