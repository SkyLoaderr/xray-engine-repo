#include "StdAfx.h"

#include "UILabel.h"

CUILabel::CUILabel()
{
	m_textPos.set(0,0);
	m_lines.SetVTextAlignment(valCenter);
}

void CUILabel::Init(float x, float y, float width, float height){
	CUIFrameLineWnd::Init(x,y,width,height);
	m_lines.Init(0,0,width, height);
}

void CUILabel::Draw(){
	CUIFrameLineWnd::Draw();
	Frect r = GetAbsoluteRect();
	m_lines.Draw(r.x1 + m_textPos.x, r.y1 + m_textPos.y);
}

void CUILabel::SetTextPosX(float x){
	m_textPos.x = x;
}

void CUILabel::SetTextPosY(float y){
	m_textPos.y = y;
}

void CUILabel::SetWidth(float width){
	m_lines.SetWidth(width);
	CUIFrameLineWnd::SetWidth(width);
}

void CUILabel::SetHeight(float height){
	m_lines.SetHeight(height);
	CUIFrameLineWnd::SetHeight(height);
}