// UIDragDropItem.h: класс перетаскиваемой картинки
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIDragDropItem.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIDragDropItem:: CUIDragDropItem()
{
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;

	m_str = " ";

	m_bButtonClicked = false;
	m_bCursorOverButton = false;


	m_bDDEnabled = true;


	m_iGridWidth = 0;
	m_iGridHeight = 0;

	m_iGridCol = 0;
	m_iGridRow = 0;
}

CUIDragDropItem::~ CUIDragDropItem()
{

}



void CUIDragDropItem::Init(LPCSTR tex_name, int x, int y, int width, int height)
{
	m_pParentWnd = NULL;
	m_pMouseCapturer = NULL;



	m_iOldMouseX = x;
	m_iOldMouseY = y;

	m_previousPos.x = x;
	m_previousPos.y = y;

	CUIButton::Init(tex_name, x, y, width, height);
}

void  CUIDragDropItem::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{

	int deltaX = 0;
	int deltaY = 0;


	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
	{
		cursor_on_button = true;
	}
	else
	{
		cursor_on_button = false;
	}

	
	m_bCursorOverButton = cursor_on_button;


	m_bButtonClicked = false;


	if(mouse_action == MOUSE_MOVE && m_eButtonState == BUTTON_NORMAL)
			GetParent()->SetCapture(this, cursor_on_button);

	if(mouse_action == LBUTTON_DB_CLICK && m_bCursorOverButton)
	{
		GetTop()->SendMessage(this, ITEM_DB_CLICK);
		GetParent()->SetCapture(this, false);
	}



	if(m_eButtonState == BUTTON_NORMAL)
	{
		if(mouse_action == LBUTTON_DOWN)
		{
			m_eButtonState = BUTTON_PUSHED;
			
			//GetParent()->SendMessage(this, ITEM_DRAG);
			GetTop()->SendMessage(this, ITEM_DRAG);
			GetParent()->SetCapture(this, true);


			m_previousPos.x = GetWndRect().left;
			m_previousPos.y = GetWndRect().top;
		}
	}
	else if(m_eButtonState == BUTTON_PUSHED)
	{
		if(mouse_action == LBUTTON_UP)
		{
			if(cursor_on_button)
			{
				//GetParent()->SendMessage(this, ITEM_DROP);
				GetTop()->SendMessage(this, ITEM_DROP);
				
				m_bButtonClicked = true;

				//////
				//MoveWindow(m_previousPos.x,	m_previousPos.y);
			}
		
			m_eButtonState = BUTTON_NORMAL;
			
			//освободить мышь
			GetParent()->SetCapture(this, false);
		}
		else if(mouse_action == MOUSE_MOVE)
		{
		//	if(!cursor_on_button)
		//		m_eButtonState = BUTTON_UP;
			
			//только если включен режим	drag drop
//			else 
			if(m_bDDEnabled) 
			{
				deltaX = x - m_iOldMouseX;
				deltaY = y - m_iOldMouseY;

				
				MoveWindow(GetWndRect().left+deltaX,  GetWndRect().top+deltaY);

				//GetParent()->SendMessage(this, ITEM_MOVE);
				GetTop()->SendMessage(this, ITEM_MOVE);
			}
		}
	}
	else if(m_eButtonState == BUTTON_UP)
	{
		if(mouse_action == MOUSE_MOVE)
		{
			if(cursor_on_button)
				m_eButtonState = BUTTON_PUSHED;
		}
		else if(mouse_action == LBUTTON_UP)
		{
			m_eButtonState = BUTTON_NORMAL;
			GetParent()->SetCapture(this, false);


			GetTop()->SendMessage(this, ITEM_DROP);
			
			/////
			//MoveWindow(m_previousPos.x,	m_previousPos.y);
		}
	}


	//запомнить положение мыши, с учетом
	//возможного перемещения картинки
	m_iOldMouseX = x - deltaX;
	m_iOldMouseY = y - deltaY;

}



void CUIDragDropItem::Draw()
{
	RECT rect = GetAbsoluteRect();

	//отцентрировать текстуру по центру ее окна
	int right_offset = (GetWidth()-m_UIStaticItem.GetRect().width())/2;
	int down_offset = (GetHeight()-m_UIStaticItem.GetRect().height())/2;

	m_UIStaticItem.SetPos(rect.left + right_offset, rect.top + down_offset);
	m_UIStaticItem.Render();
}

void CUIDragDropItem::Update()
{
	RECT rect = GetAbsoluteRect();

	GetFont()->SetColor(0xFFEEEEEE);
	GetFont()->Out((float)rect.left,(float)rect.top, m_str);
}
