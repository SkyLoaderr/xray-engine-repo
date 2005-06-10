//////////////////////////////////////////////////////////////////////
// UIScrollBar.h: ������ ��������� ��� ����
//////////////////////////////////////////////////////////////////////

#ifndef _UI_SCROLLBAR_H_
#define _UI_SCROLLBAR_H_

#pragma once
#include "uiwindow.h"

#include "UIButton.h"
#include "UIScrollBox.h"

#define SCROLLBAR_WIDTH  16.0f
#define SCROLLBAR_HEIGHT 16.0f


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
	u32				m_iScrollPos;

	// step size
	u32				m_iStepSize;

	//������� �����������
	u32				m_iMinPos;
	u32				m_iMaxPos;

	//������ ������������ ��������
	u32				m_iPageSize;

	// internal use
	u32				m_ScrollWorkArea;
	bool			m_b_enabled;
protected:
	u32				ScrollSize			(){return _max(0,m_iMaxPos-m_iMinPos-m_iPageSize+1);}
	void			ClampByViewRect		();
	void			SetPosScrollFromView(float view_pos, float view_width, float view_offs);
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
	virtual void	Init			(float x, float y, float length, bool bIsHorizontal);

	//���������, ������������ ������������� ����
//	typedef enum{VSCROLL, HSCROLL} E_MESSAGE;

	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void	OnMouse(float x, float y, EUIMessages mouse_action);

	virtual void	Draw			();

	virtual void	SetWidth		(float width);
	virtual void	SetHeight		(float height);

	virtual void	Reset			();
	// � ��������� ��������� ������ ���� �� ������� �������� ����� �����, � ���������
	// � ��� ����� ��� � ������� �� 0. ��� ������� - ����
	void			Refresh			();
	// ��������
	// �������� ���� ��� ������� ������ ��� ������ ����
	void			SetStepSize		(u32 step);
	// �������� �������� 
	void 			SetRange		(u32 iMin, u32 iMax);
	void 			GetRange		(u32& iMin, u32& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}
	u32 			GetMaxRange		() {return m_iMaxPos;}
	u32 			GetMinRange		() {return m_iMinPos;}
	// ������ �������� (������ �� �������� ������� �� (m_iMinPos .. m_iMaxPos-m_iPageSize) )
	void			SetPageSize		(u32 iPage) { m_iPageSize = _max(0,iPage); UpdateScrollBar();}
	u32				GetPageSize		() {return m_iPageSize;}
	// ��������� �������
	void			SetScrollPos	(u32 iPos) { SetScrollPosClamped(iPos); UpdateScrollBar();}
	int				GetScrollPos	() {return _max(m_iMinPos,m_iScrollPos);}
	// ������� ������� ��� ������
	void			TryScrollInc	();
	void			TryScrollDec	();
};


#endif