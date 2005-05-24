// UIButton.cpp: ����� ���������� ������
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIButton.h"
#include "../HUDManager.h"

#define PUSH_OFFSET_RIGHT 1
#define PUSH_OFFSET_DOWN  1


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIButton:: CUIButton()
{
	m_eButtonState				= BUTTON_NORMAL;
	m_ePressMode				= NORMAL_PRESS;

	m_str						= "";

	m_bButtonClicked			= false;
	
	m_bAvailableTexture			= false;

	m_bIsSwitch					= false;

	m_iPushOffsetX				= PUSH_OFFSET_RIGHT;
    m_iPushOffsetY				= PUSH_OFFSET_DOWN;

	SetTextAlign				(CGameFont::alCenter);

	m_HighlightColor			= 0xFF999999;
	m_uAccelerator				= static_cast<u32>(-1);

	m_iTexOffsetX				= 0;
	m_iTexOffsetY				= 0;

	m_bNewRenderMethod			= false;
	m_bEnableTextHighlighting	= true;

	m_iShadowOffsetX			= 0;
	m_iShadowOffsetY			= 0;

	m_lines.SetTextAlignment(CGameFont::alCenter);
}

 CUIButton::~ CUIButton()
{

}

void CUIButton::Reset()
{
	m_eButtonState = BUTTON_NORMAL;
	m_bButtonClicked = false;
	m_bCursorOverWindow = false;

	inherited::Reset();
}


void CUIButton::Init(LPCSTR tex_name, int x, int y, int width, int height)
{
	CUIStatic::Init(tex_name, x, y, width, height);
}

void CUIButton::Init(int x, int y, int width, int height)
{
	CUIStatic::Init(x, y, width, height);
}

void CUIButton::Enable(bool status){
	CUIStatic::Enable(status);

	if (!status)
		m_bCursorOverWindow = false;
}

void  CUIButton::OnMouse(int x, int y, EUIMessages mouse_action)
{
	m_bButtonClicked = false;

	inherited::OnMouse(x, y, mouse_action);
    
	if(mouse_action == WINDOW_MOUSE_MOVE && m_eButtonState == BUTTON_NORMAL)
		GetParent()->SetCapture(this, m_bCursorOverWindow);

	switch (m_ePressMode)
	{
	case NORMAL_PRESS:
		if(m_eButtonState == BUTTON_NORMAL)
		{
			if(mouse_action == WINDOW_LBUTTON_DOWN || mouse_action == WINDOW_LBUTTON_DB_CLICK)
			{
				m_eButtonState = BUTTON_PUSHED;
				GetMessageTarget()->SendMessage(this, BUTTON_DOWN, NULL);
				GetParent()->SetCapture(this, true);
			}
		}
		else if(m_eButtonState == BUTTON_PUSHED)
		{
			if(mouse_action == WINDOW_LBUTTON_UP)
			{
				if(m_bCursorOverWindow)
					OnClick();
			
				if (!m_bIsSwitch)
					m_eButtonState = BUTTON_NORMAL;
			
				GetParent()->SetCapture(this, false);
			}
			else if(mouse_action == WINDOW_MOUSE_MOVE)
			{
				if(!m_bCursorOverWindow && !m_bIsSwitch)
					m_eButtonState = BUTTON_UP;
			}
		}
		else if(m_eButtonState == BUTTON_UP)
		{
			if(mouse_action == WINDOW_MOUSE_MOVE)
			{
				if(m_bCursorOverWindow)
					m_eButtonState = BUTTON_PUSHED;
			}
			else if(mouse_action == WINDOW_LBUTTON_UP)
			{
				m_eButtonState = BUTTON_NORMAL;
				GetParent()->SetCapture(this, false);
			}
		}
		break;

	case DOWN_PRESS:
        if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			if(m_bCursorOverWindow)
			{
				m_eButtonState = BUTTON_PUSHED;
				GetParent()->SetCapture(this, true);
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
				GetParent()->SetCapture(this, false);
			}
		}
		else if(mouse_action == WINDOW_LBUTTON_DOWN || mouse_action == WINDOW_LBUTTON_DB_CLICK)
			if(m_bCursorOverWindow)
				OnClick();

		break;
	}
}

void CUIButton::OnClick(){
	GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
	m_bButtonClicked = true;
}

