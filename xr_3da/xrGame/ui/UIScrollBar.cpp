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
	m_iMinPos		= 1;
	m_iMaxPos		= 1;
	m_iPageSize		= 0;
	m_iStepSize		= 1;
	m_iScrollPos	= 0;
}

CUIScrollBar::~CUIScrollBar(void)
{
}

void CUIScrollBar::Init(int x, int y, int length, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;

	if(m_bIsHorizontal){
		CUIWindow::Init			(x,y, length, SCROLLBAR_HEIGHT);
		m_DecButton.Init		(SCROLLBAR_LEFT_ARROW, 0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init		(SCROLLBAR_RIGHT_ARROW,length-SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_ScrollBox.Init		(SCROLLBAR_WIDTH, 0, length/2, SCROLLBAR_HEIGHT, m_bIsHorizontal);
		m_StaticBackground.Init	(SCROLLBAR_BACKGROUND_HORZ ,"hud\\default", 0,0,alNone);
		m_ScrollWorkArea		= _max(0,GetWidth()-2*SCROLLBAR_WIDTH);
	}else{
		CUIWindow::Init			(x,y, SCROLLBAR_WIDTH, length);
		m_DecButton.Init		(SCROLLBAR_UP_ARROW,0, 0,SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init		(SCROLLBAR_DOWN_ARROW, 0, length-SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_ScrollBox.Init		(0, SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, length/2, m_bIsHorizontal);
		m_StaticBackground.Init	(SCROLLBAR_BACKGROUND_VERT ,"hud\\default", 0,0,alNone);
		m_ScrollWorkArea		= _max(0,GetHeight()-2*SCROLLBAR_HEIGHT);
	}

	if(!IsChild(&m_DecButton))	AttachChild(&m_DecButton);
	if(!IsChild(&m_IncButton))	AttachChild(&m_IncButton);
	if(!IsChild(&m_ScrollBox))	AttachChild(&m_ScrollBox);

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

void CUIScrollBar::SetStepSize(int step)
{
	m_iStepSize				= step;
	UpdateScrollBar			();
}

void CUIScrollBar::SetRange(int iMin, int iMax) 
{
	m_iMinPos				= iMin;  
	m_iMaxPos				= iMax;
	R_ASSERT				(iMax>=iMin);
	UpdateScrollBar			();
}

void CUIScrollBar::UpdateScrollBar()
{	
	Show						(!!(0!=ScrollSize()));
	if (IsShown){
		//уcтановить размер и положение каретки
		int box_sz				= iFloor(float(m_ScrollWorkArea)*float(m_iPageSize)/float(m_iMaxPos-m_iMinPos));
		if(m_bIsHorizontal){	
			// set width
			clamp					(box_sz,_min(int(SCROLLBAR_WIDTH),GetWidth()-2*SCROLLBAR_WIDTH),GetWidth()-2*SCROLLBAR_WIDTH);
			m_ScrollBox.SetWidth	(box_sz);
			// set pos
			int pos					= PosViewFromScroll(m_ScrollBox.GetWidth(),SCROLLBAR_WIDTH);
			m_ScrollBox.SetWndPos	(pos, m_ScrollBox.GetWndRect().top);
		}else{
			// set height
			clamp					(box_sz,_min(int(SCROLLBAR_HEIGHT),GetHeight()-2*SCROLLBAR_HEIGHT),GetHeight()-2*SCROLLBAR_HEIGHT);
			m_ScrollBox.SetHeight	(box_sz);
			// set pos
			int pos					= PosViewFromScroll(m_ScrollBox.GetHeight(),SCROLLBAR_HEIGHT);
			m_ScrollBox.SetWndPos	(m_ScrollBox.GetWndRect().left, pos);
		}
	}
}

void CUIScrollBar::OnMouse(int x, int y, EUIMessages mouse_action)
{
	switch(mouse_action){
		case WINDOW_MOUSE_WHEEL_DOWN:
			TryScrollInc();
			break;
		case WINDOW_MOUSE_WHEEL_UP:
			TryScrollDec();
			break;
	};
	inherited::OnMouse(x, y, mouse_action);
}


void CUIScrollBar::ClampByViewRect()
{
	if(m_bIsHorizontal){
		if(m_ScrollBox.GetWndRect().left<=SCROLLBAR_WIDTH)
			m_ScrollBox.SetWndPos	(SCROLLBAR_WIDTH, 
									m_ScrollBox.GetWndRect().top);
		else if(m_ScrollBox.GetWndRect().right>=GetWidth() - SCROLLBAR_WIDTH)
			m_ScrollBox.SetWndPos	(GetWidth() - SCROLLBAR_WIDTH - m_ScrollBox.GetWidth(), 
									m_ScrollBox.GetWndRect().top);
	}else{
		// limit vertical position (TOP) by position of button	
		if(m_ScrollBox.GetWndRect().top <= SCROLLBAR_HEIGHT)
			m_ScrollBox.SetWndPos	(m_ScrollBox.GetWndRect().left, 
									SCROLLBAR_HEIGHT);
		// limit vertical position (BOTTOM) by position of button
		else if(m_ScrollBox.GetWndRect().bottom >= GetHeight() - SCROLLBAR_HEIGHT)
			m_ScrollBox.SetWndPos	(m_ScrollBox.GetWndRect().left,
									GetHeight() - SCROLLBAR_HEIGHT - m_ScrollBox.GetHeight());
	}
}

void CUIScrollBar::SetPosScrollFromView(int view_pos, int view_size, int view_offs)
{
	int scroll_size	= ScrollSize();
	int pos			= view_pos-view_offs;
	int work_size	= m_ScrollWorkArea-view_size;
	SetScrollPosClamped	(work_size?iFloor(((float(pos)/float(work_size))*(scroll_size) + m_iMinPos)):0);
}

int CUIScrollBar::PosViewFromScroll(int view_size, int view_offs)
{
	int work_size	= m_ScrollWorkArea-view_size;
	int scroll_size	= ScrollSize();
	return			scroll_size?(m_iScrollPos*work_size+scroll_size*view_offs-m_iMinPos*work_size)/scroll_size:0;
}

void CUIScrollBar::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_DecButton){
		if(msg == BUTTON_CLICKED){
			TryScrollDec();
		}
	}else if(pWnd == &m_IncButton){
		if(msg == BUTTON_CLICKED){
			TryScrollInc();
		}
	}else if(pWnd == &m_ScrollBox){
		if(msg == SCROLLBOX_MOVE){
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal){
				//горизонтальный
				ClampByViewRect		();
				SetPosScrollFromView(m_ScrollBox.GetWndPos().x,m_ScrollBox.GetWidth(),SCROLLBAR_WIDTH);
				if (GetMessageTarget())
					GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
			}else{
				//вертикальный
				ClampByViewRect		();
				SetPosScrollFromView(m_ScrollBox.GetWndPos().y,m_ScrollBox.GetHeight(),SCROLLBAR_HEIGHT);
				if (GetMessageTarget())
					GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
			}
		}else if(msg == SCROLLBOX_STOP){
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal && GetMessageTarget())
				GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
			else
				GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
		}
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIScrollBar::TryScrollInc()
{
	if(ScrollInc())
		if(m_bIsHorizontal)
			GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
		else
			GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
}

void CUIScrollBar::TryScrollDec()
{
	if(ScrollDec())
		if(m_bIsHorizontal)
			GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
		else
			GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);

}


bool CUIScrollBar::ScrollDec()
{
	if(m_iScrollPos>m_iMinPos){
		SetScrollPos	(m_iScrollPos-m_iStepSize);
		return true;
	}

	return false;
}


bool CUIScrollBar::ScrollInc()
{
	if(m_iScrollPos<=(m_iMaxPos-m_iPageSize)){
		SetScrollPos	(m_iScrollPos+m_iStepSize);
		return true;
	}

	return false;	
}

void CUIScrollBar::Reset()
{
	ResetAll();
	inherited::Reset();
}


void CUIScrollBar::Draw()
{
	//нарисовать фоновую подложку
	if(m_bIsHorizontal){
		m_StaticBackground.SetTile(GetWidth()/4>0?GetWidth()/4:1, 1,
								   GetWidth()%2, 0);
	}else{
		m_StaticBackground.SetTile(1, GetHeight()/4>0?GetHeight()/4:1,
							   0, GetHeight()%2);
	}
	Irect rect = GetAbsoluteRect();
	m_StaticBackground.SetPos(rect.left,rect.top);
	m_StaticBackground.Render();

	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIScrollBar::Refresh()
{
	SendMessage(&m_ScrollBox, SCROLLBOX_MOVE, NULL);
}

