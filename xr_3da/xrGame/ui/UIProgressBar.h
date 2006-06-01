//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: ������ �������� ����������
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
	//�������������� ��� ������������ 
	bool			m_bIsHorizontal;

	//������� �������
	s16				m_iProgressPos;
	//������� �����������
	s16				m_iMinPos;
	s16				m_iMaxPos;

	//������� ��������� ������ � ��������
	float				m_iCurrentLength;

	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	//����� ��� ������ ������, ����� ��� �������� - ���������� 100% ���������
	//float				m_iProgressLength;


	bool			m_bBackgroundPresent;
	Fvector2		m_BackgroundOffset;

	//�������� ������
	void			UpdateProgressBar();

	DECLARE_SCRIPT_REGISTER_FUNCTION
public:
	bool			m_bUseColor;
	Fcolor			m_minColor;
	Fcolor			m_maxColor;

public:
	//items
	CUIStatic		m_UIProgressItem;
	CUIStatic		m_UIBackgroundItem;


					CUIProgressBar(void);
	virtual			~CUIProgressBar(void);


	virtual void	Init(float x, float y, float width, float height, bool bIsHorizontal);

	void			SetRange(s16 iMin, s16 iMax) {m_iMinPos = iMin;  m_iMaxPos = iMax;
						UpdateProgressBar();}
	void			GetRange(s16& iMin, s16& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}

	s16				GetRange_min() {return  m_iMinPos;}
	s16				GetRange_max() {return  m_iMaxPos;}

	void			SetProgressPos(s16 iPos) { m_iProgressPos = iPos; 
						UpdateProgressBar();}
	s16				GetScrollPos() {return m_iProgressPos;}

	//������� ������� ��� ������
	//enum {PROGRESSBAR_WIDTH = 32, PROGRESSBAR_HEIGHT = 32};


	//����������� �������, ���� ��������
	bool			ProgressInc();
	bool			ProgressDec();

	virtual void	Draw();
};

add_to_type_list(CUIProgressBar)
#undef script_type_list
#define script_type_list save_type_list(CUIProgressBar)

#endif //_UI_PROGRESSBAR_H_