//////////////////////////////////////////////////////////////////////
// UIScrollBar.cpp: полоса прокрутки для окон
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiscrollbar.h"

CUIScrollBar::CUIScrollBar(void)
{
	m_iMinPos = 1;
	m_iMaxPos = 1;
	m_iPageSize = 1;

}

CUIScrollBar::~CUIScrollBar(void)
{
}

void CUIScrollBar::Init(int x, int y, int length, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;


	if(m_bIsHorizontal)
	{
		CUIWindow::Init(x,y, length, SCROLLBAR_HEIGHT);
		m_DecButton.Init("ui\\ui_scb_left_arrow", 0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init("ui\\ui_scb_rigth_arrow",length-SCROLLBAR_WIDTH, 0,
													SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);

		m_ScrollBox.Init(SCROLLBAR_WIDTH, 0, length/2, SCROLLBAR_HEIGHT, m_bIsHorizontal);
	}
	else
	{
		CUIWindow::Init(x,y, SCROLLBAR_WIDTH, length);
		m_DecButton.Init("ui\\ui_scb_rigth_arrow",0, 0,	SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init("ui\\ui_scb_left_arrow", 0, length-SCROLLBAR_HEIGHT, 
						  SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		

		m_ScrollBox.Init(0, SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, length/2, m_bIsHorizontal);
	}

	
	

	AttachChild(&m_DecButton);
	AttachChild(&m_IncButton);
	AttachChild(&m_ScrollBox);
	
	UpdateScrollBar();
}


//корректировка размеров скроллера
void CUIScrollBar::SetWidth(int width)
{
	if(width<=0) width = 1;
	inherited::SetWidth(width);
}
void CUIScrollBar::SetHeight(int height)
{
	if(height<=0) height = 1;
	inherited::SetHeight(height);
}

void CUIScrollBar::UpdateScrollBar()
{
	//относительный размер единичного прокручемого элемента
	float scrollbar_unit;
	scrollbar_unit = (float)1/(m_iMaxPos-m_iMinPos+1);

	//утановить размер и положение каретки
	if(m_bIsHorizontal)
	{	
		m_ScrollBox.SetWidth((int)((GetWidth()-2*SCROLLBAR_WIDTH)*
									scrollbar_unit*m_iPageSize));
		
		m_ScrollBox.MoveWindow((int)(SCROLLBAR_WIDTH + 	
									(GetWidth()-2*SCROLLBAR_WIDTH)
									*scrollbar_unit*(m_iScrollPos-m_iMinPos)),  
							          m_ScrollBox.GetWndRect().top);
		if(m_iScrollPos == m_iMaxPos - m_iPageSize + 1)
		{
			m_ScrollBox.MoveWindow(GetWidth() - SCROLLBAR_WIDTH - 
											m_ScrollBox.GetWidth(), 
											m_ScrollBox.GetWndRect().top);
		}
	}
	else
	{
		m_ScrollBox.SetHeight((int)((GetHeight()-2*SCROLLBAR_HEIGHT)*
									scrollbar_unit*m_iPageSize));
		
		m_ScrollBox.MoveWindow(m_ScrollBox.GetWndRect().left,
								(int)( SCROLLBAR_HEIGHT+
								(GetHeight()-2*SCROLLBAR_HEIGHT)*
								scrollbar_unit*(m_iScrollPos-m_iMinPos)));

		//в крайней позиции подправить положение scrollbox
		if(m_iScrollPos == m_iMaxPos - m_iPageSize + 1)
		{
			m_ScrollBox.MoveWindow(m_ScrollBox.GetWndRect().left,
									GetHeight() - SCROLLBAR_HEIGHT - 
									m_ScrollBox.GetHeight());
		}
	}
}


void CUIScrollBar::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_DecButton)
	{
		if(msg == CUIButton::BUTTON_CLICKED)
		{
			if(ScrollDec())
				if(m_bIsHorizontal)
					GetParent()->SendMessage(this, HSCROLL);
				else
					GetParent()->SendMessage(this, VSCROLL);
		}
	}
	else if(pWnd == &m_IncButton)
	{
		if(msg == CUIButton::BUTTON_CLICKED)
		{
			if(ScrollInc())
				if(m_bIsHorizontal)
					GetParent()->SendMessage(this, HSCROLL);
				else
					GetParent()->SendMessage(this, VSCROLL);
		}
	}	
	else if(pWnd == &m_ScrollBox)
	{
		if(msg == CUIScrollBox::SCROLLBOX_MOVE)
		{
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal)
			//горизонтальный
			{
				if(m_ScrollBox.GetWndRect().left<SCROLLBAR_WIDTH)
					m_ScrollBox.MoveWindow(SCROLLBAR_WIDTH, m_ScrollBox.GetWndRect().top);
				else if(m_ScrollBox.GetWndRect().right>GetWidth() - SCROLLBAR_WIDTH)
					m_ScrollBox.MoveWindow(GetWidth() - SCROLLBAR_WIDTH - 
											m_ScrollBox.GetWidth(), 
											m_ScrollBox.GetWndRect().top);


				m_iScrollPos = (s16)((s16)m_ScrollBox.GetWndRect().left - SCROLLBAR_WIDTH)*
							(m_iMaxPos-m_iMinPos+1)/
							((s16)GetWidth() - (s16)2*SCROLLBAR_WIDTH) + m_iMinPos;

				if(m_iScrollPos+m_iPageSize>m_iMaxPos) 
					m_iScrollPos = m_iMaxPos - m_iPageSize +1;
				if(m_iScrollPos<m_iMinPos) 
					m_iScrollPos = m_iMinPos;


				GetParent()->SendMessage(this, HSCROLL);
			}
			//вертикальный
			else
			{
				if(m_ScrollBox.GetWndRect().top<SCROLLBAR_HEIGHT)
					m_ScrollBox.MoveWindow(m_ScrollBox.GetWndRect().left, SCROLLBAR_HEIGHT);
				else if(m_ScrollBox.GetWndRect().bottom>GetHeight() - SCROLLBAR_HEIGHT)
					m_ScrollBox.MoveWindow(m_ScrollBox.GetWndRect().left,
											GetHeight() - SCROLLBAR_HEIGHT - 
											m_ScrollBox.GetHeight());


				m_iScrollPos = (s16)(m_ScrollBox.GetWndRect().top - SCROLLBAR_HEIGHT)*
							(m_iMaxPos-m_iMinPos+1)/
							((s16)GetHeight() - 2*SCROLLBAR_HEIGHT) + m_iMinPos;

				if(m_iScrollPos+m_iPageSize>m_iMaxPos)
							m_iScrollPos = m_iMaxPos - m_iPageSize + 1;
				if(m_iScrollPos<m_iMinPos) m_iScrollPos = m_iMinPos;

				GetParent()->SendMessage(this, VSCROLL);
			}
			
		}
		else if(msg == CUIScrollBox::SCROLLBOX_STOP)
		{
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal)
			{
				m_iScrollPos = (s16)(m_ScrollBox.GetWndRect().left - SCROLLBAR_WIDTH)*
							(m_iMaxPos-m_iMinPos+1)/
							((s16)GetWidth() - 2*SCROLLBAR_WIDTH) + m_iMinPos;

				if(m_iScrollPos+m_iPageSize>m_iMaxPos) m_iScrollPos = m_iMaxPos - m_iPageSize +1;
				if(m_iScrollPos<m_iMinPos) m_iScrollPos = m_iMinPos;
			}
			else
			{
				m_iScrollPos = (s16)(m_ScrollBox.GetWndRect().top - SCROLLBAR_HEIGHT)*
							(m_iMaxPos-m_iMinPos+1)/
							((s16)GetHeight() - 2*SCROLLBAR_HEIGHT) + m_iMinPos;

				if(m_iScrollPos+m_iPageSize>m_iMaxPos) 
					m_iScrollPos = m_iMaxPos - m_iPageSize+1;
				if(m_iScrollPos<m_iMinPos) m_iScrollPos = m_iMinPos;
			}

			UpdateScrollBar();

			if(m_bIsHorizontal)
				GetParent()->SendMessage(this, HSCROLL);
			else
				GetParent()->SendMessage(this, VSCROLL);
			//*/
		}

	}


	CUIWindow::SendMessage(pWnd, msg, pData);
}


bool CUIScrollBar::ScrollDec()
{
	if(m_iScrollPos>m_iMinPos)
	{
		m_iScrollPos--;
		UpdateScrollBar();
		return true;
	}

	return false;
}


bool CUIScrollBar::ScrollInc()
{
	if(m_iScrollPos+m_iPageSize<=m_iMaxPos)
	{
		m_iScrollPos++;
		UpdateScrollBar();
		return true;
	}

	return false;	
}

void CUIScrollBar::SetRange(s16 iMin, s16 iMax) 
{
	m_iMinPos = iMin;  
	m_iMaxPos = iMax;

	R_ASSERT(iMax>=iMin);
	
	UpdateScrollBar();
}

void CUIScrollBar::Reset()
{
	ResetAll();

	inherited::Reset();
}
