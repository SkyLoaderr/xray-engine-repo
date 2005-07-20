// CUIMapWnd.cpp:  диалог интерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../HUDManager.h"
#include "../actor.h"
#include "../pda.h"
#include "../infoportion.h"
#include "../string_table.h"
#include "UIPdaWnd.h"
#include "UIXmlInit.h"
#include "UIMapWnd.h"
#include "../alife_registry_wrappers.h"
#include "../game_graph.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "../level.h"
#include "../map_manager.h"
#include "../map_spot.h"
#include "UIInventoryUtilities.h"
#include "../LevelFogOfWar.h"
#include "../map_location.h"
#include "../gameTaskmanager.h" //remove me !!!
#include "../gameTask.h" //remove me !!!

#include "UIScrollBar.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIMapWndActions.h"
#include "UIMapWndActionsSpace.h"
#include <dinput.h>
#include "../../xr_input.h"

const				SCROLLBARS_SHIFT			= 5;
const				VSCROLLBAR_STEP				= 20; // В пикселях
const				HSCROLLBAR_STEP				= 20; // В пикселях

const u32			activeLocalMapColor			= 0xffc80000;
const u32			inactiveLocalMapColor		= 0xff438cd1;
const u32			ourLevelMapColor			= 0xffffffff;



CUICustomMap::CUICustomMap ()
{
	m_BoundRect.set			(0,0,0,0);
	SetWindowName			("map");
	m_flags.zero			();
}

CUICustomMap::~CUICustomMap ()
{
}

void CUICustomMap::Update()
{
	CUIStatic::Update();
	UpdateSpots();
}


void CUICustomMap::Init	(shared_str name, CInifile& gameLtx, LPCSTR sh_name)
{

	m_name				= name;
	LPCSTR tex;
	Fvector4 tmp;
	if( gameLtx.line_exist(m_name,"texture") ){
		tex			= gameLtx.r_string(m_name,"texture");
		tmp		= gameLtx.r_fvector4(m_name,"bound_rect");
	}else{
		tex = "ui\\ui_nomap2";
		tmp.set(-10000.0f,-10000.0f,10000.0f,10000.0f);
	}
	m_BoundRect.set		(tmp.x, tmp.y, tmp.z, tmp.w);
	CUIStatic::InitEx	(tex, sh_name, 0, 0, m_BoundRect.width(), m_BoundRect.height() );
	
	SetStretchTexture	(true);
	ClipperOn			();
//	SetWindowName(*m_name);
}


void CUICustomMap::MoveWndDelta(const Fvector2& d)
{
	CUIWindow::MoveWndDelta	(d);
/*
	Frect clip			= GetClipperRect();
	Frect r				= GetWndRect();
	if (r.x2<clip.width())	r.x1 += clip.width()-r.x2;
	if (r.y2<clip.height())	r.y1 += clip.height()-r.y2;
	if (r.x1>0.0f)			r.x1 = 0.0f;
	if (r.y1>0.0f)			r.y1 = 0.0f;
	SetWndPos				(r.x1, r.y1);
*/

}

void rotation_(float x, float y, const float angle, float& x_, float& y_)
{
	float _sc = _cos(angle);
	float _sn = _sin(angle);
	x_= x*_sc+y*_sn;
	y_= y*_sc-x*_sn;
}

Fvector2 CUICustomMap::ConvertLocalToReal(const Fvector2& src)
{
	Fvector2 res; 
	res.x = m_BoundRect.lt.x + src.x/GetCurrentZoom();
	res.y = m_BoundRect.height() + m_BoundRect.lt.y - src.y/GetCurrentZoom();

	return res;
}

Fvector2 CUICustomMap::ConvertRealToLocal  (const Fvector2& src)// meters->pixels (relatively own left-top pos)
{
	Fvector2 res;
	if( !Heading() ){
		return ConvertRealToLocalNoTransform(src);
	}else
	{
		Fvector2 heading_pivot = GetStaticItem()->GetHeadingPivot();
	
		res = ConvertRealToLocalNoTransform(src);
		res.sub(heading_pivot);
		rotation_(res.x, res.y, GetHeading(), res.x, res.y);
		res.add(heading_pivot);
		return res;
	};
}

