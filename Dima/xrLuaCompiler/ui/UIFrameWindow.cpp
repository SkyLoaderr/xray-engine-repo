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
	m_bOverLeftTop = false; 
	m_bOverLeftBottom = false;
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
	m_UIWndFrame.Init(base_name,x,y,width, height - BOTTOM_OFFSET,alNone);
	
	
	CUIWindow::Init(x,y, width, height);
}

void CUIFrameWindow::Init(LPCSTR base_name, RECT* pRect)
{
	Init(base_name, pRect->left, pRect->top, 
				pRect->right - pRect->left, 
				pRect->bottom - pRect->top);
}




void CUIFrameWindow::InitLeftTop(LPCSTR tex_name, int left_offset, int up_offset)
{
	m_iLeftTopOffset = left_offset;
	m_iUpTopOffset = up_offset;

	m_UIStaticOverLeftTop.Init(tex_name, "hud\\default", 
										  GetWndRect().left - left_offset,
  										  GetWndRect().top - up_offset,alNone);
										  


	m_bOverLeftTop = true;
}
void CUIFrameWindow::InitLeftBottom(LPCSTR tex_name, int left_offset, int up_offset)
{
	m_iLeftBottomOffset = left_offset;
	m_iUpBottomOffset = up_offset;

	m_UIStaticOverLeftBottom.Init(tex_name, "hud\\default", 
											GetWndRect().left - left_offset,
											GetWndRect().bottom - up_offset,alNone);

	m_bOverLeftBottom = true;
}



//
// прорисовка окна
//
void CUIFrameWindow::Draw()
{
	RECT rect = GetAbsoluteRect();


	m_UIWndFrame.SetPos(rect.left, rect.top);


	m_UIWndFrame.Render();

	if(m_bOverLeftTop)
	{
		m_UIStaticOverLeftTop.SetPos(rect.left- m_iLeftTopOffset, 
								rect.top- m_iUpTopOffset);
	
		m_UIStaticOverLeftTop.Render();
	}
	
	if(m_bOverLeftBottom)
	{
		m_UIStaticOverLeftBottom.SetPos(rect.left- m_iLeftBottomOffset, 
									rect.bottom - m_iUpBottomOffset);
		m_UIStaticOverLeftBottom.Render();
	}

	CUIWindow::Draw();
}