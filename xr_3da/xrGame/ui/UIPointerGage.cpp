//////////////////////////////////////////////////////////////////////////
// UIPointerGage.cpp:			�������� ������
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIPointerGage.h"



CUIPointerGage::CUIPointerGage	()
{
	m_fAngle = m_fAngleMax = m_fAngleMin = 0.f;
}
CUIPointerGage::~CUIPointerGage	()
{
}

void CUIPointerGage::Init    (LPCSTR tex_name, 
								 int x, int y, 
								 int width, int height)
{
	inherited::Init(tex_name, x , y, width, height);
}


void CUIPointerGage::InitPointer(LPCSTR arrow_tex_name,  int arrow_offset_x , int arrow_offset_y,
								 float angle_min, float angle_max)
{
	m_iArrowOffsetX = GetWidth()/2 + arrow_offset_x;
	m_iArrowOffsetY = GetHeight()/2 + arrow_offset_y;
	m_ArrowPointer.Init(arrow_tex_name,	"hud\\default", 0, 0, alNone);

	m_fAngleMin = angle_min;
	m_fAngleMax = angle_max;
	R_ASSERT(m_fAngleMin<m_fAngleMax);
}

void CUIPointerGage::Update		()
{
	inherited::Update();
}

void CUIPointerGage::Draw		()
{
	inherited::Draw();
	m_ArrowPointer.SetPos(GetAbsoluteRect().left + m_iArrowOffsetX, 
							GetAbsoluteRect().top + m_iArrowOffsetY);
	m_ArrowPointer.Render(m_fAngle);
}


void CUIPointerGage::SetValue	(float value)
{
	m_fAngle = m_fAngleMin + (m_fAngleMax - m_fAngleMin)*value;
}