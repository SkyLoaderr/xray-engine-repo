// UIButton.cpp: класс нажимаемой кнопки
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIButton.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIButton:: CUIButton()
{
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;

	m_str = " ";

	m_bButtonClicked = false;
	m_bCursorOverButton = false;

	m_bAvailableTexture = false;

	SetTextAlign(CGameFont::alCenter);
}

 CUIButton::~ CUIButton()
{

}

void CUIButton::Reset()
{
	m_eButtonState = BUTTON_NORMAL;
	m_bButtonClicked = false;
	m_bCursorOverButton = false;

	inherited::Reset();
}


void CUIButton::Init(LPCSTR tex_name, int x, int y, int width, int height)
{

	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;
	m_bButtonClicked = false;
	m_bCursorOverButton = false;


	inherited::Init(tex_name, x, y, width, height);
	/*if(tex_name!=NULL)
	{
        m_UIStaticItem.Init(tex_name,"hud\\default",x,y,alNone);
		m_bAvailableTexture = true;
	}
	else
		m_bAvailableTexture = false;*/
}


void  CUIButton::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
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



	switch(m_ePressMode)
	{
	//////////////////////////
	//нормальный режим нажатия
	case NORMAL_PRESS:
		if(m_eButtonState == BUTTON_NORMAL)
		{
			if(mouse_action == LBUTTON_DOWN || mouse_action == LBUTTON_DB_CLICK)
			{
				m_eButtonState = BUTTON_PUSHED;
			
				GetParent()->SetCapture(this, true);
			}
		}
		else if(m_eButtonState == BUTTON_PUSHED)
		{
			if(mouse_action == LBUTTON_UP)
			{
				if(cursor_on_button)
				{
					GetParent()->SendMessage(this, BUTTON_CLICKED);
					m_bButtonClicked = true;
				}
			
				m_eButtonState = BUTTON_NORMAL;
			
				//освободить мышь
				GetParent()->SetCapture(this, false);
			}
			else if(mouse_action == MOUSE_MOVE)
			{
				if(!cursor_on_button)
					m_eButtonState = BUTTON_UP;
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
			}
		}
		break;
	////////////////////////////////
	//нужно только подвести и нажать
	case DOWN_PRESS:
        if(mouse_action == MOUSE_MOVE)
		{
			if(cursor_on_button)
			{
				m_eButtonState = BUTTON_PUSHED;
				//захватить мышь
				GetParent()->SetCapture(this, true);
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
				//освободить мышь
				GetParent()->SetCapture(this, false);

			}
		}
		else if(mouse_action == LBUTTON_DOWN || mouse_action == LBUTTON_DB_CLICK)
		{
			if(cursor_on_button)
			{
				GetParent()->SendMessage(this, BUTTON_CLICKED);
				m_bButtonClicked = true;
			}
		}

		break;
	///////////////////////////////////
	//нужно только подвести и отпустить
	case UP_PRESS:
        if(mouse_action == MOUSE_MOVE)
		{
			if(cursor_on_button)
			{
				m_eButtonState = BUTTON_PUSHED;
				//захватить мышь
				GetParent()->SetCapture(this, true);
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
				//освободить мышь
				GetParent()->SetCapture(this, false);
			}
		}
		else if(mouse_action == LBUTTON_UP)
		{
			if(cursor_on_button)
			{
				GetParent()->SendMessage(this, BUTTON_CLICKED);
				m_bButtonClicked = true;
			}	
		} 
		break;
	}

}


#define PUSH_OFFSET_RIGHT 2
#define PUSH_OFFSET_DOWN  3


//прорисовка кнопки
void  CUIButton::Draw()
{
	if(!m_bAvailableTexture) return;

	RECT rect = GetAbsoluteRect();
		

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
	{
		m_UIStaticItem.SetPos(rect.left, rect.top);
	}
	else
	{
		m_UIStaticItem.SetPos(rect.left+PUSH_OFFSET_RIGHT, 
								rect.top+PUSH_OFFSET_DOWN);
	}

	m_UIStaticItem.Render();
}






//#define TEXT_OFFSET_RIGHT 15
//#define TEXT_OFFSET_DOWN  10

#define TEXT_OFFSET_RIGHT (GetWidth()/2)
#define TEXT_OFFSET_DOWN  (GetHeight()/2 - GetFont()->CurrentHeight()/2)




void  CUIButton::Update()
{
	RECT rect = GetAbsoluteRect();

	int right_offset;
	int down_offset;

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL
		|| !m_bAvailableTexture)
	{
			right_offset = 0;
			down_offset = 0;
	}
	else
	{
			right_offset = PUSH_OFFSET_RIGHT;
			down_offset = PUSH_OFFSET_DOWN;
	}


	GetFont()->SetAligment(GetTextAlign());
	

	if(m_bCursorOverButton)
	{
			GetFont()->SetColor(0xFF999999);
			GetFont()->Out((float)rect.left + right_offset + 1 +TEXT_OFFSET_RIGHT, 
					   (float)rect.top + down_offset + 1  +TEXT_OFFSET_DOWN,
					    m_str);
			GetFont()->Out((float)rect.left + right_offset - 1 +TEXT_OFFSET_RIGHT, 
					   (float)rect.top + down_offset - 1 +TEXT_OFFSET_DOWN,
					    m_str);
			GetFont()->Out((float)rect.left + right_offset - 1 +TEXT_OFFSET_RIGHT, 
					   (float)rect.top + down_offset + 1 +TEXT_OFFSET_DOWN,
					    m_str);
			GetFont()->Out((float)rect.left + right_offset + 1 +TEXT_OFFSET_RIGHT, 
					   (float)rect.top + down_offset - 1 +TEXT_OFFSET_DOWN,
					    m_str);
			GetFont()->Out((float)rect.left + right_offset + 1 +TEXT_OFFSET_RIGHT, 
					   (float)rect.top + down_offset + 0 +TEXT_OFFSET_DOWN,
					    m_str);
			GetFont()->Out((float)rect.left + right_offset - 1 +TEXT_OFFSET_RIGHT, 
					   (float)rect.top + down_offset - 0 +TEXT_OFFSET_DOWN,
					    m_str);
			GetFont()->Out((float)rect.left + right_offset - 0 +TEXT_OFFSET_RIGHT, 
					   (float)rect.top + down_offset + 1 +TEXT_OFFSET_DOWN,
					    m_str);
			GetFont()->Out((float)rect.left + right_offset + 0 +TEXT_OFFSET_RIGHT,  
					   (float)rect.top + down_offset - 1 +TEXT_OFFSET_DOWN,
					    m_str);

	}

	GetFont()->SetColor(0xFFEEEEEE);

	
	GetFont()->Out((float)rect.left + right_offset  +  TEXT_OFFSET_RIGHT, 
				   (float)rect.top + down_offset  + TEXT_OFFSET_DOWN,
				    m_str);



}