Fvector2 CUICustomMap::ConvertRealToLocalNoTransform  (const Fvector2& src)// meters->pixels (relatively own left-top pos)
{
	Fvector2 res;
	res.x = (src.x-m_BoundRect.lt.x) * GetCurrentZoom();
	res.y = (m_BoundRect.height()-(src.y-m_BoundRect.lt.y)) * GetCurrentZoom();

	return res;
}

//position and heading for drawing pointer to src pos
bool CUICustomMap::GetPointerTo(const Fvector2& src, float item_radius, Fvector2& pos, float& heading)
{
	Frect		clip_rect_abs			= GetClipperRect(); //absolute rect coords
	Frect		map_rect_abs			= GetAbsoluteRect();

	Frect		rect;
	BOOL res = rect.intersection(clip_rect_abs, map_rect_abs);
	VERIFY(res);
	
	rect = clip_rect_abs;
	rect.sub(map_rect_abs.lt.x,map_rect_abs.lt.y);

	Fbox2 f_clip_rect_local;
	f_clip_rect_local.set(rect.x1, rect.y1, rect.x2, rect.y2 );

	Fvector2 f_center;
	f_clip_rect_local.getcenter(f_center);

	Fvector2 f_dir, f_src;

	f_src.set(src.x, src.y );
	f_dir.sub(f_center, f_src );
	f_dir.normalize_safe();
	Fvector2 f_intersect_point;
	res = f_clip_rect_local.Pick2(f_src,f_dir,f_intersect_point);
	VERIFY(res);


	heading = -f_dir.getH();

	f_intersect_point.mad(f_intersect_point,f_dir,item_radius );

	pos.set( iFloor(f_intersect_point.x), iFloor(f_intersect_point.y) );
	return true;
}


void CUICustomMap::FitToWidth	(float width)
{
	float k			= m_BoundRect.width()/m_BoundRect.height();
	float w			= width;
	float h			= width/k;
	SetWndRect		(0.0f,0.0f,w,h);
	
}

void CUICustomMap::FitToHeight	(float height)
{
	float k			= m_BoundRect.width()/m_BoundRect.height();
	float h			= height;
	float w			= k*height;
	SetWndRect		(0.0f,0.0f,w,h);
	
}


void CUICustomMap::OptimalFit(const Frect& r)
{
	if ((m_BoundRect.height()/r.height())<(m_BoundRect.width()/r.width()))
		FitToHeight	(r.height());
	else
		FitToWidth	(r.width());

}
// try to positioning clipRect center to vNewPoint
void CUICustomMap::SetActivePoint(const Fvector &vNewPoint)
{
	Fvector2 pos;
	pos.set(vNewPoint.x,vNewPoint.z);
	Frect bound = BoundRect();
	if( FALSE==bound.in(pos) )return;

	Fvector2	pos_on_map		= ConvertRealToLocalNoTransform(pos);
	Frect		map_abs_rect	= GetAbsoluteRect();
	Fvector2	pos_abs;

	pos_abs.set(map_abs_rect.lt);
	pos_abs.add(pos_on_map);

	Frect		clip_abs_rect	= GetClipperRect();
	Fvector2	clip_center;
	clip_abs_rect.getcenter(clip_center);
	clip_center.sub(pos_abs);
	MoveWndDelta				(clip_center);
	SetHeadingPivot				(pos_on_map);
}

LPCSTR	CUICustomMap::GetHint()
{
	Fvector2 cursor_pos = GetUICursor()->GetPos();
	LPCSTR hint = NULL;
	WINDOW_LIST& wl = GetChildWndList();
	WINDOW_LIST::reverse_iterator it = wl.rbegin();
	for(;it!=wl.rend();++it)
		if( (*it)->GetAbsoluteRect().in(cursor_pos) ){
			hint=(*it)->GetHint();
			if(hint)
				break;
		}
	if(NULL==hint)
		hint = *m_name;

	return hint;
}

void CUICustomMap::Draw()
{
	CUIStatic::Draw		();
}

bool CUICustomMap::IsRectVisible(Frect r)
{
	Frect map_visible_rect = GetClipperRect();
	Fvector2 pos = GetAbsolutePos();
	r.add(pos.x,pos.y);

	return !!map_visible_rect.intersected(r);
}

bool CUICustomMap::NeedShowPointer(Frect r)
{
	Frect map_visible_rect = GetClipperRect();
	map_visible_rect.shrink(5,5);
	Fvector2 pos = GetAbsolutePos();
	r.add(pos.x,pos.y);

	return !map_visible_rect.intersected(r);
}

