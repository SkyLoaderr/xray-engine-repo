//=============================================================================
//  Filename:   UIFrameLineWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Обертка над UIFrameLine классом
//=============================================================================

#include "stdafx.h"
#include "UIFrameLineWnd.h"

//////////////////////////////////////////////////////////////////////////

CUIFrameLineWnd::CUIFrameLineWnd()
	:	bHorizontal(true)
{
	AttachChild(&UITitleText);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Init(int x, int y, int width, int height){
	inherited::Init(x,y, width, height);

	UITitleText.Init(0,0, width, 50);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Init(LPCSTR base_name, int x, int y, int width, int height, bool horizontal)
{
	if (horizontal)
	{
		UIFrameLine.Init(base_name, x, y, width, horizontal, alNone);
		UITitleText.Init(0,0, width, 50);
	}
	else
	{
		UIFrameLine.Init(base_name, x, y, height, horizontal, alNone);
		UITitleText.Init(0,0, 50, height);
	}	

	inherited::Init(x,y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::InitTexture(LPCSTR tex_name, bool horizontal){

	RECT rect = GetAbsoluteRect();	

	if (horizontal)
	{
		UIFrameLine.Init(tex_name, rect.left, rect.top, rect.right - rect.left, horizontal, alNone);
		UITitleText.Init(0,0, rect.right - rect.left, 50);
	}
	else
	{
		UIFrameLine.Init(tex_name, rect.left, rect.top, rect.bottom - rect.top, horizontal, alNone);
		UITitleText.Init(0,0, 50, rect.bottom - rect.top);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Draw()
{
	RECT rect = GetAbsoluteRect();
	UIFrameLine.SetPos(rect.left, rect.top);
	UIFrameLine.Render();

	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetWidth(int width)
{
	inherited::SetWidth(width);
	if (bHorizontal)
		UIFrameLine.SetSize(width);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetHeight(int height)
{
	inherited::SetHeight(height);
	if (!bHorizontal)
		UIFrameLine.SetSize(height);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetOrientation(bool horizontal)
{
	UIFrameLine.SetOrientation(horizontal);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetColor(u32 cl)
{
	UIFrameLine.SetColor(cl);
}