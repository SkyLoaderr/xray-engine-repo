// UIButton.cpp: класс нажимаемой кнопки
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIButton.h"
#include "../HUDManager.h"
#include "UILines.h"
#include "UIBtnHint.h"

#define PUSH_OFFSET_RIGHT 1
#define PUSH_OFFSET_DOWN  1



CUIButton:: CUIButton()
{
	m_eButtonState				= BUTTON_NORMAL;
	m_ePressMode				= NORMAL_PRESS;
	m_bButtonClicked			= false;
	m_bAvailableTexture			= false;
	m_bIsSwitch					= false;

	m_iPushOffsetX				= PUSH_OFFSET_RIGHT;
    m_iPushOffsetY				= PUSH_OFFSET_DOWN;

	m_HighlightColor			= 0xFFFFFFFF;
	m_uAccelerator				= static_cast<u32>(-1);

	m_iTexOffsetX				= 0.0f;
	m_iTexOffsetY				= 0.0f;

	m_bEnableTextHighlighting	= true;

	m_iShadowOffsetX			= 0.0f;
	m_iShadowOffsetY			= 0.0f;

	SetTextAlignment			(CGameFont::alCenter); // this will create class instance for m_pLines
	SetVTextAlignment			(valCenter);
	m_bClickable				= true;

}

 CUIButton::~ CUIButton()
{
}

void CUIButton::Reset()
{
	m_eButtonState				= BUTTON_NORMAL;
	m_bButtonClicked			= false;
	m_bCursorOverWindow			= false;
	inherited::Reset			();
}


void CUIButton::Init(LPCSTR tex_name, float x, float y, float width, float height)
{
	CUIStatic::Init				(tex_name, x, y, width, height);
}

void CUIButton::Init(float x, float y, float width, float height)
{
	CUIStatic::Init				(x, y, width, height);
}

void CUIButton::Enable(bool status){
	CUIStatic::Enable			(status);

	if (!status)
		m_bCursorOverWindow		= false;
}

bool  CUIButton::OnMouse(float x, float y, EUIMessages mouse_action)
{
	m_bButtonClicked			= false;

	if( inherited::OnMouse(x, y, mouse_action) ) return true;

	if ( (	WINDOW_LBUTTON_DOWN==mouse_action	||
			WINDOW_LBUTTON_UP==mouse_action		||
			WINDOW_RBUTTON_DOWN==mouse_action	||
			WINDOW_RBUTTON_UP==mouse_action)	&& 
			HasChildMouseHandler())
		return false;

	//if(mouse_action == WINDOW_MOUSE_MOVE && m_eButtonState == BUTTON_NORMAL)
	//	GetParent()->SetCapture(this, m_bCursorOverWindow);

	switch (m_ePressMode)
	{
	case NORMAL_PRESS:
		if(m_eButtonState == BUTTON_NORMAL)
		{
			if(mouse_action == WINDOW_LBUTTON_DOWN || mouse_action == WINDOW_LBUTTON_DB_CLICK)
			{
				m_eButtonState = BUTTON_PUSHED;
				GetMessageTarget()->SendMessage(this, BUTTON_DOWN, NULL);
//				GetParent()->SetCapture(this, true);
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
			
//				GetParent()->SetCapture(this, false);
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
//				GetParent()->SetCapture(this, false);
			}
		}
		break;

	case DOWN_PRESS:
        if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			if(m_bCursorOverWindow)
			{
				m_eButtonState = BUTTON_PUSHED;
//				GetParent()->SetCapture(this, true);
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
//				GetParent()->SetCapture(this, false);
			}
		}
		else if(mouse_action == WINDOW_LBUTTON_DOWN || mouse_action == WINDOW_LBUTTON_DB_CLICK)
			if(m_bCursorOverWindow)
				OnClick();

		break;
	}
	return false;
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
	u32 def_col = m_pLines->GetTextColor();
	m_pLines->SetTextColor(m_HighlightColor);

	m_pLines->Draw(	rect.left + right_offset + 0 +m_iTextOffsetX + m_iShadowOffsetX, 
					rect.top + down_offset   - 0 +m_iTextOffsetY + m_iShadowOffsetY);

	m_pLines->SetTextColor(def_col);

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

	CUIStatic::DrawText();
	if(g_btnHint->Owner()==this)
		g_btnHint->Draw_();
}


bool is_in2(const Frect& b1, const Frect& b2){
	return (b1.x1<b2.x1)&&(b1.x2>b2.x2)&&(b1.y1<b2.y1)&&(b1.y2>b2.y2);
}

void  CUIButton::Update()
{
	CUIStatic::Update();

	if(CursorOverWindow() && m_hint_text.size() && !g_btnHint->Owner() && Device.dwTimeGlobal>m_dwFocusReceiveTime+500)
	{
		g_btnHint->SetHintText	(this,*m_hint_text);

		Fvector2 c_pos			= GetUICursor()->GetPos();
		Frect vis_rect;
		vis_rect.set			(0,0,UI_BASE_WIDTH, UI_BASE_HEIGHT);

		//select appropriate position
		Frect r;
		r.set					(0.0f, 0.0f, g_btnHint->GetWidth(), g_btnHint->GetHeight());
		r.add					(c_pos.x, c_pos.y);

		r.sub					(0.0f,r.height());
		if (false==is_in2(vis_rect,r))
			r.sub				(r.width(),0.0f);
		if (false==is_in2(vis_rect,r))
			r.add				(0.0f,r.height());

		if (false==is_in2(vis_rect,r))
			r.add				(r.width(), 45.0f);

		g_btnHint->SetWndPos(r.lt);
	}
}

void CUIButton::OnFocusLost()
{
	inherited::OnFocusLost();
	if(g_btnHint->Owner()==this)
		g_btnHint->Discard	();
}

