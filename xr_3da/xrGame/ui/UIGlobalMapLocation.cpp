//=============================================================================
//  Filename:   UIGlobalMapLocation.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� c����� ��� ����������� ��������������� ��������� ����� �� ����������
//=============================================================================

#include "StdAfx.h"
#include "UIGlobalMapLocation.h"
#include "UIMapWnd.h"

//////////////////////////////////////////////////////////////////////////

CUIGlobalMapLocation::CUIGlobalMapLocation()
{
}

//////////////////////////////////////////////////////////////////////////

CUIGlobalMapLocation::~CUIGlobalMapLocation()
{

}

//////////////////////////////////////////////////////////////////////////

void CUIGlobalMapLocation::Init(int width, int height, RECT clipRect, LPCSTR frameBase)
{
	UIBorder.Init(frameBase, 0, 0, width, height);
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