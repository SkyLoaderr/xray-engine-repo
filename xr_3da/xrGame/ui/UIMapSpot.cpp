// UIMapSpot.cpp:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapSpot.h"

CUIMapSpot::CUIMapSpot()
{
	m_pObject = NULL;
	m_vWorldPos.set(0,0,0);

	m_bCenter = false;
	m_sDescText.SetText("");
	m_sNameText.SetText("");

	m_fHeading = 0.f;

	ClipperOn();
}
CUIMapSpot::~CUIMapSpot()
{
}

void CUIMapSpot::Draw()
{
	if(m_fHeading != 0.f)
	{
		RECT rect = GetAbsoluteRect();
		if(m_bClipper) TextureClipper();
		m_UIStaticItem.SetPos(rect.left, rect.top);
		m_UIStaticItem.Render(m_fHeading);
	}
	else
		CUIStatic::Draw();
}

void CUIMapSpot::Update()
{
	CUIStatic::Update();
}