void CUIButton::DrawTexture()
{
	Irect rect = GetAbsoluteRect();

	if(m_bAvailableTexture && m_bTextureEnable)
	{
		if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
			m_UIStaticItem.SetPos(rect.left + m_iTexOffsetX, rect.top + m_iTexOffsetY);
		else
			m_UIStaticItem.SetPos(rect.left + m_iPushOffsetX + m_iTexOffsetX, rect.top + m_iPushOffsetY + m_iTexOffsetY);

		if(m_bStretchTexture)
			m_UIStaticItem.SetRect(0, 0, rect.width(), rect.height());
		else
		{
			Irect r={0,0,
				m_UIStaticItem.GetOriginalRectScaled().width(),
				m_UIStaticItem.GetOriginalRectScaled().height()};
			m_UIStaticItem.SetRect(r);
		}

		if( Heading() )
			m_UIStaticItem.Render( GetHeading() );
		else
			m_UIStaticItem.Render();		
	}
}

void CUIButton::DrawHighlightedText(){
	int right_offset;
	int down_offset;

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
		//|| !m_bAvailableTexture)
	{
		right_offset = 0;
		down_offset = 0;
	}
	else
	{
		right_offset = m_iPushOffsetX;
		down_offset = m_iPushOffsetY;
	}

	Irect rect = GetAbsoluteRect();
	Irect r = GetSelfClipRect();
	CGameFont * F = GetFont();
	F->SetColor(m_HighlightColor);

	UI()->OutText(F, r, 
		(float)rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
		(float)rect.top + down_offset + 1  +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
	UI()->OutText(F, r, 
		(float)rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
		(float)rect.top + down_offset - 1 +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
	UI()->OutText(F, r,
		(float)rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
		(float)rect.top + down_offset + 1 +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
	UI()->OutText(F, r, 
		(float)rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
		(float)rect.top + down_offset - 1 +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
	UI()->OutText(F, r,
		(float)rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
		(float)rect.top + down_offset + 0 +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
	UI()->OutText(F, r,
		(float)rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
		(float)rect.top + down_offset - 0 +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
	UI()->OutText(F, r,
		(float)rect.left + right_offset - 0 +m_iTextOffsetX + m_iShadowOffsetX, 
		(float)rect.top + down_offset + 1 +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
	UI()->OutText(F, r,
		(float)rect.left + right_offset + 0 +m_iTextOffsetX + m_iShadowOffsetX,  
		(float)rect.top + down_offset - 1 +m_iTextOffsetY + m_iShadowOffsetY,
		m_str);
}

void CUIButton::DrawText(){
	int right_offset;
	int down_offset;

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
	{
		right_offset = 0;
		down_offset = 0;
	}
	else
	{
		right_offset = m_iPushOffsetX;
		down_offset = m_iPushOffsetY;
	}

	Irect rect = GetAbsoluteRect();
	CGameFont * F = GetFont();

	if (F)
	{
		UpdateTextAlign();
		F->SetAligment(GetTextAlign());

		if(IsHighlightText() && m_str && xr_strlen(m_str)>0 && m_bEnableTextHighlighting)
		{
			DrawHighlightedText();
		}

		F->SetColor(m_dwFontColor);

		if (!m_bNewRenderMethod)
		{
			if(m_str && xr_strlen(m_str)>0)
				UI()->OutText(GetFont(), GetSelfClipRect(), 
				(float)rect.left + right_offset  +  m_iTextOffsetX, 
				(float)rect.top + down_offset  + m_iTextOffsetY,
				m_str);
			GetFont()->OnRender();
		}
		else
		{
			rect.left	+=	right_offset;
			rect.top	+=	down_offset;

			inherited::DrawString(rect);
		}
	}

    Irect r = GetAbsoluteRect();
	m_lines.Draw(r.x1 + m_iTextOffsetX, r.y1 + m_iTextOffsetY);
}


bool CUIButton::IsHighlightText()
{
	return m_bCursorOverWindow;
}

void  CUIButton::Update()
{
	CUIStatic::Update();
}

void CUIButton::UpdateTextAlign()
{
#pragma todo("Need change for ::m_lines")
	if (m_iTextOffsetY < 0)
		m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2;

	if(m_eTextAlign == CGameFont::alCenter)
	{
		m_iTextOffsetX = GetWidth()/2;
	}
	else if(m_eTextAlign == CGameFont::alRight)
	{
		m_iTextOffsetX = GetWidth();
	}
	else
	{
		m_iTextOffsetX = 0;
	}
}