CUIGlobalMap::CUIGlobalMap(CUIMapWnd*	pMapWnd)
{
	m_mapWnd				= pMapWnd;
//	m_MinimizedSize.set		(0,0);
//	m_NormalSize.set		(0,0);
//	m_State					= stNone;
	Show					(false);
}



CUIGlobalMap::~CUIGlobalMap()
{
}

void CUIGlobalMap::Init		(shared_str name, CInifile& gameLtx, LPCSTR sh_name)
{
	inherited::Init(name, gameLtx, sh_name);

}

void	CUIGlobalMap::SendMessage			(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}


void CUIGlobalMap::Update()
{
	inherited::Update();
}

void CUIGlobalMap::Draw					()
{
	inherited::Draw();
}

Fvector2 CUIGlobalMap::ConvertRealToLocal(const Fvector2& src)// pixels->pixels (relatively own left-top pos)
{
	Fvector2 res;
	res.x = (src.x-m_BoundRect.lt.x ) * GetCurrentZoom();
	res.y = (src.y-m_BoundRect.lt.y ) * GetCurrentZoom();
	return res;
}

//////////////////////////////////////////////////////////////////////////

CUILevelMap::CUILevelMap(CUIMapWnd* p)
{
	m_mapWnd			= p;
	m_globalMapSpot		= xr_new<CUIGlobalMapSpot>(this);
	m_anomalies_map		= NULL;
	Show				(false);
}

CUILevelMap::~CUILevelMap()
{
	xr_delete			(m_globalMapSpot);
	xr_delete			(m_anomalies_map);
}

void CUILevelMap::Draw()
{
	inherited::Draw();
/*
	CGameFont* F		= UI()->Font()->pFontDI;
	F->SetAligment		(CGameFont::alCenter);
	F->SetSizeI			(0.02f);
	F->OutSetI			(0.f,-0.8f);
	F->SetColor			(0xffffffff);

	Fvector2 cursor_pos = GetUICursor()->GetPos();
	cursor_pos.sub(GetAbsolutePos());
	Fvector2 pt = ConvertLocalToReal(cursor_pos);
	F->OutNext			("pos on map = %3.2f-%3.2f",pt.x, pt.y);
*/
}

void CUILevelMap::Init	(shared_str name, CInifile& gameLtx, LPCSTR sh_name)
{
	inherited::Init(name, gameLtx, sh_name);
	Fvector4 tmp = gameLtx.r_fvector4(MapName(),"global_rect");
	m_GlobalRect.set(tmp.x, tmp.y, tmp.z, tmp.w);

	float kw = m_GlobalRect.width	()	/	BoundRect().width		();
	float kh = m_GlobalRect.height	()	/	BoundRect().height	();

	if(abs(kw-kh)>0.1){
		Msg("----incorrect global rect definition for map [%s]  kw=%f kh=%f",*MapName(),kw,kh);
	}

	LPCSTR global_map_spot = NULL;
	if( gameLtx.line_exist(name,"texture_global") ){
		global_map_spot			= gameLtx.r_string(name,"texture_global");
	}else
	if( gameLtx.line_exist(name,"texture") ){
		global_map_spot			= gameLtx.r_string(name,"texture");
	}else
	{
		global_map_spot = "ui\\ui_nomap2";
	}
	m_globalMapSpot->Init(inactiveLocalMapColor, global_map_spot);

	if(gameLtx.line_exist(MapName(),"anomalies_texture")){
		LPCSTR texture						= gameLtx.r_string	(MapName(),"anomalies_texture");
		Fvector4 tmp						= gameLtx.r_fvector4(MapName(),"anomalies_texture_rect"); //lt,wh
		Frect rect; rect.set				(tmp.x,tmp.y,tmp.x+tmp.z,tmp.y+tmp.w);
		m_anomalies_map						= xr_new<CUIStatic>();
		m_anomalies_map->Init				(texture,0.0f,0.0f,0.0f,0.0f);
		m_anomalies_map->GetUIStaticItem().SetOriginalRect(rect);
		m_anomalies_map->SetStretchTexture	(true);
		m_anomalies_map->SetAutoDelete		(false);
	}
}



