//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: полоса процента выполнения
//////////////////////////////////////////////////////////////////////

#ifndef _UI_PROGRESSBAR_H_
#define _UI_PROGRESSBAR_H_

#pragma once

#include "uiwindow.h"
#include "uibutton.h"
#include "../script_export_space.h"


class CUIProgressBar : public CUIWindow
{
	friend class CUIXmlInit;
protected:
	//горизонтальный или вертикальный 
	bool			m_bIsHorizontal;

	//текущая позиция
	s16				m_iProgressPos;
	//границы отображения
	s16				m_iMinPos;
	s16				m_iMaxPos;

	//текущий состояние полосы в пикселях
	float				m_iCurrentLength;

	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	//вывод при помощи тайлов, иначе вся текстура - показывает 100% прогресса
	float				m_iProgressLength;

	//items
	CUIStaticItem	m_UIProgressItem;
	CUIStaticItem	m_UIBackgroundItem;
	bool			m_bBackgroundPresent;
	Fvector2		m_BackgroundOffset;

	//обновить полосу
	void			UpdateProgressBar();

	DECLARE_SCRIPT_REGISTER_FUNCTION
public:
	bool			m_bUseColor;
	Fcolor			m_minColor;
	Fcolor			m_maxColor;

public:
					CUIProgressBar(void);
	virtual			~CUIProgressBar(void);


	virtual void	Init(float x, float y, float length, float broad, bool bIsHorizontal);

	void			SetProgressTexture(LPCSTR tex_name, float progress_length, 
						float x, float y, float width, float height, u32 color = 0xFFFFFFFF);
	void			SetBackgroundTexture(LPCSTR tex_name, float x, float y, float width, float height, float offs_x, float offs_y);

	void			SetRange(s16 iMin, s16 iMax) {m_iMinPos = iMin;  m_iMaxPos = iMax;
						UpdateProgressBar();}
	void			GetRange(s16& iMin, s16& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}

	s16				GetRange_min() {return  m_iMinPos;}
	s16				GetRange_max() {return  m_iMaxPos;}

	void			SetProgressPos(s16 iPos) { m_iProgressPos = iPos; 
						UpdateProgressBar();}
	s16				GetScrollPos() {return m_iProgressPos;}

	//базовые размеры для кнопок
	//enum {PROGRESSBAR_WIDTH = 32, PROGRESSBAR_HEIGHT = 32};


	//передвинуть каретку, если возможно
	bool			ProgressInc();
	bool			ProgressDec();

	virtual void	Draw();
};

add_to_type_list(CUIProgressBar)
#undef script_type_list
#define script_type_list save_type_list(CUIProgressBar)

#endif //_UI_PROGRESSBAR_H_