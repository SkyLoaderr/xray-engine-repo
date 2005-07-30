#include "stdafx.h"
#include "../level.h"
#include "../LevelFogOfWar.h"
#include "../map_location.h"
#include "../map_manager.h"
#include "../MainUI.h"
#include "UIMap.h"
#include "UIMapWnd.h"

const u32			activeLocalMapColor			= 0xffffffff;//0xffc80000;
const u32			inactiveLocalMapColor		= 0xffffffff;//0xff438cd1;
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
	if(!Locked())
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
	}else{
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
	m_minZoom				= 1.f;
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
/*	if (!fsimilar(GetCurrentZoom(),m_mapWnd->GetZoom(),EPS_L)){ 
		float new_zoom	= GetCurrentZoom()+(m_mapWnd->GetZoom()-GetCurrentZoom())*1.f*Device.fTimeDelta;
		Fvector2 np		= {0.f,0.f};
		Frect tgt_rect;
		tgt_rect.set	(np.x,np.y, np.x+BoundRect().width()*new_zoom,np.y+BoundRect().height()*new_zoom);
		SetWndRect		(tgt_rect);
//		SetWidth		(BoundRect().width()*new_zoom);
//		SetHeight		(BoundRect().height()*new_zoom);
		m_mapWnd->UpdateScroll();
	}*/
}

void CUIGlobalMap::Draw					()
{
	inherited::Draw();
}

Fvector2 CUIGlobalMap::ConvertRealToLocal(const Fvector2& src)// pixels->pixels (relatively own left-top pos)
{
	Fvector2 res;
	res.x = (src.x-m_BoundRect.lt.x) * GetCurrentZoom();
	res.y = (src.y-m_BoundRect.lt.y) * GetCurrentZoom();
	return res;
}

void CUIGlobalMap::MoveWndDelta(const Fvector2& d)
{
	inherited::MoveWndDelta	(d);
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

void CUIGlobalMap::CalcOpenRect         (const Fvector2& center_point, Frect& map_desired_rect, float tgt_zoom)
{
        Fvector2                                new_center_pt;
        map_desired_rect.set    (0.0f,0.0f, BoundRect().width()*tgt_zoom,BoundRect().height()*tgt_zoom);
        float d_zoom                    = tgt_zoom/GetCurrentZoom();
        new_center_pt.set               (center_point.x*d_zoom,center_point.y*d_zoom);
        
        Fvector2 tmp_pt                 = GetAbsolutePos(); tmp_pt.add(new_center_pt);
        Frect   vis_abs_rect    = m_mapWnd->ActiveMapRect();
        Fvector2 tmp_pt2;
        vis_abs_rect.getcenter  (tmp_pt2);
        float dx                                = (tmp_pt2.x-tmp_pt.x)*d_zoom;
        float dy                                = (tmp_pt2.y-tmp_pt.y)*d_zoom;

        map_desired_rect.add    (GetWndPos().x+dx,GetWndPos().y+dy);
}
//////////////////////////////////////////////////////////////////////////

CUILevelMap::CUILevelMap(CUIMapWnd* p)
{
	m_mapWnd			= p;
	m_anomalies_map		= NULL;
	Show				(false);
}

CUILevelMap::~CUILevelMap()
{
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

	if(FALSE==fsimilar(kw,kh,EPS_L)){
		Msg("----incorrect global rect definition for map [%s]  kw=%f kh=%f",*MapName(),kw,kh);
		Msg("----try x2=%f",m_GlobalRect.x1+kh*BoundRect().width());
		Msg("----or  y2=%f",m_GlobalRect.y1+kw*BoundRect().height());
	}

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

//	CLevelFogOfWar* F	= Level().FogOfWarMngr().GetFogOfWar(MapName());
//	AttachChild		(F);
	Locations& ls =Level().MapManager().Locations();
	for(Locations_it it=ls.begin(); it!=ls.end(); ++it){
		if ((*it).location->Update())
			(*it).location->UpdateLevelMap(this);
	}
}

Frect CUILevelMap::CalcWndRectOnGlobal	()
{
	Frect res;
	CUIGlobalMap* globalMap			= MapWnd()->GlobalMap();

	res.lt							= globalMap->ConvertRealToLocal(GlobalRect().lt);
	res.rb							= globalMap->ConvertRealToLocal(GlobalRect().rb);
	res.add							(globalMap->GetWndPos().x, globalMap->GetWndPos().y);

	return res;
}

void CUILevelMap::Update()
{
	CUIGlobalMap*	w				= MapWnd()->GlobalMap();
	Frect			rect;
	Fvector2		tmp;

	tmp								= w->ConvertRealToLocal(GlobalRect().lt);
	rect.lt							= tmp;
	tmp								= w->ConvertRealToLocal(GlobalRect().rb);
	rect.rb							= tmp;

	SetWndRect						(rect);

	inherited::Update		();
}

void CUILevelMap::OnMouse	(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
	switch (mouse_action){			
		case WINDOW_LBUTTON_DOWN:{	
			MapWnd()->SetTargetMap( MapName() );
//			CUIGlobalMap* globalMap	= MapWnd()->GlobalMap();
//			globalMap->MapWnd()->SetZoom(16.f/*globalMap->MapWnd()->GetZoom()*2.f*/);
		}break;
	}
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
