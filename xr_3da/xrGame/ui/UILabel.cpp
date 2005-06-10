#include "StdAfx.h"

#include "UILabel.h"

CUILabel::CUILabel()
{
	m_textPos.set(0,0);
	m_lines.SetVTextAlignment(valCenter);
}

void CUILabel::Init(int x, int y, int width, int height){
	CUIFrameLineWnd::Init(x,y,width,height);
	m_lines.Init(0,0,width, height);
}

void CUILabel::Draw(){
	CUIFrameLineWnd::Draw();
	Irect r = GetAbsoluteRect();
	m_lines.Draw(r.x1 + m_textPos.x, r.y1 + m_textPos.y);
}

void CUILabel::SetTextPosX(int x){
	m_textPos.x = x;
}

void CUILabel::SetTextPosY(int y){
	m_textPos.y = y;
}