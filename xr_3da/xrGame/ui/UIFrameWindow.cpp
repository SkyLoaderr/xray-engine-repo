// UIFrameWindow.cpp: 
//
// окно осуществялющие граф. вывод через CUIFrameRect
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIFrameWindow.h"
#include "../HUDManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFrameWindow::CUIFrameWindow()
	:	m_bClipper	(false)
{
	m_bOverLeftTop = false; 
	m_bOverLeftBottom = false;
}

CUIFrameWindow::~CUIFrameWindow()
{
}


//#define BOTTOM_OFFSET 30
//#define LEFT_TOP_OFFSET 16
//#define UP_TOP_OFFSET 28
//#define LEFT_BOTTOM_OFFSET 19
//#define UP_BOTTOM_OFFSET 125

void CUIFrameWindow::Init(LPCSTR base_name, int x, int y, int width, int height)
{
	m_UIWndFrame.Init(base_name,x,y,width, height,alNone);
	
	AttachChild(&UITitleText);
	UITitleText.Init(0,0, width, 50);
		
	inherited::Init(x,y, width, height);
}

void CUIFrameWindow::Init(LPCSTR base_name, Irect* pRect)
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
	Irect rect = GetAbsoluteRect();


	m_UIWndFrame.SetPos(rect.left, rect.top);


	// Clipper all static items in frame rect
	if (m_bClipper)
	{
		FrameClip(m_ClipRect);
	}

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

	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameWindow::SetWidth(int width)
{
	inherited::SetWidth(width);
	m_UIWndFrame.SetSize(GetWidth(), GetHeight());
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameWindow::SetHeight(int height)
{
	inherited::SetHeight(height);
	m_UIWndFrame.SetSize(GetWidth(), GetHeight());
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameWindow::SetColor(u32 cl)
{
	m_UIWndFrame.SetColor(cl);
	m_UIStaticOverLeftBottom.SetColor(cl);
	m_UIStaticOverLeftTop.SetColor(cl);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameWindow::FrameClip(const Irect parentAbsR)
{
	using std::min;
	using std::max;
	VERIFY(g_bRendering);

	// Если нет границ клиппанья, то скипаем
	if (!GetParent()) return;

	// Клиппаем по границам окна-парента
	Irect		ourAbsR		= GetAbsoluteRect();
	CTexture	*T;
	Ivector2	ts;
	int			rem_x, rem_y, tile_x, tile_y;
	int			size_x, size_y;
	Irect		r, null;
	null.set(0, 0, 0, 0);

	m_UIWndFrame.UpdateSize();

	// Проверяем на видимость фрейма
	if (   max(ourAbsR.right, parentAbsR.left) == parentAbsR.left
		|| min(ourAbsR.left, parentAbsR.right) == parentAbsR.right
		|| min(ourAbsR.top, parentAbsR.bottom) == parentAbsR.bottom
		|| max(ourAbsR.bottom, parentAbsR.top) == parentAbsR.top)
	{
		m_UIWndFrame.frame[CUIFrameRect::fmRT].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmR].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmRB].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmB].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmLB].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmL].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmLT].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmT].SetTile(0, 0, 0, 0);
		m_UIWndFrame.back.SetTile(0, 0, 0, 0);
		return;
	}

