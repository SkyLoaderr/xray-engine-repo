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


#define BOTTOM_OFFSET 30
#define LEFT_TOP_OFFSET 16
#define UP_TOP_OFFSET 28
#define LEFT_BOTTOM_OFFSET 19
#define UP_BOTTOM_OFFSET 125

void CUIFrameWindow::Init(LPCSTR base_name, int x, int y, int width, int height)
{
	//"ui\\ui_hud_frame"
	m_UIWndFrame.Init(base_name,x,y,width, height - BOTTOM_OFFSET,alNone);
	
	string256 buf;
	m_UIStaticOverLeftTop.Init(strconcat(buf,base_name,"_over_lt"),
								"hud\\default",	x,y,alNone);
	m_UIStaticOverLeftBottom.Init(strconcat(buf,base_name,"_over_lb"),
								"hud\\default",	x,y,alNone);
	
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
	m_UIStaticOverLeftTop.SetPos(rect.left-LEFT_TOP_OFFSET, 
								rect.top-UP_TOP_OFFSET);
	m_UIStaticOverLeftBottom.SetPos(rect.left-LEFT_BOTTOM_OFFSET, 
									rect.bottom - UP_BOTTOM_OFFSET - BOTTOM_OFFSET);

	m_UIWndFrame.Render();
	m_UIStaticOverLeftTop.Render();
	m_UIStaticOverLeftBottom.Render();

	CUIWindow::Draw();
}