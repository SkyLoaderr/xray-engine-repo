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

	m_bButtonClicked = false;
}

 CUIButton::~ CUIButton()
{

}



void CUIButton::Init(int x, int y, int width, int height)
{
	
	m_UIStaticNormal.Init("ui\\ui_hud_frame_l","hud\\default",x,y,alNone);
	m_UIStaticPushed.Init("ui\\ui_hud_frame_back","hud\\default",x,y,alNone);

	
	CUIWindow::Init(x, y, width, height);
}



void  CUIButton::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
		cursor_on_button = true;
	else
		cursor_on_button = false;


	m_bButtonClicked = false;



	switch(m_ePressMode)
	{
	//////////////////////////
	//нормальный режим нажатия
	case NORMAL_PRESS:
		if(m_eButtonState == BUTTON_NORMAL)
		{
			if(mouse_action == LBUTTON_DOWN)
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
		else if(mouse_action == LBUTTON_DOWN)
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

//прорисовка кнопки
void  CUIButton::Draw()
{
	RECT rect = GetAbsoluteRect();

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
	{
		m_UIStaticNormal.SetPos(rect.left, rect.top);
		m_UIStaticNormal.Render();
	}
	else
	{
		m_UIStaticPushed.SetPos(rect.left, rect.top);
		m_UIStaticPushed.Render();
	}
}

void  CUIButton::Update()
{
	RECT rect = GetAbsoluteRect();

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
	{
			m_pFont->SetColor(0xFF00FF00);
			
			m_pFont->Out((float)rect.left, (float)rect.top,	"normal");

			
			
	}
	else
	{
			m_pFont->SetColor(0xFF0000FF);
			m_pFont->Out((float)rect.left, (float)rect.top,	"pushed");
	}
}