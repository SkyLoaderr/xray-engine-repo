#include "stdafx.h"
#include "map_spot.h"
#include "map_location.h"
#include "ui/UIXmlInit.h"
#include "ui/UIMApWnd.h"

CMapSpot::CMapSpot(CMapLocation* ml)
:m_map_location(ml)
{
	m_flags.zero();
}

CMapSpot::~CMapSpot()
{}

void CMapSpot::Load(CUIXml* xml, LPCSTR path)
{
	CUIXmlInit::InitStatic(*xml,path,0,this);
	
	xml->NavigateToNode(path,0);
	int heading_flag = xml->ReadAttribInt(path, 0, "heading");
	GetUIStaticItem().SetAlign(alCenter);
	if(heading_flag){ 
		m_flags.set(eHeading,TRUE);	
	}
}
LPCSTR CMapSpot::GetHint() 
{
	return MapLocation()->GetHint();
};
void CMapSpot::Update()
{
	inherited::Update();

	CUICustomMap* map = (CUICustomMap*)GetParent();
	Fvector2 pos_global = MapLocation()->Position();

	Ivector2 pos_on_parent_map =	map->ConvertRealToLocal(pos_global);

	SetWndPos(pos_on_parent_map.x, pos_on_parent_map.y);

	SetClipRect( map->GetClipperRect() );

	EnableHeading( !!m_flags.test(eHeading) );
	if( m_flags.test(eHeading) ){
		Fvector2 dir_global = MapLocation()->Direction();
		float h = dir_global.getH();
		float h_ = map->GetHeading()+h;
		SetHeading( h_ );
	}

}

//////////////////////////////////////////////////

CLevelMapSpot::CLevelMapSpot(CMapLocation* ml)
:inherited(ml)
{
	ClipperOn();
}

CLevelMapSpot::~CLevelMapSpot()
{
}

void CLevelMapSpot::Draw()
{
	inherited::Draw();
}


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
