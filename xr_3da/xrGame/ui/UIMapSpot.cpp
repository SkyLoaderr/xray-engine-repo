// UIMapSpot.cpp:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapSpot.h"

#include "../actor.h"

CUIMapSpot::CUIMapSpot()
{
	m_pObject = NULL;
	m_vWorldPos.set(0,0,0);

	m_eAlign = eNone;

	m_sDescText.SetText("");
	m_sNameText.SetText("");

	m_fHeading = 0.f;
	m_bHeading = false;

	ClipperOn();
}
CUIMapSpot::~CUIMapSpot()
{
}

void CUIMapSpot::Draw()
{
	if(dynamic_cast<CActor*>(this->m_pObject))
	{
		int a =0;
		a++;
	}
	if(m_bHeading)
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