void CUILevelMap::UpdateSpots		()
{
	DetachAll		();
	if(m_anomalies_map){
		m_anomalies_map->SetWndPos	(0.0f,0.0f);
		m_anomalies_map->SetWndSize	(GetWndSize());
		AttachChild					(m_anomalies_map);
	}

	CLevelFogOfWar* F	= Level().FogOfWarMngr().GetFogOfWar(MapName());
	AttachChild		(F);
	Locations& ls =Level().MapManager().Locations();
	for(Locations_it it=ls.begin(); it!=ls.end(); ++it){
		if ((*it).location->Update())
			(*it).location->UpdateLevelMap(this);
	}
}

Frect CUILevelMap::CalcWndRectOnGlobal	()
{
	Frect res;
	CUIGlobalMap*					globalMap = MapWnd()->GlobalMap();

	res.lt							= globalMap->ConvertRealToLocal(GlobalRect().lt);
	res.rb							= globalMap->ConvertRealToLocal(GlobalRect().rb);
	res.add							(globalMap->GetWndPos().x, globalMap->GetWndPos().y);

	return res;
}

void CUILevelMap::CalcOpenRect			(	const Fvector2& destLevelMapCP, 
											const float map_zoom, 
											Frect& lmap_des_rect, 
											Frect& gmap_des_rect)
{
	CUIGlobalMap*					globalMap = MapWnd()->GlobalMap();
	Fvector2						local_map_center_old; 
	Fvector2						local_map_center_new;

	float kk						= GetWndRect().width();
	GetWndRect().getcenter			(local_map_center_old);

	lmap_des_rect.set				(	0.0f, 
										0.0f,
										0.0f+BoundRect().width()*map_zoom,
										0.0f+BoundRect().height()*map_zoom);
	
	Frect tmp_rect					= GetWndRect();
	SetWndRect						(lmap_des_rect);
	Fvector2 ppp					= ConvertRealToLocalNoTransform(destLevelMapCP);
	SetWndRect						(tmp_rect);

	Frect	vis_abs_rect			= m_mapWnd->ActiveMapRect();
	float dw						= vis_abs_rect.width()/2.0f - ppp.x;
	float dh						= vis_abs_rect.height()/2.0f - ppp.y;
	lmap_des_rect.add				(dw,dh);

	lmap_des_rect.getcenter			(local_map_center_new);
	

	Fvector2 global_map_pos_old		= globalMap->GetWndPos();
	Fvector2 global_map_size_old	= globalMap->GetWndSize();
	Fvector2 global_map_size_new	= global_map_size_old;
	global_map_size_new.mul			(lmap_des_rect.width()/kk);

	float kw						= (local_map_center_old.x-global_map_pos_old.x)/global_map_size_old.x;
	float kh						= (local_map_center_old.y-global_map_pos_old.y)/global_map_size_old.y;

	gmap_des_rect.x1				= local_map_center_new.x	- global_map_size_new.x*kw;
	gmap_des_rect.y1				= local_map_center_new.y	- global_map_size_new.y*kh;
	gmap_des_rect.x2				= gmap_des_rect.x1			+ global_map_size_new.x;
	gmap_des_rect.y2				= gmap_des_rect.y1			+ global_map_size_new.y;
}



CUIMiniMap::CUIMiniMap()
{}

CUIMiniMap::~CUIMiniMap()
{}



void CUIMiniMap::Init(shared_str name, CInifile& gameLtx, LPCSTR sh_name)
{
	inherited::Init(name, gameLtx, sh_name);
	CUIStatic::SetColor(0x7fffffff);
}
void CUIMiniMap::MoveWndDelta(const Fvector2& d)
{
	CUIWindow::MoveWndDelta	(d);
}

void CUIMiniMap::UpdateSpots()
{
	DetachAll();
	Locations& ls =Level().MapManager().Locations();
	for(Locations_it it=ls.begin(); it!=ls.end(); ++it){
			(*it).location->UpdateMiniMap(this);
	}

}


///////////////////////////////////////////////////////////////////////////
CUIMapWnd::CUIMapWnd()
{
	m_activeMap				= NULL;
	m_GlobalMap				= NULL;
	m_flags.zero			();
	Show					(false);
}

CUIMapWnd::~CUIMapWnd()
{
	delete_data			(m_ActionPlanner);
	delete_data			(m_GameMaps);
	delete_data			(m_MapText);
}

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	if(!ActiveMap())return;
	ActiveMap()->SetActivePoint(vNewPoint);
	UpdateScroll();
}

