//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: полоса процента выполнения
//////////////////////////////////////////////////////////////////////

#ifndef _UI_PROGRESSBAR_H_
#define _UI_PROGRESSBAR_H_

#pragma once

#include "uiwindow.h"
#include "uibutton.h"


class CUIProgressBar :
	public CUIWindow
{
public:
	CUIProgressBar(void);
	virtual ~CUIProgressBar(void);


	virtual void Init(int x, int y, int length, bool bIsHorizontal);

	void SetRange(s16 iMin, s16 iMax) {m_iMinPos = iMin;  m_iMaxPos = iMax;
						UpdateProgressBar();}
	void GetRange(s16& iMin, s16& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}


	void SetProgressPos(s16 iPos) { m_iProgressPos = iPos; 
						UpdateProgressBar();}
	s16 GetScrollPos() {return m_iProgressPos;}

	//базовые размеры для кнопок
	enum {PROGRESSBAR_WIDTH = 32, PROGRESSBAR_HEIGHT = 32};


	//передвинуть каретку, если возможно
	bool ProgressInc();
	bool ProgressDec();


	virtual void Draw();
						

protected:
	//обновить полосу
	void UpdateProgressBar();

	//горизонтальный или вертикальный 
	bool m_bIsHorizontal;
	
	//текущая позиция
	s16 m_iProgressPos;

	//границы отображения
	s16 m_iMinPos;
	s16 m_iMaxPos;
	
	//текущий состояние полосы в пикселях
	int m_iCurrentLength;


	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	CUIStaticItem m_UIStaticItem;
};


#endif //_UI_PROGRESSBAR_H_