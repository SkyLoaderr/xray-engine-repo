// UIMapSpot.cpp:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapSpot.h"

CUIMapSpot::CUIMapSpot()
{
	m_pObject = NULL;
	m_vWorldPos.set(0,0,0);
}
CUIMapSpot::~CUIMapSpot()
{
}


void CUIMapSpot::Draw()
{
	//выполнить проверку на отсечение  элемента, на выход за пределы
	//родительского окна
	if(!m_bAvailableTexture) return;
	
	RECT parent_rect = GetParent()->GetAbsoluteRect();
	RECT rect = GetAbsoluteRect();
	RECT wnd_rect = GetWndRect();
	RECT out_rect;


	//проверить попадает ли изображение в окно
	if(rect.left>parent_rect.right || rect.right<parent_rect.left ||
		rect.top>parent_rect.bottom ||  rect.bottom<parent_rect.top) return;

	
	int out_x, out_y;
	out_x = rect.left;
	out_y = rect.top;

	out_rect.top = 0;
	out_rect.bottom = GetHeight();
	out_rect.left = 0;
	out_rect.right = GetWidth();


	if(wnd_rect.left<0)
	{
		out_rect.left = -wnd_rect.left;
	}
	else if(rect.right>parent_rect.right)
	{
		out_rect.right = GetWidth() - (rect.right-parent_rect.right);
	}


	if(wnd_rect.top<0)
	{
		out_rect.top = -wnd_rect.top;
	}
	else if(rect.bottom>parent_rect.bottom)
	{
		out_rect.bottom = GetHeight() - (rect.bottom-parent_rect.bottom);
	}

	
	m_UIStaticItem.SetPos(out_x, out_y);
	


	Irect r;
	r.x1 = out_rect.left;
	r.x2 = out_rect.right;
	r.y1 = out_rect.top;
	r.y2 = out_rect.bottom;
	m_UIStaticItem.SetRect(r);

	m_UIStaticItem.Render();
}
