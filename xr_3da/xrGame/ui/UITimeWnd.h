// File:		UITimeWnd.h
// Description:	Timer window for Inventory window
// Created:		01.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World
//

#pragma once

#include "UIStatic.h"

class CUITimeWnd : public CUIStatic {
	using CUIStatic::Init;
public:
	CUITimeWnd();
	virtual ~CUITimeWnd();
	virtual void Init(int x, int y, int widht, int height);
	virtual void Init(int x, int y);	
	virtual void InitTexture(LPCSTR tex_name) {/*only STUB!!: we can't change texture*/};
	virtual void SetFont(CGameFont* pFont);
	virtual void SetTextColor(u32 color);
	virtual void SetText(LPCSTR str);
	virtual void SetTextX(int x);
	virtual void SetTextY(int y);
	virtual void Update();
protected:
	CUIStatic	m_staticString;
	CUIStatic	m_staticTime;
};
