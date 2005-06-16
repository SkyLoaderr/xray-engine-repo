// file:		UIustomSpin.cpp
// description:	base class for CSpinNum & CSpinText
// created:		15.06.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UICustomSpin.h"

#define SPIN_HEIGHT 21
#define BTN_SIZE 11

CUICustomSpin::CUICustomSpin(){
	AttachChild(&m_frameLine);
	AttachChild(&m_btnUp);
	AttachChild(&m_btnDown);
	m_lines.SetTextAlignment(CGameFont::alLeft);
	m_lines.SetVTextAlignment(valCenter);
}

CUICustomSpin::~CUICustomSpin(){

}

void CUICustomSpin::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,SPIN_HEIGHT);
	m_frameLine.Init(0,0,width, SPIN_HEIGHT);
	m_frameLine.InitTexture("ui_spiner");
	m_btnUp.Init(width - BTN_SIZE, 0, BTN_SIZE, BTN_SIZE);
	m_btnUp.InitTexture("ui_spiner_button_t");
	m_btnDown.Init(width - BTN_SIZE, BTN_SIZE, BTN_SIZE, BTN_SIZE);
	m_btnDown.InitTexture("ui_spiner_button_b");
	m_lines.Init(0,0,width - BTN_SIZE, SPIN_HEIGHT);
}

void CUICustomSpin::SendMessage(CUIWindow* pWnd, s16 msg, void* pData /* = NULL */){
	if (BUTTON_CLICKED == msg)
	{
		if (&m_btnUp == pWnd)
			OnBtnUpClick();
		else if (&m_btnDown == pWnd)
			OnBtnDownClick();
	}
}

void CUICustomSpin::OnBtnUpClick(){
	// do nothing
}

void CUICustomSpin::OnBtnDownClick(){
	// do nothing
}

void CUICustomSpin::Draw(){
	CUIWindow::Draw();
	Fvector2 pos = GetAbsolutePos();
	m_lines.Draw(pos.x, pos.y);
}

