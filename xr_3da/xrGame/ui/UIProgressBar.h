//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: полоса процента выполнения
//////////////////////////////////////////////////////////////////////

#ifndef _UI_PROGRESSBAR_H_
#define _UI_PROGRESSBAR_H_

#pragma once

#include "uiwindow.h"
#include "uibutton.h"
#include "../script_export_space.h"


class CUIProgressBar :
	public CUIWindow
{
public:
	CUIProgressBar(void);
	virtual ~CUIProgressBar(void);


	virtual void Init(int x, int y, int length, int broad, bool bIsHorizontal);

	void SetProgressTexture(const char* tex_name, int progress_length, bool tile, 
						u32 color = 0xFFFFFFFF);
	void SetBackgroundTexture(const char* tex_name, int left_offset, int up_offset);


	void SetRange(s16 iMin, s16 iMax) {m_iMinPos = iMin;  m_iMaxPos = iMax;
						UpdateProgressBar();}
	void GetRange(s16& iMin, s16& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}

	s16 GetRange_min() {return  m_iMinPos;}
	s16 GetRange_max() {return  m_iMaxPos;}

	void SetProgressPos(s16 iPos) { m_iProgressPos = iPos; 
						UpdateProgressBar();}
	s16 GetScrollPos() {return m_iProgressPos;}

	//базовые размеры для кнопок
	//enum {PROGRESSBAR_WIDTH = 32, PROGRESSBAR_HEIGHT = 32};


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
	//полоса прогресса
	CUIStaticItem m_UIStaticItem;
	//вывод при помощи тайлов, иначе вся текстура - показывает 100% прогресса
	bool m_bProgressTile;
	int m_iProgressLength;
	
	//подложка
	CUIStaticItem m_UIBackgroundItem;

	bool m_bBackgroundPresent;
	int m_iBackgroundLeftOffset;
	int m_iBackgroundUpOffset;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIProgressBar)
#undef script_type_list
#define script_type_list save_type_list(CUIProgressBar)

#endif //_UI_PROGRESSBAR_H_