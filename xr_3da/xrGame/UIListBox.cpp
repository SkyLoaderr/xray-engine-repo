#include "stdafx.h"
#include "UIListBox.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIListBox::CUIListBox()
{
}
//--------------------------------------------------------------------

void CUIListBox::Init(LPCSTR tex, int x1, int y1, int w, int h, DWORD align)
{
	frame.Init		(tex,x1,y1,w,h,align);
	list_rect.set	(frame.iPos.x,frame.iPos.y,frame.iPos.x+frame.iSize.x,frame.iPos.y+frame.iSize.y);
	list_rect.shrink(40,40);
	Level().HUD()->ClientToScreen(list_rect, align);
}
//--------------------------------------------------------------------

void CUIListBox::Render()
{
	frame.Render	();
}
//--------------------------------------------------------------------

void CUIListBox::OnFrame()
{
}
//--------------------------------------------------------------------
