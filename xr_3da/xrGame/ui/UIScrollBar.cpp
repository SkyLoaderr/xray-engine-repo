//////////////////////////////////////////////////////////////////////
// UIScrollBar.cpp: полоса прокрутки для окон
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiscrollbar.h"
#include "UIButton.h"
#include "UIScrollBox.h"

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
	m_b_enabled		= true;
	m_DecButton		= xr_new<CUIButton>();				m_DecButton->SetAutoDelete(true);
	m_IncButton		= xr_new<CUIButton>();				m_IncButton->SetAutoDelete(true);
	m_ScrollBox		= xr_new<CUIScrollBox>();			m_ScrollBox->SetAutoDelete(true);
	m_StaticBackground = xr_new<CUIStaticItem>();
}

CUIScrollBar::~CUIScrollBar(void)
{
	xr_delete(m_StaticBackground);
}

void CUIScrollBar::Init(float x, float y, float length, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;
	if(m_bIsHorizontal){
		CUIWindow::Init			(x,y, length, SCROLLBAR_HEIGHT);
		m_DecButton->Init		(SCROLLBAR_LEFT_ARROW, 0.0f, 0.0f, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton->Init		(SCROLLBAR_RIGHT_ARROW,length-SCROLLBAR_WIDTH, 0.0f, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_ScrollBox->Init		(SCROLLBAR_WIDTH, 0.0f, length/2, SCROLLBAR_HEIGHT, m_bIsHorizontal);
		m_StaticBackground->Init(SCROLLBAR_BACKGROUND_HORZ ,"hud\\default", 0.0f, 0.0f,alNone);
		m_ScrollWorkArea		= _max(0,iFloor(GetWidth()-2*SCROLLBAR_WIDTH));
	}else{
		CUIWindow::Init			(x,y, SCROLLBAR_WIDTH, length);
		m_DecButton->Init		(SCROLLBAR_UP_ARROW,0.0f, 0.0f ,SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton->Init		(SCROLLBAR_DOWN_ARROW, 0.0f, length-SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_ScrollBox->Init		(0, SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, length/2, m_bIsHorizontal);
		m_StaticBackground->Init(SCROLLBAR_BACKGROUND_VERT ,"hud\\default", 0.0f, 0.0f ,alNone);
		m_ScrollWorkArea		= _max(0,iFloor(GetHeight()-2*SCROLLBAR_HEIGHT));
	}

	if(!IsChild(m_DecButton))	AttachChild(m_DecButton);
	if(!IsChild(m_IncButton))	AttachChild(m_IncButton);
	if(!IsChild(m_ScrollBox))	AttachChild(m_ScrollBox);

	UpdateScrollBar();
}


//корректировка размеров скроллера
void CUIScrollBar::SetWidth(float width)
{
	if(width<=0.0f) width = 1.0f;
	inherited::SetWidth(width);
}

void CUIScrollBar::SetHeight(float height)
{
	if(height<=0.0f) height = 1.0f;
	inherited::SetHeight(height);
	UpdateScrollBar();
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
void CUIScrollBar::Show(bool b)
{
	if(!m_b_enabled)return;
	inherited::Show(b);
}

void CUIScrollBar::Enable(bool b)
{
	if(!m_b_enabled)return;
	inherited::Enable(b);
}

void CUIScrollBar::UpdateScrollBar()
{
	// next line disabled because of problems with appearance after size changing in listwnd
	//	Show						( !!(0!=ScrollSize()) );

	if (IsShown()){
		//уcтановить размер и положение каретки
		float box_sz				= float(m_ScrollWorkArea)*float(m_iPageSize ? m_iPageSize : 1)/float(m_iMaxPos-m_iMinPos);
		if(m_bIsHorizontal){	
			// set width
			clamp					(box_sz,_min(SCROLLBAR_WIDTH,GetWidth()-2*SCROLLBAR_WIDTH),GetWidth()-2*SCROLLBAR_WIDTH);
			m_ScrollBox->SetWidth	(box_sz);
			// set pos
			int pos					= PosViewFromScroll(iFloor(m_ScrollBox->GetWidth()),iFloor(SCROLLBAR_WIDTH));
			m_ScrollBox->SetWndPos	(float(pos), m_ScrollBox->GetWndRect().top);
			m_IncButton->SetWndPos	(GetWidth() -SCROLLBAR_WIDTH, 0.0f);
		}else{
			// set height
			clamp					(box_sz,_min(SCROLLBAR_HEIGHT,GetHeight()-2*SCROLLBAR_HEIGHT),GetHeight()-2*SCROLLBAR_HEIGHT);
			m_ScrollBox->SetHeight	(box_sz);
			// set pos
			int pos				= PosViewFromScroll(iFloor(m_ScrollBox->GetHeight()),iFloor(SCROLLBAR_HEIGHT));
			m_ScrollBox->SetWndPos	(m_ScrollBox->GetWndRect().left, float(pos));
			m_IncButton->SetWndPos	(0.0f, GetHeight() -SCROLLBAR_HEIGHT);
		}
	}
}

bool CUIScrollBar::OnMouse(float x, float y, EUIMessages mouse_action)
{
	switch(mouse_action){
		case WINDOW_MOUSE_WHEEL_DOWN:
			TryScrollInc();
			return true;
			break;
		case WINDOW_MOUSE_WHEEL_UP:
			TryScrollDec();
			return true;
			break;
	};
	return inherited::OnMouse(x, y, mouse_action);
}


void CUIScrollBar::ClampByViewRect()
{
	if(m_bIsHorizontal){
		if(m_ScrollBox->GetWndRect().left<=SCROLLBAR_WIDTH)
			m_ScrollBox->SetWndPos	(SCROLLBAR_WIDTH, 
									m_ScrollBox->GetWndRect().top);
		else if(m_ScrollBox->GetWndRect().right>=GetWidth() - SCROLLBAR_WIDTH)
			m_ScrollBox->SetWndPos	(GetWidth() - SCROLLBAR_WIDTH - m_ScrollBox->GetWidth(), 
									m_ScrollBox->GetWndRect().top);
	}else{
		// limit vertical position (TOP) by position of button	
		if(m_ScrollBox->GetWndRect().top <= SCROLLBAR_HEIGHT)
			m_ScrollBox->SetWndPos	(m_ScrollBox->GetWndRect().left, 
									SCROLLBAR_HEIGHT);
		// limit vertical position (BOTTOM) by position of button
		else if(m_ScrollBox->GetWndRect().bottom >= GetHeight() - SCROLLBAR_HEIGHT)
			m_ScrollBox->SetWndPos	(m_ScrollBox->GetWndRect().left,
									GetHeight() - SCROLLBAR_HEIGHT - m_ScrollBox->GetHeight());
	}
}

void CUIScrollBar::SetPosScrollFromView(float view_pos, float view_size, float view_offs)
{
	int scroll_size	= ScrollSize();
	float pos			= view_pos-view_offs;
	float work_size	= m_ScrollWorkArea-view_size;
	SetScrollPosClamped	(work_size?iFloor(((pos/work_size)*(scroll_size) + m_iMinPos)):0);
}

int CUIScrollBar::PosViewFromScroll(int view_size, int view_offs)
{
	int work_size	= m_ScrollWorkArea-view_size;
	int scroll_size	= ScrollSize();
	return			scroll_size?(m_iScrollPos*work_size+scroll_size*view_offs-m_iMinPos*work_size)/scroll_size:0;
}

void CUIScrollBar::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == m_DecButton){
		if(msg == BUTTON_CLICKED){
			TryScrollDec();
		}
	}else if(pWnd == m_IncButton){
		if(msg == BUTTON_CLICKED){
			TryScrollInc();
		}
	}else if(pWnd == m_ScrollBox){
		if(msg == SCROLLBOX_MOVE){
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal){
				//горизонтальный
				ClampByViewRect		();
				SetPosScrollFromView(m_ScrollBox->GetWndPos().x,m_ScrollBox->GetWidth(),SCROLLBAR_WIDTH);
				if (GetMessageTarget())
					GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
			}else{
				//вертикальный
				ClampByViewRect		();
				SetPosScrollFromView(m_ScrollBox->GetWndPos().y,m_ScrollBox->GetHeight(),SCROLLBAR_HEIGHT);
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
		if(m_iScrollPos>m_iStepSize)
			SetScrollPos	(m_iScrollPos-m_iStepSize);
		else
			SetScrollPos	(0);

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
		if (m_StaticBackground->GetOriginalRect().width()){
			int tile		= iFloor(GetWidth()/m_StaticBackground->GetOriginalRect().width());
			float rem			= GetWidth()-tile*m_StaticBackground->GetOriginalRect().width();
			m_StaticBackground->SetTile(tile,1,rem,0.0f);
		}
	}else{
		if (m_StaticBackground->GetOriginalRect().height()){
			int tile		= iFloor(GetHeight()/m_StaticBackground->GetOriginalRect().height());
			float rem			= GetHeight()-tile*m_StaticBackground->GetOriginalRect().height();
			m_StaticBackground->SetTile(1,tile,0.0f,rem);
		}
	}
	Frect rect = GetAbsoluteRect();
	m_StaticBackground->SetPos(rect.left,rect.top);
	m_StaticBackground->Render();

	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIScrollBar::Refresh()
{
	SendMessage(m_ScrollBox, SCROLLBOX_MOVE, NULL);
}

