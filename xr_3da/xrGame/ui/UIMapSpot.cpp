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

	ClipperOn();
}
CUIMapSpot::~CUIMapSpot()
{
}


void CUIMapSpot::Draw()
{
	CUIStatic::Draw();
}

void CUIMapSpot::Update()
{
	CUIStatic::Update();
}
