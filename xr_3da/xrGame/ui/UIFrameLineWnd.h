//=============================================================================
//  Filename:   UIFrameLineWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Обертка над UIFrameLine классом
//=============================================================================

#ifndef UI_FRAME_LINE_WND_H_
#define UI_FRAME_LINE_WND_H_

#pragma once

#include "UIFrameLine.h"
#include "UIWindow.h"
#include "UIStatic.h"

class CUIFrameLineWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
	CUIFrameLineWnd();
	virtual void Init(LPCSTR base_name, int x, int y, int width, int height, bool horizontal);
	virtual void Draw();
	virtual void SetWidth(int width);
	virtual void SetHeight(int height);
	virtual void SetOrientation(bool horizontal);
	void SetColor(u32 cl);

	// Also we can display textual caption on the frame
	CUIStatic		UITitleText;
	CUIStatic*		GetTitleStatic(){return &UITitleText;};

protected:
	bool			bHorizontal;
	CUIFrameLine	UIFrameLine;
};

#endif	// UI_FRAME_LINE_WND_H_