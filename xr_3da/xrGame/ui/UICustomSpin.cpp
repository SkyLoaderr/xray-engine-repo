// file:		UIustomSpin.cpp
// description:	base class for CSpinNum & CSpinText
// created:		15.06.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UI3tButton.h"
#include "UIFrameLineWnd.h"
#include "UILines.h"
#include "UICustomSpin.h"

#define SPIN_HEIGHT 21
#define BTN_SIZE 11

CUICustomSpin::CUICustomSpin(){
	m_pFrameLine	= xr_new<CUIFrameLineWnd>();
	m_pBtnUp		= xr_new<CUI3tButton>();
	m_pBtnDown		= xr_new<CUI3tButton>();
	m_pLines		= xr_new<CUILines>();

	AttachChild(m_pFrameLine);
	AttachChild(m_pBtnUp);
	AttachChild(m_pBtnDown);
	m_pLines->SetTextAlignment(CGameFont::alLeft);
	m_pLines->SetVTextAlignment(valCenter);
}

CUICustomSpin::~CUICustomSpin(){

}

void CUICustomSpin::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,SPIN_HEIGHT);
	m_pFrameLine->Init(0,0,width, SPIN_HEIGHT);
	m_pFrameLine->InitTexture("ui_spiner");
	m_pBtnUp->Init(width - BTN_SIZE, 0, BTN_SIZE, BTN_SIZE);
	m_pBtnUp->InitTexture("ui_spiner_button_t");
	m_pBtnDown->Init(width - BTN_SIZE, BTN_SIZE, BTN_SIZE, BTN_SIZE);
	m_pBtnDown->InitTexture("ui_spiner_button_b");

	m_pLines->Init(10,0,width - BTN_SIZE - 10, SPIN_HEIGHT);
}

void CUICustomSpin::SendMessage(CUIWindow* pWnd, s16 msg, void* pData /* = NULL */){
	if (BUTTON_CLICKED == msg)
	{
		if (m_pBtnUp == pWnd)
			OnBtnUpClick();
		else if (m_pBtnDown == pWnd)
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
	m_pLines->Draw(pos.x, pos.y);
}

