// UIFrameWindow.cpp: 
//
// окно осуществялющие граф. вывод через CUIFrameRect
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIFrameWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFrameWindow::CUIFrameWindow()
{
}

CUIFrameWindow::~CUIFrameWindow()
{
}


void CUIFrameWindow::Init(LPCSTR base_name, int x, int y, int width, int height)
{
	//"ui\\ui_hud_frame"
	m_UIWndFrame.Init(base_name,x,y,width, height,alNone);
	CUIWindow::Init(x,y, width, height);
}

void CUIFrameWindow::Init(LPCSTR base_name, RECT* pRect)
{
	Init(base_name, pRect->left, pRect->top, 
				pRect->right - pRect->left, 
				pRect->bottom - pRect->top);
}

//
// прорисовка окна
//
void CUIFrameWindow::Draw()
{
	RECT rect = GetAbsoluteRect();
	m_UIWndFrame.SetPos(rect.left, rect.top);

	m_UIWndFrame.Render();

	CUIWindow::Draw();
}