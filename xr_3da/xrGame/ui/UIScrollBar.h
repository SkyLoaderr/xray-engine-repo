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
public:
	CUIScrollBar(void);
	virtual ~CUIScrollBar(void);


	virtual void Init(int x, int y, int length, bool bIsHorizontal);

	//сообщения, отправляемые родительскому окну
//	typedef enum{VSCROLL, HSCROLL} E_MESSAGE;

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Draw();

	virtual void SetWidth(int width);
	virtual void SetHeight(int height);

	virtual void Reset();
	// В листбоксе скроллбар глючит если мы удаляем элементы снизу листа, а скроллбар
	// в это время был в позиции не 0. Эта функция - фикс
	void		 Refresh();

	//скролинг
	void SetRange(s16 iMin, s16 iMax);
	void GetRange(s16& iMin, s16& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}
	int GetRangeSize() {return (m_iMaxPos-m_iMinPos);}
	int GetMaxRange() {return m_iMaxPos;}
	int GetMinRange() {return m_iMinPos;}

	void SetPageSize(u16 iPage) { m_iPageSize = iPage; UpdateScrollBar();}
	u16 GetPageSize() {return m_iPageSize;}

	void SetScrollPos(s16 iPos) { m_iScrollPos = iPos; UpdateScrollBar();}
	s16 GetScrollPos() {return m_iScrollPos;}

	//базовые размеры для кнопок
	enum {SCROLLBAR_WIDTH = 16, SCROLLBAR_HEIGHT = 16};

protected:
	//обновить полосу
	void UpdateScrollBar();

	//передвинуть каретку, если возможно
	bool ScrollInc();
	bool ScrollDec();

	//горизонтальный или вертикальный 
	bool m_bIsHorizontal;
	
	//кнопки скролинга
	CUIButton m_DecButton;
	CUIButton m_IncButton;

	//каретка скролинга
	CUIScrollBox m_ScrollBox;

	//подложка для скролинга
	CUIStaticItem m_StaticBackground;

	//текущая позиция
	s16 m_iScrollPos;

	//границы отображения
	s16 m_iMinPos;
	s16 m_iMaxPos;
	
	//размер отображаемой страницы
	u16 m_iPageSize;
};


#endif