void CUIMapWnd::Init(LPCSTR xml_name, LPCSTR start_from)
{
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3						(xml_result, "xml file not found", xml_name);

	string512	pth;
	// load map background
	CUIXmlInit xml_init;
	strconcat(pth,start_from,":main_wnd");
	xml_init.InitWindow				(uiXml, pth, 0, this);



	m_UIMainFrame					= xr_new<CUIFrameWindow>(); m_UIMainFrame->SetAutoDelete(true);
	AttachChild						(m_UIMainFrame);
	strconcat(pth,start_from,":main_wnd:main_map_frame");
	xml_init.InitFrameWindow		(uiXml, pth, 0, m_UIMainFrame);

	m_UILevelFrame					= xr_new<CUIWindow>(); m_UILevelFrame->SetAutoDelete(true);
	strconcat(pth,start_from,":main_wnd:main_map_frame:level_frame");
	xml_init.InitWindow				(uiXml, pth, 0, m_UILevelFrame);
	m_UIMainFrame->AttachChild		(m_UILevelFrame);

	Frect r							= m_UILevelFrame->GetWndRect();

	m_UIMainScrollV					= xr_new<CUIScrollBar>(); m_UIMainScrollV->SetAutoDelete(true);
	m_UIMainScrollV->Init			(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	m_UIMainScrollV->SetWindowName	("scroll_v");
	m_UIMainScrollV->SetStepSize	(_max(1,iFloor(m_UILevelFrame->GetHeight()/10)));
	m_UIMainScrollV->SetPageSize	(iFloor(m_UILevelFrame->GetHeight()));
	m_UIMainFrame->AttachChild		(m_UIMainScrollV);
	Register						(m_UIMainScrollV);
	AddCallback						("scroll_v",SCROLLBAR_VSCROLL,boost::bind(&CUIMapWnd::OnScrollV,this));

	m_UIMainScrollH					= xr_new<CUIScrollBar>(); m_UIMainScrollH->SetAutoDelete(true);
	m_UIMainScrollH->Init			(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	m_UIMainScrollH->SetWindowName	("scroll_h");
	m_UIMainScrollH->SetStepSize	(_max(1,iFloor(m_UILevelFrame->GetWidth()/10)));
	m_UIMainScrollH->SetPageSize	(iFloor(m_UILevelFrame->GetWidth()));
	m_UIMainFrame->AttachChild		(m_UIMainScrollH);
	Register						(m_UIMainScrollH);

	AddCallback						("scroll_h",SCROLLBAR_HSCROLL,boost::bind(&CUIMapWnd::OnScrollH,this));
	AddCallback						("map",SCROLLBAR_NEEDUPDATE,boost::bind(&CUIMapWnd::UpdateScroll,this));
	
	UIMainMapHeader					= xr_new<CUIFrameLineWnd>(); UIMainMapHeader->SetAutoDelete(true);
	m_UIMainFrame->AttachChild		(UIMainMapHeader);
	strconcat(pth,start_from,":main_wnd:map_header_frame_line");
	xml_init.InitFrameLine			(uiXml, pth, 0, UIMainMapHeader);


// Load maps
	string256								gameLtxPath;
	FS.update_path							(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile gameLtx						(gameLtxPath);

	m_GlobalMap								= xr_new<CUIGlobalMap>(this);
	m_GlobalMap->SetAutoDelete				(true);
	m_GlobalMap->Init						("global_map",gameLtx,"hud\\default");

	m_UILevelFrame->AttachChild				(m_GlobalMap);

	m_MapText								= xr_new<CUIStatic>();
	m_MapText->SetFont						(UI()->Font()->pFontGraffiti22Russian);
	m_MapText->SetWndSize					(Fvector2().set(100.0f, 22.0f));

	// initialize local maps
	xr_string sect_name;
	if(GameID()==GAME_SINGLE)
		sect_name = "level_maps_single";
	else
		sect_name = "level_maps_mp";

	if (gameLtx.section_exist(sect_name.c_str())){
		CInifile::Sect& S		= gameLtx.r_section(sect_name.c_str());
		CInifile::SectIt	it	= S.begin(), end = S.end();
		for (;it!=end; it++){
			shared_str map_name = it->first;
			xr_strlwr(map_name);
			R_ASSERT2	(m_GameMaps.end() == m_GameMaps.find(map_name), "Duplicate level name not allowed");
			
			CUICustomMap*& l = m_GameMaps[map_name];

			l = xr_new<CUILevelMap>(this);
			
			l->Init(map_name, gameLtx, "hud\\set");

			l->OptimalFit( m_UILevelFrame->GetWndRect() );
			
//			if(l->GlobalMapSpot())
//				m_GlobalMap->AttachChild(l->GlobalMapSpot() );
		}
	}

	Register						(m_GlobalMap);
	m_ActionPlanner					= xr_new<CMapActionPlanner>();
	m_ActionPlanner->setup			(this);
	SetActiveMap					(m_GlobalMap);
}

void CUIMapWnd::Show(bool status)
{
	if (status)
	{
		m_GlobalMap->Show	(true);
		GameMaps::iterator	it = m_GameMaps.begin();
		for(;it!=m_GameMaps.end();++it){
			if(it->second->GlobalMapSpot())
				m_GlobalMap->AttachChild	(it->second->GlobalMapSpot());
		}

		if(ActiveMap()==NULL)
			SetActiveMap		(m_GlobalMap->MapName());
//		if(m_activeLevelMap==NULL)
//			SetActiveMap				(Level().name());
//		SetActivePoint				( Level().CurrentEntity()->Position() );
		InventoryUtilities::SendInfoToActor("ui_pda_map_local");
	}else{
		if(GlobalMap()){
			GlobalMap()->DetachAll();
			GlobalMap()->Show(false);
		}
		GameMaps::iterator	it = m_GameMaps.begin();
		for(;it!=m_GameMaps.end();++it)
			it->second->DetachAll();
	}

	inherited::Show(status);
}


void CUIMapWnd::AddMapToRender			(CUICustomMap* m)
{
	Register							( smart_cast<CUIWindow*>(m) );
//	if(!m_UILevelFrame->IsChild(smart_cast<CUIWindow*>(m)))//fix it!!!
	m_UILevelFrame->AttachChild			( smart_cast<CUIWindow*>(m) );
	m->Show								( true );
	m_UILevelFrame->BringToTop			( smart_cast<CUIWindow*>(m) );
	m->SetClipRect						( ActiveMapRect() );
}

void CUIMapWnd::RemoveMapToRender		(CUICustomMap* m)
{
	if( m!=GlobalMap() )
		m_UILevelFrame->DetachChild			(smart_cast<CUIWindow*>(m));
}

void CUIMapWnd::SetActiveMap(CUICustomMap* m)
{
	m_activeMap							=	m;
	m_UILevelFrame->BringToTop			( smart_cast<CUIWindow*>(m) );
	m->SetClipRect						( ActiveMapRect() );
	UpdateScroll						();
}

void CUIMapWnd::SetActiveMap			(shared_str level_name, Fvector2 pos)
{
	u16 map_idx;
	if(level_name == m_GlobalMap->MapName())
		map_idx							= GLOBAL_MAP_IDX;
	else{
		map_idx							= GetIdxByName(level_name);
		CUILevelMap* lm					= smart_cast<CUILevelMap*>(GetMapByIdx(map_idx));
		lm->TargetCenter()				= pos;
	}

	m_ActionPlanner->m_storage.set_property(1, false);
	CWorldState							target_state;
	target_state.add_condition			(CWorldProperty(UIMapWndActionsSpace::make_map_id(map_idx,UIMapWndActionsSpace::ePropMapOpenedIdle),true));
	m_ActionPlanner->set_target_state	(target_state);

}

void CUIMapWnd::SetActiveMap			(shared_str level_name)
{
	u16 map_idx;
	if(level_name == m_GlobalMap->MapName())
		map_idx							= GLOBAL_MAP_IDX;
	else{
		map_idx							= GetIdxByName(level_name);
		CUILevelMap* lm					= smart_cast<CUILevelMap*>(GetMapByIdx(map_idx));
		Frect r							= lm->BoundRect();
		r.getcenter						(lm->TargetCenter());
	}

	m_ActionPlanner->m_storage.set_property(1, false);
	CWorldState							target_state;
	target_state.add_condition			(CWorldProperty(UIMapWndActionsSpace::make_map_id(map_idx,UIMapWndActionsSpace::ePropMapOpenedIdle),true));
	m_ActionPlanner->set_target_state	(target_state);
//	GetUICursor()->HoldMode(false);
//	m_flags.set(lmMouseHold,FALSE);

}


void CUIMapWnd::Draw()
{
	if(ActiveMap())
		ActiveMap()->SetClipRect( ActiveMapRect() );

	inherited::Draw();
	m_MapText->SetWndPos					(ActiveMapRect().lt);
	m_MapText->Draw();
}

bool CUIMapWnd::OnKeyboard				(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_A == dik)
	{
		if(ActiveMap() && ActiveMap()!=GlobalMap())
			AddUserMapSpot();
		return true;
	}
	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_G == dik)
	{
		SetActiveMap(GlobalMap()->MapName());
		return true;
	}
	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_N == dik)
	{
		u16 curr_map_idx = 0;
		if(GlobalMap()!=ActiveMap()){
			curr_map_idx = GetIdxByName(ActiveMap()->MapName());
			++curr_map_idx;
			if(curr_map_idx == (u16)GameMaps().size()) curr_map_idx = 0;
		}
		SetActiveMap(GetMapByIdx(curr_map_idx)->MapName());
		return true;
	}
	return inherited::OnKeyboard	(dik, keyboard_action);
}

void CUIMapWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
	Fvector2 cursor_pos = GetUICursor()->GetPos();

	if(ActiveMap() && !ActiveMap()->Locked() && ActiveMapRect().in( cursor_pos ) ){
		switch (mouse_action){
		case WINDOW_MOUSE_MOVE:
			if(/*m_flags.test(lmMouseHold)*/ pInput->iGetAsyncBtnState(0)){
				ActiveMap()->MoveWndDelta	(GetUICursor()->GetPosDelta());
				UpdateScroll					();
			}
		break;
		case WINDOW_LBUTTON_DOWN:
			if(m_flags.test(lmUserSpotAdd) ){
				Fvector2 cp = cursor_pos;
				cp.sub(ActiveMap()->GetAbsolutePos());
				Fvector2 p = ActiveMap()->ConvertLocalToReal(cp);
				Fvector pos;
				pos.set(p.x, 0.0f, p.y);
				CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
				shared_str spot = "user";
				CMapLocation* ml = Level().MapManager().AddUserLocation(spot, ActiveMap()->MapName(), pos);
				CGameTask* t = pActor->GameTaskManager().GiveGameTaskToActor("user_task",false);
				t->m_Objectives[0].task_state	= eTaskUserDefined;
				t->m_Objectives[1].task_state	= eTaskUserDefined;
				t->m_Objectives[1].object_id	= ml->ObjectID();
				t->m_Objectives[1].map_location	= spot;
				m_flags.set(lmUserSpotAdd, FALSE);
				break;
			}
//			GetUICursor()->HoldMode(true);
//			m_flags.set(lmMouseHold,TRUE);
		break;
		case WINDOW_LBUTTON_UP:
//			GetUICursor()->HoldMode(false);
//			m_flags.set(lmMouseHold,FALSE);
		break;
		case WINDOW_MOUSE_WHEEL_UP:
			m_UIMainScrollV->TryScrollDec();
		break;
		case WINDOW_MOUSE_WHEEL_DOWN:
			m_UIMainScrollV->TryScrollInc();
		break;


		}	
	};
}

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

