// UIButton.cpp: ����� ���������� ������
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

	m_str = "Hello";

	m_bButtonClicked = false;
	m_bCursorOverButton = false;
}

 CUIButton::~ CUIButton()
{

}



void CUIButton::Init(LPCSTR tex_name, int x, int y, int width, int height)
{
	
	m_UIStaticItem.Init(tex_name,"hud\\default",x,y,alNone);

	
	CUIWindow::Init(x, y, width, height);
}


void  CUIButton::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	//��������� �������� �� ������ �� ������
	//���������� ������ ������������ ����� ������
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
		cursor_on_button = true;
	else
		cursor_on_button = false;

	m_bCursorOverButton = cursor_on_button;


	m_bButtonClicked = false;



	switch(m_ePressMode)
	{
	//////////////////////////
	//���������� ����� �������
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
			
				//���������� ����
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
	//����� ������ �������� � ������
	case DOWN_PRESS:
        if(mouse_action == MOUSE_MOVE)
		{
			if(cursor_on_button)
			{
				m_eButtonState = BUTTON_PUSHED;
				//��������� ����
				GetParent()->SetCapture(this, true);
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
				//���������� ����
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
	//����� ������ �������� � ���������
	case UP_PRESS:
        if(mouse_action == MOUSE_MOVE)
		{
			if(cursor_on_button)
			{
				m_eButtonState = BUTTON_PUSHED;
				//��������� ����
				GetParent()->SetCapture(this, true);
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
				//���������� ����
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


//���������� ������
void  CUIButton::Draw()
{
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

	//���������� ������ ������ � ������������ � �������� ������������� ��������
	/*SetWndRect(GetWndRect().left + m_UIStaticNormal.GetRect().x1,
			   GetWndRect().top  + m_UIStaticNormal.GetRect().y1,
			   m_UIStaticNormal.GetRect().x2-m_UIStaticNormal.GetRect().x1,
			   m_UIStaticNormal.GetRect().y2-m_UIStaticNormal.GetRect().y1);*/

				
}

void  CUIButton::Update()
{
	RECT rect = GetAbsoluteRect();

	int right_offset;
	int down_offset;

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
	{
			right_offset = 0;
			down_offset = 0;
	}
	else
	{
			right_offset = PUSH_OFFSET_RIGHT;
			down_offset = PUSH_OFFSET_DOWN;
	}

	GetFont()->SetColor(0xFFEEEEEE);
	GetFont()->Out((float)rect.left + right_offset, 
				   (float)rect.top + down_offset,
				    m_str);

	if(m_bCursorOverButton)
	{
			GetFont()->SetColor(0xFF444444);
			GetFont()->Out((float)rect.left + right_offset + 2, 
					   (float)rect.top + down_offset + 2,
					    m_str);
	}
}