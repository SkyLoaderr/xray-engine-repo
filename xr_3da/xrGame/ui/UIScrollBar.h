//////////////////////////////////////////////////////////////////////
// UIScrollBar.h: ������ ��������� ��� ����
//////////////////////////////////////////////////////////////////////

#ifndef _UI_SCROLLBAR_H_
#define _UI_SCROLLBAR_H_

#pragma once
#include "uiwindow.h"

#include "UIButton.h"
#include "UIScrollBox.h"


class CUIScrollBar :
	public CUIWindow
{
public:
	CUIScrollBar(void);
	virtual ~CUIScrollBar(void);


	virtual void Init(int x, int y, int length, bool bIsHorizontal);

	//���������, ������������ ������������� ����
	typedef enum{VSCROLL, HSCROLL} E_MESSAGE;

	virtual void SendMessage(CUIWindow *pWnd, u16 msg, void *pData);


	//��������
	void SetRange(s16 iMin, s16 iMax) {m_iMinPos = iMin;  m_iMaxPos = iMax;
										UpdateScrollBar();}
	void GetRange(s16& iMin, s16& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}

	void SetPageSize(u16 iPage) { m_iPageSize = iPage; UpdateScrollBar();}
	u16 GetPageSize() {return m_iPageSize;}

	void SetScrollPos(s16 iPos) { m_iScrollPos = iPos; UpdateScrollBar();}
	s16 GetScrollPos() {return m_iScrollPos;}

	//������� ������� ��� ������
	enum {SCROLLBAR_WIDTH = 32, SCROLLBAR_HEIGHT = 32};

protected:
	//�������� ������
	void UpdateScrollBar();

	//����������� �������, ���� ��������
	bool ScrollInc();
	bool ScrollDec();

	//�������������� ��� ������������ 
	bool m_bIsHorizontal;
	
	//������ ���������
	CUIButton m_DecButton;
	CUIButton m_IncButton;

	//������� ���������
	CUIScrollBox m_ScrollBox;

	//������� �������
	s16 m_iScrollPos;

	//������� �����������
	s16 m_iMinPos;
	s16 m_iMaxPos;
	
	//������ ������������ ��������
	u16 m_iPageSize;
};


#endif