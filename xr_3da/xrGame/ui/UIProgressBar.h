//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: ������ �������� ����������
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

	//������� ������� ��� ������
	enum {PROGRESSBAR_WIDTH = 32, PROGRESSBAR_HEIGHT = 32};


	//����������� �������, ���� ��������
	bool ProgressInc();
	bool ProgressDec();


	virtual void Draw();
						

protected:
	//�������� ������
	void UpdateProgressBar();

	//�������������� ��� ������������ 
	bool m_bIsHorizontal;
	
	//������� �������
	s16 m_iProgressPos;

	//������� �����������
	s16 m_iMinPos;
	s16 m_iMaxPos;
	
	//������� ��������� ������ � ��������
	int m_iCurrentLength;


	///////////////////////////////////////	
	//����������� ��������� ��� ���������
	///////////////////////////////////////
	CUIStaticItem m_UIStaticItem;
};


#endif //_UI_PROGRESSBAR_H_