CUICustomMap*	CUIMapWnd::GetMapByIdx				(u16 idx)
{
	GameMapsPairIt it				= m_GameMaps.begin();
	std::advance(it, idx);
	return it->second;
}

u16 CUIMapWnd::GetIdxByName			(const shared_str& map_name)
{
	GameMapsPairIt it				= m_GameMaps.find(map_name);
	return (u16)std::distance		(m_GameMaps.begin(),it);
}

void CUIMapWnd::UpdateScroll()
{
	Fvector2 w_pos					= ActiveMap()->GetWndPos();
	m_UIMainScrollV->SetRange		(0,iFloor(ActiveMap()->GetHeight()));
	m_UIMainScrollH->SetRange		(0,iFloor(ActiveMap()->GetWidth()));

	m_UIMainScrollV->SetScrollPos	(iFloor(-w_pos.y));
	m_UIMainScrollH->SetScrollPos	(iFloor(-w_pos.x));
}


void CUIMapWnd::OnScrollV()
{
	if (ActiveMap()){
		int s_pos					= m_UIMainScrollV->GetScrollPos();
		Fvector2 w_pos				= ActiveMap()->GetWndPos();
		ActiveMap()->SetWndPos	(w_pos.x,float(-s_pos));
	}
}
void CUIMapWnd::OnScrollH()
{
	if (ActiveMap()){
		int s_pos					= m_UIMainScrollH->GetScrollPos();
		Fvector2 w_pos				= ActiveMap()->GetWndPos();
		ActiveMap()->SetWndPos	(float(-s_pos),w_pos.y);
	}
}

