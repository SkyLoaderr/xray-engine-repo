// UIButton.cpp: ����� ���������� ������
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIButton.h"
#include "../Level.h"
#include "../HUDManager.h"

#define PUSH_OFFSET_RIGHT 2
#define PUSH_OFFSET_DOWN  3


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIButton:: CUIButton()
{
	m_eButtonState		= BUTTON_NORMAL;
	m_ePressMode		= NORMAL_PRESS;

	m_str				= "";

	m_bButtonClicked	= false;
	m_bCursorOverButton	= false;

	m_bAvailableTexture	= false;

	m_bIsSwitch			= false;

	m_iPushOffsetX		= PUSH_OFFSET_RIGHT;
    m_iPushOffsetY		= PUSH_OFFSET_DOWN;

	SetTextAlign(CGameFont::alCenter);

	m_HighlightColor	= 0xFF999999;
	m_uAccelerator		= static_cast<u32>(-1);

	m_iTexOffsetX		= 0;
	m_iTexOffsetY		= 0;
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
}

void CUIButton::Init(int x, int y, int width, int height)
{
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;
	m_bButtonClicked = false;
	m_bCursorOverButton = false;

	inherited::Init(x, y, width, height);
}


void  CUIButton::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	//��������� �������� �� ������ �� ������
	//���������� ������ ������������ ����� ������
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
	{
		cursor_on_button = true;
	}
	else
	{
		cursor_on_button = false;
	}

	
	if(m_bCursorOverButton != cursor_on_button)
	{
		if(cursor_on_button)
			GetMessageTarget()->SendMessage(this,BUTTON_FOCUS_RECEIVED, NULL);
		else
			GetMessageTarget()->SendMessage(this,BUTTON_FOCUS_LOST, NULL);
	}
	m_bCursorOverButton = cursor_on_button;

	
	m_bButtonClicked = false;


	if(mouse_action == MOUSE_MOVE && m_eButtonState == BUTTON_NORMAL)
			GetParent()->SetCapture(this, cursor_on_button);


	switch(m_ePressMode)
	{
	//////////////////////////
	//���������� ����� �������
	//
	// ��������� ������ ��� ������������� ����������� ���� ������ � ������ NORMAL_PRESS
	// ������� ����������� � ������ ������� SetButtonMode(BUTTON_NORMAL)
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
					GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
					m_bButtonClicked = true;
				}
			
				if (!m_bIsSwitch)
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
		else if(mouse_action == LBUTTON_DOWN || mouse_action == LBUTTON_DB_CLICK)
		{
			if(cursor_on_button)
			{
				GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
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
				GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
				m_bButtonClicked = true;
			}	
		} 
		break;
	}

}

//���������� ������
void  CUIButton::Draw()
{
	CUIWindow::Draw();

	RECT rect = GetAbsoluteRect();

	if(m_bAvailableTexture)
	{
		if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
		{
			m_UIStaticItem.SetPos(rect.left + m_iTexOffsetX, rect.top + m_iTexOffsetY);
		}
		else
		{
			m_UIStaticItem.SetPos(rect.left + m_iPushOffsetX + m_iTexOffsetX, 
				rect.top + m_iPushOffsetY + m_iTexOffsetY);
		}

		m_UIStaticItem.Render();
	}

	CUIWindow::Draw();

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
		right_offset = m_iPushOffsetX;
		down_offset = m_iPushOffsetY;
	}

	UpdateTextAlign();
	GetFont()->SetAligment(GetTextAlign());

	if(IsHighlightText() && m_str && xr_strlen(m_str)>0)
	{
		GetFont()->SetColor(m_HighlightColor);
		HUD().OutText(GetFont(), GetClipRect(), 
			(float)rect.left + right_offset + 1 +m_iTextOffsetX, 
			(float)rect.top + down_offset + 1  +m_iTextOffsetY,
			m_str);
		HUD().OutText(GetFont(), GetClipRect(), 
			(float)rect.left + right_offset - 1 +m_iTextOffsetX, 
			(float)rect.top + down_offset - 1 +m_iTextOffsetY,
			m_str);
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + right_offset - 1 +m_iTextOffsetX, 
			(float)rect.top + down_offset + 1 +m_iTextOffsetY,
			m_str);
		HUD().OutText(GetFont(), GetClipRect(), 
			(float)rect.left + right_offset + 1 +m_iTextOffsetX, 
			(float)rect.top + down_offset - 1 +m_iTextOffsetY,
			m_str);
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + right_offset + 1 +m_iTextOffsetX, 
			(float)rect.top + down_offset + 0 +m_iTextOffsetY,
			m_str);
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + right_offset - 1 +m_iTextOffsetX, 
			(float)rect.top + down_offset - 0 +m_iTextOffsetY,
			m_str);
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + right_offset - 0 +m_iTextOffsetX, 
			(float)rect.top + down_offset + 1 +m_iTextOffsetY,
			m_str);
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + right_offset + 0 +m_iTextOffsetX,  
			(float)rect.top + down_offset - 1 +m_iTextOffsetY,
			m_str);
	}

	//	inherited::Update();
	//	return;


	GetFont()->SetColor(m_dwFontColor);

	if(m_str && xr_strlen(m_str)>0)
		HUD().OutText(GetFont(), GetClipRect(), 
		(float)rect.left + right_offset  +  m_iTextOffsetX, 
		(float)rect.top + down_offset  + m_iTextOffsetY,
		m_str);

	GetFont()->OnRender();
}


bool CUIButton::IsHighlightText()
{
	return m_bCursorOverButton;
}

void  CUIButton::Update()
{
	CUIWindow::Update();
}

void CUIButton::UpdateTextAlign()
{
	m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2;

	if(m_eTextAlign == CGameFont::alCenter)
	{
		m_iTextOffsetX = GetWidth()/2;
	}
	else if(m_eTextAlign == CGameFont::alRight)
	{
		m_iTextOffsetX = GetWidth();
	}
//	else
//	{
//		m_iTextOffsetX = 0;
//	}
}