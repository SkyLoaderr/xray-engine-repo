//////////////////////////////////////////////////////////////////////
// UIScrollBar.cpp: полоса прокрутки для окон
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiscrollbar.h"

#define SCROLLBAR_LEFT_ARROW		"ui\\ui_scb_left_arrow"
#define SCROLLBAR_RIGHT_ARROW		"ui\\ui_scb_right_arrow"
#define SCROLLBAR_UP_ARROW			"ui\\ui_scb_up_arrow"
#define SCROLLBAR_DOWN_ARROW		"ui\\ui_scb_down_arrow"
#define SCROLLBAR_BACKGROUND_HORZ	"ui\\ui_scb_back"
#define SCROLLBAR_BACKGROUND_VERT	"ui\\ui_scb_back_v"

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
		m_DecButton.Init(SCROLLBAR_LEFT_ARROW, 0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init(SCROLLBAR_RIGHT_ARROW,length-SCROLLBAR_WIDTH, 0,
													SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);

		m_ScrollBox.Init(SCROLLBAR_WIDTH, 0, length/2, SCROLLBAR_HEIGHT, m_bIsHorizontal);

		m_StaticBackground.Init(SCROLLBAR_BACKGROUND_HORZ ,"hud\\default", 0,0,alNone);
	}
	else
	{
		CUIWindow::Init(x,y, SCROLLBAR_WIDTH, length);
		m_DecButton.Init(SCROLLBAR_UP_ARROW,0, 0,SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init(SCROLLBAR_DOWN_ARROW, 0, length-SCROLLBAR_HEIGHT, 
						  SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		

		m_ScrollBox.Init(0, SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, length/2, m_bIsHorizontal);

		m_StaticBackground.Init(SCROLLBAR_BACKGROUND_VERT ,"hud\\default", 0,0,alNone);
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
	//относительный размер единичного прокручиваемого элемента
	float scrollbar_unit;
	scrollbar_unit = (float)1/(m_iMaxPos-m_iMinPos+1);

	//утановить размер и положение каретки
	if(m_bIsHorizontal)
	{	
		m_ScrollBox.SetWidth(iFloor(0.5f + 
									(float)(GetWidth()-2*SCROLLBAR_WIDTH)*
									scrollbar_unit*m_iPageSize));
		
		m_ScrollBox.MoveWindow((int)(SCROLLBAR_WIDTH + 	
									(GetWidth()-2*SCROLLBAR_WIDTH)
									*scrollbar_unit*(m_iScrollPos-m_iMinPos)),  
							          m_ScrollBox.GetWndRect().top);
		
		//в крайней позиции подправить положение scrollbox
		if(m_iScrollPos == m_iMaxPos - m_iPageSize + 1)
		{
			m_ScrollBox.SetWidth(m_IncButton.GetWndRect().left-m_ScrollBox.GetWndRect().left);
		}
	}
	else
	{
		int height = iCeil(float((GetHeight()-2*SCROLLBAR_HEIGHT))*
						   scrollbar_unit*float(m_iPageSize)
						   +.5f);

		m_ScrollBox.SetHeight(height);

		m_ScrollBox.MoveWindow(m_ScrollBox.GetWndRect().left,
								iFloor( SCROLLBAR_HEIGHT+
								(GetHeight()-2*SCROLLBAR_HEIGHT)*
								scrollbar_unit*(m_iScrollPos-m_iMinPos)));

		//в крайней позиции подправить положение scrollbox
		if(m_iScrollPos == m_iMaxPos - m_iPageSize + 1)
		{
			m_ScrollBox.SetHeight(m_IncButton.GetWndRect().top-m_ScrollBox.GetWndRect().top);
		}
	}
}


void CUIScrollBar::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_DecButton)
	{
		if(msg == BUTTON_CLICKED)
		{
			if(ScrollDec())
				if(m_bIsHorizontal)
					GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
				else
					GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
		}
	}
	else if(pWnd == &m_IncButton)
	{
		if(msg == BUTTON_CLICKED)
		{
			if(ScrollInc())
				if(m_bIsHorizontal)
					GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
				else
					GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
		}
	}	
	else if(pWnd == &m_ScrollBox)
	{
		if(msg == SCROLLBOX_MOVE)
		{
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal)
			//горизонтальный
			{
				if(m_ScrollBox.GetWndRect().left<=SCROLLBAR_WIDTH)
					m_ScrollBox.MoveWindow(SCROLLBAR_WIDTH, m_ScrollBox.GetWndRect().top);
				else if(m_ScrollBox.GetWndRect().right>=GetWidth() - SCROLLBAR_WIDTH)
					m_ScrollBox.MoveWindow(GetWidth() - SCROLLBAR_WIDTH - 
											m_ScrollBox.GetWidth(), 
											m_ScrollBox.GetWndRect().top);


				m_iScrollPos = (s16)iFloor((float)((s16)m_ScrollBox.GetWndRect().left - SCROLLBAR_WIDTH)*
							(float)(m_iMaxPos-m_iMinPos+1)/
							(float)((s16)GetWidth() - (s16)2*SCROLLBAR_WIDTH) + m_iMinPos);

				if(m_iScrollPos+m_iPageSize>m_iMaxPos) 
					m_iScrollPos = m_iMaxPos - m_iPageSize +1;
				if(m_iScrollPos<m_iMinPos) 
					m_iScrollPos = m_iMinPos;


				if (GetMessageTarget())
					GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
			}
			//вертикальный
			else
			{
				if(m_ScrollBox.GetWndRect().top<=SCROLLBAR_HEIGHT)
					m_ScrollBox.MoveWindow(m_ScrollBox.GetWndRect().left, SCROLLBAR_HEIGHT);
				else if(m_ScrollBox.GetWndRect().bottom>=GetHeight() - SCROLLBAR_HEIGHT)
					m_ScrollBox.MoveWindow(m_ScrollBox.GetWndRect().left,
											GetHeight() - SCROLLBAR_HEIGHT - 
											m_ScrollBox.GetHeight());


				m_iScrollPos = (s16)iFloor(0.5f +
					        float(m_ScrollBox.GetWndRect().top - SCROLLBAR_HEIGHT)*
							float(m_iMaxPos-m_iMinPos+1)/
							float((s16)GetHeight() - 2*SCROLLBAR_HEIGHT) + m_iMinPos);

				if(m_iScrollPos+m_iPageSize>=m_iMaxPos)
							m_iScrollPos = m_iMaxPos - m_iPageSize + 1;
				if(m_iScrollPos<m_iMinPos) m_iScrollPos = m_iMinPos;

				if (GetMessageTarget())
					GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
			}
			
		}
		else if(msg == SCROLLBOX_STOP)
		{
			//вычислить новое положение прокрутки
//			UpdateScrollBar();

			if(m_bIsHorizontal && GetMessageTarget())
				GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
			else
				GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
		}
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}


bool CUIScrollBar::ScrollDec()
{
	if(m_iScrollPos>m_iMinPos)
	{
		--m_iScrollPos;
		UpdateScrollBar();
		return true;
	}

	return false;
}


bool CUIScrollBar::ScrollInc()
{
	if(m_iScrollPos+m_iPageSize<=m_iMaxPos)
	{
		++m_iScrollPos;
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


void CUIScrollBar::Draw()
{

	//нарисовать фоновую подложку
	if(m_bIsHorizontal)
	{
		m_StaticBackground.SetTile(GetWidth()/4>0?GetWidth()/4:1, 1,
								   GetWidth()%2, 0);
	}
	else
	{
		m_StaticBackground.SetTile(1, GetHeight()/4>0?GetHeight()/4:1,
							   0, GetHeight()%2);
	}
	RECT rect = GetAbsoluteRect();
	m_StaticBackground.SetPos(rect.left,rect.top);
	m_StaticBackground.Render();

	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIScrollBar::Refresh()
{
	SendMessage(&m_ScrollBox, SCROLLBOX_MOVE, NULL);
}

