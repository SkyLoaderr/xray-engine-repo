//////////////////////////////////////////////////////////////////////
// UIScrollBox.h: класс перемещаемой каретки в CScrollBar
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiscrollbox.h"



#define SCROLLBOX_HORZ	"ui\\ui_scb_scroll_box"
#define SCROLLBOX_VERT	"ui\\ui_scb_scroll_box_v"




CUIScrollBox::CUIScrollBox(void)
{
	m_bAvailableTexture = true;
}

CUIScrollBox::~CUIScrollBox(void)
{
}


void CUIScrollBox::Init(int x, int y, int length, int broad, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;

	if(m_bIsHorizontal)
	{
		m_UIStaticItem.Init(SCROLLBOX_HORZ, "hud\\default",x,y, alNone);
		CUIWindow::Init(x,y, length, broad);
	}
	else
	{
		m_UIStaticItem.Init(SCROLLBOX_VERT, "hud\\default",x,y, alNone);
		CUIWindow::Init(x,y, broad, length);
	}

	SetPushOffsetX(0);
	SetPushOffsetY(0);
}

//////////////////////////////////////////////////////////////////////////

void CUIScrollBox::OnMouse(int x, int y, EUIMessages mouse_action)
{
	int deltaX = 0;
	int deltaY = 0;

	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
		cursor_on_button = true;
	else
		cursor_on_button = false;


	if(m_eButtonState == BUTTON_NORMAL)
	{
		if(mouse_action == WINDOW_LBUTTON_DOWN)
		{
			m_eButtonState = BUTTON_PUSHED;
		
			GetParent()->SetCapture(this, true);
		}
	}
	else if(m_eButtonState == BUTTON_PUSHED)
	{
		if(mouse_action == WINDOW_LBUTTON_UP)
		{
			if(cursor_on_button)
			{
				GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
				m_bButtonClicked = true;
			}
		
			m_eButtonState = BUTTON_NORMAL;
		
			//освободить мышь
			GetParent()->SetCapture(this, false);

			GetMessageTarget()->SendMessage(this, SCROLLBOX_STOP);
		}
		else if(mouse_action == WINDOW_MOUSE_MOVE)
		{
//			if(!cursor_on_button)
//				m_eButtonState = BUTTON_UP;
//			else
			{
				deltaX = x - m_iOldMouseX;
				deltaY = y - m_iOldMouseY;

				if(m_bIsHorizontal)
				{
					MoveWindow(GetWndRect().left+deltaX, GetWndRect().top);
				}
				else
					MoveWindow(GetWndRect().left, GetWndRect().top+deltaY);

				GetMessageTarget()->SendMessage(this, SCROLLBOX_MOVE);
			}
		}
	}
	else if(m_eButtonState == BUTTON_UP)
	{
		if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			if(cursor_on_button)
				m_eButtonState = BUTTON_PUSHED;
		}
		else if(mouse_action == WINDOW_LBUTTON_UP)
		{
			m_eButtonState = BUTTON_NORMAL;
			GetParent()->SetCapture(this, false);
		}
	}


	//запомнить положение мыши, с учетом
	//возможного перемещения каретки
	m_iOldMouseX = x - deltaX;
	m_iOldMouseY = y - deltaY;
}

void CUIScrollBox::Draw()
{
	if(m_bIsHorizontal)
	{
		m_UIStaticItem.SetTile(GetWidth()/4>0?GetWidth()/4:1, 1,
							  GetWidth()%2, 0);
	}
	else
	{
		m_UIStaticItem.SetTile(1, GetHeight()/4>0?GetHeight()/4:1,
							   0, GetHeight()%2);
	}

	 CUIButton::Draw();
}