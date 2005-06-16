// file:		CCustomSpin.h
// description:	base class for CSpinNum & CSpinText
// created:		15.06.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#pragma once
#include "UIWindow.h"
#include "UI3tButton.h"
#include "UIFrameLineWnd.h"
#include "UILines.h"


class CUICustomSpin : public CUIWindow{
public:
	CUICustomSpin();
	~CUICustomSpin();

	virtual void	Init(float x, float y, float width, float height);
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, void* pData  = NULL);
	virtual void	OnBtnUpClick();
	virtual void	OnBtnDownClick();

	virtual void	Draw();

protected:
	CUIFrameLineWnd	m_frameLine;
	CUI3tButton		m_btnUp;
	CUI3tButton		m_btnDown;
	CUILines		m_lines;
};