//	m_UIWndFrame.frame[CUIFrameRect::fmRT].SetRect(null);
//	m_UIWndFrame.frame[CUIFrameRect::fmR].SetRect(null);
//	m_UIWndFrame.frame[CUIFrameRect::fmRB].SetRect(null);
//	m_UIWndFrame.frame[CUIFrameRect::fmB].SetRect(null);
//	m_UIWndFrame.frame[CUIFrameRect::fmLB].SetRect(null);
//	m_UIWndFrame.frame[CUIFrameRect::fmL].SetRect(null);
//	m_UIWndFrame.frame[CUIFrameRect::fmLT].SetRect(null);
//	m_UIWndFrame.frame[CUIFrameRect::fmT].SetRect(null);

	// Для каждого статик айтема модифицируем его рект
	// Блин, статикайтем отдает свои размер как размер текстуры, даже если в нем включен тайлинг.
	// Из-за этого и такой геморой с изменениями размеров у затайленой части FrameRect'a

	// fmRT
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosY() ;
	r.x2 = min(ourAbsR.right, parentAbsR.right) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX(), parentAbsR.left) + r.x1;
	r.y2 = min(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosY(), parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRT].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRT].SetTile(1, 1, 0, 0);

	// fmR
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmR].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_y		= min(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosY(), parentAbsR.top);
	size_x		= min(ourAbsR.right, parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosX(), parentAbsR.left);
	rem_y		= size_y % ts.y;
	rem_x		= size_x % ts.x;
	tile_y		= iFloor(float(size_y) / ts.y);
	tile_x		= iFloor(float(size_x) / ts.x);

	if (tile_y < 0) tile_y = 0;
	if (tile_x < 0) tile_x = 0;
	clamp(rem_x, 0, abs(rem_x));
	clamp(rem_y, 0, abs(rem_y));

	m_UIWndFrame.frame[CUIFrameRect::fmR].SetTile(tile_x, tile_y, rem_x, rem_y);

	if (max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosX(), parentAbsR.left) == parentAbsR.left)
		m_UIWndFrame.frame[CUIFrameRect::fmR].SetReverseRem(true, false);
	else
		m_UIWndFrame.frame[CUIFrameRect::fmR].SetReverseRem(false, false);

	m_UIWndFrame.frame[CUIFrameRect::fmR].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosX(), parentAbsR.left),
			max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosY(), parentAbsR.top));

	// fmRB
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY();
	r.x2 = min(ourAbsR.right, parentAbsR.right) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX(), parentAbsR.left) + r.x1;
	r.y2 = min(ourAbsR.bottom, parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRB].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRB].SetTile(1, 1, 0, 0);

	// fmB
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmB].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.left);
	size_y		= min(ourAbsR.bottom, parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top);
	rem_x		= size_x % ts.x;
	rem_y		= size_y % ts.y;
	tile_x		= iFloor(float(size_x) / ts.x);
	tile_y		= iFloor(float(size_y) / ts.y);

	ClampMax_Zero(r);
	if (tile_x < 0) tile_x = 0;
	if (tile_y < 0) tile_y = 0;
	clamp(rem_x, 0, abs(rem_x));
	clamp(rem_y, 0, abs(rem_y));

	m_UIWndFrame.frame[CUIFrameRect::fmB].SetTile	(tile_x, tile_y, rem_x, rem_y);

	if (max(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosY(), parentAbsR.top) == parentAbsR.top)
		m_UIWndFrame.frame[CUIFrameRect::fmB].SetReverseRem(false, true);
	else
		m_UIWndFrame.frame[CUIFrameRect::fmB].SetReverseRem(false, false);

	m_UIWndFrame.frame[CUIFrameRect::fmB].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.left),
		max(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosY(), parentAbsR.top));

	// fmLB
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosY();
	r.x2 = min(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.right) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosX(), parentAbsR.left) + r.x1;
	r.y2 = min(ourAbsR.bottom, parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLB].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLB].SetTile(1, 1, 0, 0);

	// fmL
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmL].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_y		= min(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosY(), parentAbsR.top);
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosX(), parentAbsR.left);
	rem_x		= size_x % ts.x;
	rem_y		= size_y % ts.y;
	tile_y		= iFloor(float(size_y) / ts.y);
	tile_x		= iFloor(float(size_x) / ts.y);

	if (tile_y < 0) tile_y = 0;
	if (tile_x < 0) tile_x = 0;
	clamp(rem_x, 0, abs(rem_x));
	clamp(rem_y, 0, abs(rem_y));

	m_UIWndFrame.frame[CUIFrameRect::fmL].SetTile	(tile_x, tile_y, rem_x, rem_y);

	if (max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosX(), parentAbsR.left) == parentAbsR.left)
		m_UIWndFrame.frame[CUIFrameRect::fmL].SetReverseRem(true, false);
	else
		m_UIWndFrame.frame[CUIFrameRect::fmL].SetReverseRem(false, false);

	m_UIWndFrame.frame[CUIFrameRect::fmL].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosX(), parentAbsR.left),
		max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosY(), parentAbsR.top));

	// fmLT
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosY();
	r.x2 = min(m_UIWndFrame.back.GetPosX(), parentAbsR.right) -
		   max(ourAbsR.left, parentAbsR.left) + r.x1;
	r.y2 = min(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosY(), parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLT].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLT].SetTile(1, 1, 0, 0);

	// fmT
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmT].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosX(), parentAbsR.left);
	size_y		= min(m_UIWndFrame.back.GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosY(), parentAbsR.top);
	rem_x		= size_x % ts.x;
	rem_y		= size_y % ts.y;
	tile_x		= iFloor(float(size_x) / ts.x);
	tile_y		= iFloor(float(size_y) / ts.y);

	if (tile_x < 0 || tile_y < 0)
	{
		rem_x = rem_y = tile_x = tile_y = 0;
	}

	m_UIWndFrame.frame[CUIFrameRect::fmT].SetTile	(tile_x, tile_y, rem_x, rem_y);

	if (max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosY(), parentAbsR.top) == parentAbsR.top)
		m_UIWndFrame.frame[CUIFrameRect::fmT].SetReverseRem(false, true);
	else
		m_UIWndFrame.frame[CUIFrameRect::fmT].SetReverseRem(false, false);

	m_UIWndFrame.frame[CUIFrameRect::fmT].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosX(), parentAbsR.left),
		max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosY(), parentAbsR.top));

	// back
	RCache.set_Shader(m_UIWndFrame.back.GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.back.GetPosX(), parentAbsR.left);
	size_y		= min(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.back.GetPosY(), parentAbsR.top);
	rem_x		= size_x % ts.x;
	rem_y		= size_y % ts.y;
	tile_x		= iFloor(float(size_x) / ts.x);
	tile_y		= iFloor(float(size_y) / ts.y);

	if (tile_y < 0 || tile_x < 0)
	{
		m_UIWndFrame.back.SetRect(null);
		m_UIWndFrame.back.SetTile(0, 0, 0, 0);
		m_UIWndFrame.back.SetPos(0, 0);
	}
	else
	{
		m_UIWndFrame.back.SetRect(0, 0, ts.x, ts.y);
		m_UIWndFrame.back.SetTile(tile_x,tile_y,rem_x,rem_y);

		m_UIWndFrame.back.SetPos(max(m_UIWndFrame.back.GetPosX(), parentAbsR.left),
			max(m_UIWndFrame.back.GetPosY(), parentAbsR.top));
	}
}

//////////////////////////////////////////////////////////////////////////

inline void CUIFrameWindow::ClampMax_Zero(Irect &r)
{
	clamp(r.x1, 0, abs(r.x1));
	clamp(r.x2, 0, abs(r.x2));
	clamp(r.y1, 0, abs(r.y1));
	clamp(r.y2, 0, abs(r.y2));
}