void CUIMapWnd::SetStatusInfo			(LPCSTR status_text)
{
//	UIMainMapHeader->UITitleText.SetText(status_text);
	m_MapText->SetText					(status_text);
}

void CUIMapWnd::Update()
{
	inherited::Update			();
	ShowHint					();
	m_ActionPlanner->update		();
}

void CUIMapWnd::ShowHint()
{
/*	Fvector2 cursor_pos = GetUICursor()->GetPos();
	LPCSTR hint = NULL;

	if( ActiveMap()==m_GlobalMap && m_GlobalMap->GetAbsoluteRect().in(cursor_pos) ){
		hint = m_GlobalMap->GetHint();
	}else
		if( ActiveMap() && ActiveMap()->GetAbsoluteRect().in(cursor_pos) ){
			hint = ActiveMap()->GetHint();
	}*/
	LPCSTR hint = NULL;
	if(ActiveMap())
		hint = *(ActiveMap()->MapName());
	SetStatusInfo(hint);
}

void CUIMapWnd::AddUserMapSpot			()
{
	m_flags.set(lmUserSpotAdd,TRUE);
}


CUIGlobalMapSpot::CUIGlobalMapSpot		(CUICustomMap* m)
{
	m_owner_map = smart_cast<CUILevelMap*>(m);
}

CUIGlobalMapSpot::~CUIGlobalMapSpot		()
{}


void CUIGlobalMapSpot::Draw		()
{
	if(m_owner_map->Locked()) return;
	inherited::Draw(	);
}

void CUIGlobalMapSpot::Init	(u32 color, LPCSTR tex_name)
{
	inherited::Init				(0.0f,0.0f,20.0f,20.0f);
//	UIBorder.SetVisiblePart(CUIFrameRect::fmBK,FALSE);
//	UIBorder.Init("ui_frame_very_small",0.0f,0.0f,20.0f,20.0f);
	UIBorder					= xr_new<CUIStatic>(); UIBorder->SetAutoDelete(true);
	UIBorder->Init				(tex_name,0.0f,0.0f,20.0f,20.0f);
	UIBorder->SetStretchTexture	(true);
	UIBorder->ClipperOn			();

	UIBorder->SetColor			(color);
	AttachChild					(UIBorder);
}

void CUIGlobalMapSpot::OnMouse	(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
		switch (mouse_action){
			case WINDOW_LBUTTON_DOWN:{
					CUIGlobalMap* globalMap = (CUIGlobalMap*)GetParent();
					globalMap->MapWnd()->SetActiveMap( m_owner_map->MapName() );
				}break;
		}
}

void CUIGlobalMapSpot::Update()
{
	if(m_owner_map->Locked()) return;
	//rect, pos
	CUIGlobalMap* w = (CUIGlobalMap*)GetParent();
	Frect rect;
	Fvector2 tmp;

	tmp = w->ConvertRealToLocal(m_owner_map->GlobalRect().lt);
	rect.lt = tmp;

	tmp = w->ConvertRealToLocal(m_owner_map->GlobalRect().rb);
	rect.rb = tmp;
	
	SetWndRect				(rect);
	UIBorder->SetWndPos		(0.0f,0.0f);
	UIBorder->SetWidth		(rect.width());
	UIBorder->SetHeight		(rect.height());

	//highlight border
	if(!w->Locked()){
		if(w->MapWnd()->ActiveMap()==m_owner_map)
			UIBorder->SetColor(ourLevelMapColor);
		else{
			Fvector2 cursor_pos = GetUICursor()->GetPos();
			if(GetAbsoluteRect().in( cursor_pos ) )
					UIBorder->SetColor(activeLocalMapColor);
			else
					UIBorder->SetColor(inactiveLocalMapColor);
		};
	}
}
LPCSTR CUIGlobalMapSpot::GetHint()
{
	return *m_owner_map->MapName();
}
