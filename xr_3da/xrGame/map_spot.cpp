#include "stdafx.h"
#include "map_spot.h"
#include "map_location.h"
#include "ui/UIXmlInit.h"
#include "ui/UIMApWnd.h"

CMapSpot::CMapSpot(CMapLocation* ml)
:m_map_location(ml)
{
	ClipperOn();
}

CMapSpot::~CMapSpot()
{
}

void CMapSpot::Load(CUIXml* xml, LPCSTR path)
{
	CUIXmlInit::InitStatic(*xml,path,0,this);
	
}

LPCSTR CMapSpot::GetHint() 
{
	return MapLocation()->GetHint();
};

void CMapSpot::Update()
{
	inherited::Update();
}

void CMapSpot::Draw()
{
	inherited::Draw();
}


CMapSpotPointer::CMapSpotPointer(CMapLocation* ml)
:inherited(ml)
{
	ClipperOn();
}

CMapSpotPointer::~CMapSpotPointer()
{
}

void CMapSpotPointer::Draw()
{
	inherited::Draw();
}

void CMapSpotPointer::Update()
{
	inherited::Update();
}

LPCSTR CMapSpotPointer::GetHint()
{
	m_pointer_hint = "to: ";
	m_pointer_hint += inherited::GetHint();
	return m_pointer_hint.c_str();
}
/*
//////////////////////////////////////////////////
CZoneMapSpot::CZoneMapSpot(CMapLocation* ml)
:inherited(ml)
{
	ClipperOn();
}

CZoneMapSpot::~CZoneMapSpot()
{
}

void CZoneMapSpot::Draw()
{
	inherited::Draw();
}

void CZoneMapSpot::Update()
{
	inherited::Update();
}
*/