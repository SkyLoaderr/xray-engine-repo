// UIButton.cpp: класс нажимаемой кнопки
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIButton.h"
#include "../HUDManager.h"
#include "UILines.h"

#define PUSH_OFFSET_RIGHT 1
#define PUSH_OFFSET_DOWN  1


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIButton:: CUIButton()
{
	m_eButtonState				= BUTTON_NORMAL;
	m_ePressMode				= NORMAL_PRESS;

//	m_str						= "";

	m_bButtonClicked			= false;
	
	m_bAvailableTexture			= false;

	m_bIsSwitch					= false;

	m_iPushOffsetX				= PUSH_OFFSET_RIGHT;
    m_iPushOffsetY				= PUSH_OFFSET_DOWN;

	SetTextAlign				(CGameFont::alCenter);

	m_HighlightColor			= 0xFF999999;
	m_uAccelerator				= static_cast<u32>(-1);

	m_iTexOffsetX				= 0.0f;
	m_iTexOffsetY				= 0.0f;

	m_bNewRenderMethod			= false;
	m_bEnableTextHighlighting	= true;

	m_iShadowOffsetX			= 0.0f;
	m_iShadowOffsetY			= 0.0f;

    //m_pLines = xr_new<CUILines>();
	SetTextAlignment(CGameFont::alCenter); // this will create class instance for m_pLines
	SetVTextAlignment(valCenter);
	m_bClickable				= true;

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


void CUIButton::Init(LPCSTR tex_name, float x, float y, float width, float height)
{
	CUIStatic::Init(tex_name, x, y, width, height);
}

void CUIButton::Init(float x, float y, float width, float height)
{
	CUIStatic::Init(x, y, width, height);
}

void CUIButton::Enable(bool status){
	CUIStatic::Enable(status);

	if (!status)
		m_bCursorOverWindow = false;
}

void  CUIButton::OnMouse(float x, float y, EUIMessages mouse_action)
{
	m_bButtonClicked = false;

	inherited::OnMouse(x, y, mouse_action);

	if ( (	WINDOW_LBUTTON_DOWN==mouse_action	||
			WINDOW_LBUTTON_UP==mouse_action		||
			WINDOW_RBUTTON_DOWN==mouse_action	||
			WINDOW_RBUTTON_UP==mouse_action)	&& 
			HasChildMouseHandler())
		return;

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
	Frect rect = GetAbsoluteRect();

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
			Frect r={0,0,
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
	float right_offset;
	float down_offset;

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
		//|| !m_bAvailableTexture)
	{
		right_offset = 0.0f;
		down_offset = 0.0f;
	}
	else
	{
		right_offset = m_iPushOffsetX;
		down_offset = m_iPushOffsetY;
	}

	Frect rect = GetAbsoluteRect();
//	Irect r = GetSelfClipRect();
//	CGameFont * F = GetFont();
//	F->SetColor(m_HighlightColor);
	u32 def_col = m_pLines->GetTextColor();
	m_pLines->SetTextColor(m_HighlightColor);

	m_pLines->Draw(	rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   + 1 +m_iTextOffsetY + m_iShadowOffsetY);
	m_pLines->Draw(	rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   - 1 +m_iTextOffsetY + m_iShadowOffsetY);
	m_pLines->Draw(	rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   + 1 +m_iTextOffsetY + m_iShadowOffsetY);
	m_pLines->Draw(	rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   - 1 +m_iTextOffsetY + m_iShadowOffsetY);
	m_pLines->Draw(	rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   + 0 +m_iTextOffsetY + m_iShadowOffsetY);
	m_pLines->Draw(	rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   - 0 +m_iTextOffsetY + m_iShadowOffsetY);
	m_pLines->Draw(	rect.left + right_offset - 0 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   + 1 +m_iTextOffsetY + m_iShadowOffsetY);
	m_pLines->Draw(	rect.left + right_offset + 0 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   - 1 +m_iTextOffsetY + m_iShadowOffsetY);

	m_pLines->SetTextColor(def_col);

	//UI()->OutText(F, r, 
	//	(float)rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
	//	(float)rect.top + down_offset + 1  +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
	//UI()->OutText(F, r, 
	//	(float)rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
	//	(float)rect.top + down_offset - 1 +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
	//UI()->OutText(F, r,
	//	(float)rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
	//	(float)rect.top + down_offset + 1 +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
	//UI()->OutText(F, r, 
	//	(float)rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
	//	(float)rect.top + down_offset - 1 +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
	//UI()->OutText(F, r,
	//	(float)rect.left + right_offset + 1 +m_iTextOffsetX + m_iShadowOffsetX, 
	//	(float)rect.top + down_offset + 0 +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
	//UI()->OutText(F, r,
	//	(float)rect.left + right_offset - 1 +m_iTextOffsetX + m_iShadowOffsetX, 
	//	(float)rect.top + down_offset - 0 +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
	//UI()->OutText(F, r,
	//	(float)rect.left + right_offset - 0 +m_iTextOffsetX + m_iShadowOffsetX, 
	//	(float)rect.top + down_offset + 1 +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
	//UI()->OutText(F, r,
	//	(float)rect.left + right_offset + 0 +m_iTextOffsetX + m_iShadowOffsetX,  
	//	(float)rect.top + down_offset - 1 +m_iTextOffsetY + m_iShadowOffsetY,
	//	m_str);
}

void CUIButton::DrawText(){
	float right_offset;
	float down_offset;

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

	if(IsHighlightText() && xr_strlen(m_pLines->GetText())>0 && m_bEnableTextHighlighting)
	{
		DrawHighlightedText();
	}

	CUIStatic::DrawText();
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
	//if (m_iTextOffsetY < 0)
	//	m_iTextOffsetY = (GetHeight() - (int)GetFont()->CurrentHeight())/2;

	//if(m_eTextAlign == CGameFont::alCenter)
	//{
	//	m_iTextOffsetX = GetWidth()/2;
	//}
	//else if(m_eTextAlign == CGameFont::alRight)
	//{
	//	m_iTextOffsetX = GetWidth();
	//}
	//else
	//{
	//	m_iTextOffsetX = 0;
	//}
}