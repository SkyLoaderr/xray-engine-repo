//////////////////////////////////////////////////////////////////////
// UIScrollBar.h: ������ ��������� ��� ����
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
	//����������� �������, ���� ��������
	bool			ScrollInc();
	bool			ScrollDec();

	//�������� ������
	void			UpdateScrollBar();


	//�������������� ��� ������������ 
	bool			m_bIsHorizontal;

	//������ ���������
	CUIButton		m_DecButton;
	CUIButton		m_IncButton;

	//������� ���������
	CUIScrollBox	m_ScrollBox;

	//�������� ��� ���������
	CUIStaticItem	m_StaticBackground;

	//������� �������
	int				m_iScrollPos;

	// step size
	int				m_iStepSize;

	//������� �����������
	int				m_iMinPos;
	int				m_iMaxPos;

	//������ ������������ ��������
	int				m_iPageSize;

	// internal use
	int				m_ScrollWorkArea;
protected:
	int				ScrollSize			(){return _max(0,m_iMaxPos-m_iMinPos-m_iPageSize+1);}
	void			ClampByViewRect		();
	void			SetPosScrollFromView(int view_pos, int view_width, int view_offs);
	int				PosViewFromScroll	(int view_size, int view_offs);
	void			SetScrollPosClamped	(int iPos) { 
														m_iScrollPos = iPos; 
														clamp(m_iScrollPos,m_iMinPos,m_iMaxPos-m_iPageSize+1); }
public:
					CUIScrollBar	(void);
	virtual			~CUIScrollBar	(void);


	virtual void	Init			(int x, int y, int length, bool bIsHorizontal);

	//���������, ������������ ������������� ����
//	typedef enum{VSCROLL, HSCROLL} E_MESSAGE;

	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void	OnMouse(int x, int y, EUIMessages mouse_action);

	virtual void	Draw			();

	virtual void	SetWidth		(int width);
	virtual void	SetHeight		(int height);

	virtual void	Reset			();
	// � ��������� ��������� ������ ���� �� ������� �������� ����� �����, � ���������
	// � ��� ����� ��� � ������� �� 0. ��� ������� - ����
	void			Refresh			();
	// ��������
	// �������� ���� ��� ������� ������ ��� ������ ����
	void			SetStepSize		(int step);
	// �������� �������� 
	void 			SetRange		(int iMin, int iMax);
	void 			GetRange		(int& iMin, int& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}
	int 			GetMaxRange		() {return m_iMaxPos;}
	int 			GetMinRange		() {return m_iMinPos;}
	// ������ �������� (������ �� �������� ������� �� (m_iMinPos .. m_iMaxPos-m_iPageSize) )
	void			SetPageSize		(int iPage) { m_iPageSize = _max(0,iPage); UpdateScrollBar();}
	int				GetPageSize		() {return m_iPageSize;}
	// ��������� �������
	void			SetScrollPos	(int iPos) { SetScrollPosClamped(iPos); UpdateScrollBar();}
	int				GetScrollPos	() {return _max(m_iMinPos,m_iScrollPos);}
	// ������� ������� ��� ������
	enum {SCROLLBAR_WIDTH = 16, SCROLLBAR_HEIGHT = 16};
	void			TryScrollInc	();
	void			TryScrollDec	();
};


#endif