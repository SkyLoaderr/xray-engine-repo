//=============================================================================
//  Filename:   UIGlobalMapLocation.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс cлужит для отображения прямоугольничка локальной карты на глобальной
//=============================================================================

#include "StdAfx.h"
#include "UIGlobalMapLocation.h"
#include "UIMapWnd.h"

//////////////////////////////////////////////////////////////////////////

const char * const	BORDER_FRAME	= "ui\\ui_pda_frame_sub";

//////////////////////////////////////////////////////////////////////////

CUIGlobalMapLocation::CUIGlobalMapLocation()
{
}

//////////////////////////////////////////////////////////////////////////

CUIGlobalMapLocation::~CUIGlobalMapLocation()
{

}

//////////////////////////////////////////////////////////////////////////

void CUIGlobalMapLocation::Init(int width, int height, RECT clipRect)
{
	UIBorder.Init(BORDER_FRAME, 0, 0, width, height);
	inherited::SetWidth(width);
	inherited::SetHeight(height);
	UIBorder.SetClipper(true, clipRect);
	AttachChild(&UIBorder);
}

//////////////////////////////////////////////////////////////////////////

void CUIGlobalMapLocation::Draw()
{
	CUIWindow::Draw();
}