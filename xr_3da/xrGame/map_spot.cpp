#include "stdafx.h"
#include "map_spot.h"
#include "map_location.h"
#include "ui/UIXmlInit.h"
#include "ui/UIMApWnd.h"
#include "level.h"
#include "../xr_object.h"

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

//////////////////////////////////////////////////
CMiniMapSpot::CMiniMapSpot(CMapLocation* ml)
:inherited(ml)
{
}

CMiniMapSpot::~CMiniMapSpot()
{
}

void CMiniMapSpot::Load(CUIXml* xml, LPCSTR path)
{
	inherited::Load(xml,path);

	string256 buf;
	XML_NODE* n = NULL;
	
	Irect base_rect = GetUIStaticItem().GetOriginalRect();

	strconcat(buf, path, ":texture_above");
	n = xml->NavigateToNode(buf,0);
	if(n){
		LPCSTR texture  = xml->Read(buf, 0, NULL);
		int x				= xml->ReadAttribInt(buf, 0, "x", base_rect.x1);
		int y				= xml->ReadAttribInt(buf, 0, "y", base_rect.y1);
		int width			= xml->ReadAttribInt(buf, 0, "width", base_rect.width());
		int height			= xml->ReadAttribInt(buf, 0, "height", base_rect.height());
		
		m_icon_above.create("hud\\default",texture);
		m_tex_rect_above.set(x,y,width,height);
	}

	strconcat(buf, path, ":texture_below");
	n = xml->NavigateToNode(buf,0);
	if(n){
		LPCSTR texture  = xml->Read(buf, 0, NULL);
		int x				= xml->ReadAttribInt(buf, 0, "x", base_rect.x1);
		int y				= xml->ReadAttribInt(buf, 0, "y", base_rect.y1);
		int width			= xml->ReadAttribInt(buf, 0, "width", base_rect.width());
		int height			= xml->ReadAttribInt(buf, 0, "height", base_rect.height());
		
		m_icon_below.create("hud\\default",texture);
		m_tex_rect_below.set(x,y,width,height);
	}
	strconcat(buf, path, ":texture");
	n = xml->NavigateToNode(buf,0);
	if(n){
		LPCSTR texture  = xml->Read(buf, 0, NULL);
		int x				= xml->ReadAttribInt(buf, 0, "x", base_rect.x1);
		int y				= xml->ReadAttribInt(buf, 0, "y", base_rect.y1);
		int width			= xml->ReadAttribInt(buf, 0, "width", base_rect.width());
		int height			= xml->ReadAttribInt(buf, 0, "height", base_rect.height());
		
		m_icon_normal.create("hud\\default",texture);
		m_tex_rect_normal.set(x,y,width,height);
	}

}

void CMiniMapSpot::Draw()
{
	CObject* O = Level().CurrentViewEntity();
	if(O&&m_icon_above&&m_icon_below){
		float ml_y = MapLocation()->GetLastPosition().y;
		float d = O->Position().y-ml_y;

		if(d>1.8f){
			GetUIStaticItem().SetShader(m_icon_below);
			GetUIStaticItem().SetOriginalRect(m_tex_rect_below.x1,m_tex_rect_below.y1,m_tex_rect_below.x2,m_tex_rect_below.y2);
		}else
		if(d<-1.8f){
			GetUIStaticItem().SetShader(m_icon_above);
			GetUIStaticItem().SetOriginalRect(m_tex_rect_above.x1,m_tex_rect_above.y1,m_tex_rect_above.x2,m_tex_rect_above.y2);
		}else{
			GetUIStaticItem().SetShader(m_icon_normal);
			GetUIStaticItem().SetOriginalRect(m_tex_rect_normal.x1,m_tex_rect_normal.y1,m_tex_rect_normal.x2,m_tex_rect_normal.y2);
		}
	};

	inherited::Draw();
}

void CMiniMapSpot::Update()
{
	inherited::Update();
}
