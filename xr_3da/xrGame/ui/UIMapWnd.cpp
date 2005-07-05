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

const				SCROLLBARS_SHIFT			= 5;
const				VSCROLLBAR_STEP				= 20; // В пикселях
const				HSCROLLBAR_STEP				= 20; // В пикселях

const u32			activeLocalMapColor			= 0xffc80000;
const u32			inactiveLocalMapColor		= 0xff438cd1;
const u32			inactiveSmallLocalMapColor	= 0x00000000;
const u32			ourLevelMapColor			= 0xffffffff;


CUICustomMap::CUICustomMap ()
{
	m_BoundRect.set			(0,0,0,0);
	m_zoom_factor			=1.f;
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
	SetWindowName(*m_name);
}


void CUICustomMap::MoveWndDelta(const Fvector2& d)
{
	CUIWindow::MoveWndDelta	(d);
	Frect clip			= GetClipperRect();
	Frect r				= GetWndRect();
	if (r.x2<clip.width())	r.x1 += clip.width()-r.x2;
	if (r.y2<clip.height())	r.y1 += clip.height()-r.y2;
	if (r.x1>0.0f)			r.x1 = 0.0f;
	if (r.y1>0.0f)			r.y1 = 0.0f;
	SetWndPos				(r.x1, r.y1);


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
	res.x = m_BoundRect.lt.x + src.x/m_zoom_factor;
	res.y = m_BoundRect.height() + m_BoundRect.lt.y - src.y/m_zoom_factor;

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
	res.x = (src.x-m_BoundRect.lt.x) * m_zoom_factor;
	res.y = (m_BoundRect.height()-(src.y-m_BoundRect.lt.y)) * m_zoom_factor;

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
	
	m_zoom_factor	= w/m_BoundRect.width();

}

void CUICustomMap::FitToHeight	(float height)
{
	float k			= m_BoundRect.width()/m_BoundRect.height();
	float h			= height;
	float w			= k*height;
	SetWndRect		(0.0f,0.0f,w,h);
	
	m_zoom_factor	= h/m_BoundRect.height();

}

void CUICustomMap::SetZoomFactor(float z)
{
	m_zoom_factor	= z;
	SetWidth		( m_zoom_factor*m_BoundRect.width()		);
	SetHeight		( m_zoom_factor*m_BoundRect.height()	);
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
	CUIStatic::Draw();
#ifdef DEBUG
	if(0){ //bDebud

		CGameFont* F		= UI()->Font()->pFontDI;
		F->SetAligment		(CGameFont::alCenter);
		F->SetSizeI			(0.02f);
		F->OutSetI			(0.f,-0.8f);
		F->SetColor			(0xffffffff);

		Fvector2 cursor_pos = GetUICursor()->GetPos();

		Fvector2 pt = ConvertLocalToReal(cursor_pos);
		F->OutNext			("pos on map = %3.2f-%3.2f",pt.x, pt.y);
	}

#endif
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
	m_MinimizedSize.set		(0,0);
	m_NormalSize.set		(0,0);
	m_State					= stNone;
	Show					(false);
}



CUIGlobalMap::~CUIGlobalMap()
{
}

void CUIGlobalMap::Init		(shared_str name, CInifile& gameLtx, LPCSTR sh_name)
{
	inherited::Init(name, gameLtx, sh_name);
	CUIStatic::SetColor(0xc8ffffff);
	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH, "pda_map.xml");
	R_ASSERT3(xml_result, "xml file not found", "global_map.xml");



	CUIXmlInit xml_init;
	string256				path;
	// load map state definition
	strcpy					(path,"main_wnd:global_map:background:minimized_size");
	R_ASSERT3				(uiXml.NavigateToNode(path,0), "XML node not found", path);
	m_MinimizedSize.x		= uiXml.ReadAttribFlt(path, 0, "w", 0);
	m_MinimizedSize.y		= uiXml.ReadAttribFlt(path, 0, "h", 0);
	strcpy					(path,"main_wnd:global_map:background:normal_size");
	R_ASSERT3				(uiXml.NavigateToNode(path,0), "XML node not found", path);
	m_NormalSize.x			= uiXml.ReadAttribFlt(path, 0, "w", 0);
	m_NormalSize.y			= uiXml.ReadAttribFlt(path, 0, "h", 0);

	// minimized button
	strcpy						(path,"main_wnd:global_map:background:minimized_btn");
	m_minimized_btn.SetWindowName("minimized_btn");
	xml_init.InitButton			(uiXml, path, 0, &m_minimized_btn);
	m_minimized_btn_offset		= m_minimized_btn.GetWndPos();//store parent offest for buttons

	m_minimized_btn.SetAutoDelete(false);
	AttachChild					(&m_minimized_btn);

	// maximized button
	strcpy						(path,"main_wnd:global_map:background:maximized_btn");
	m_maximized_btn.SetWindowName("maximized_btn");
	xml_init.InitButton			(uiXml, path, 0, &m_maximized_btn);
	m_maximized_btn_offset		= m_maximized_btn.GetWndPos();//store parent offest for buttons
	m_maximized_btn.SetAutoDelete(false);
	AttachChild					(&m_maximized_btn);

	//init callbacks
	Register					(&m_minimized_btn);
	Register					(&m_maximized_btn);
	AddCallback					("minimized_btn",BUTTON_CLICKED,boost::bind(&CUIGlobalMap::OnBtnMinimizedClick,this));
	AddCallback					("maximized_btn",BUTTON_CLICKED,boost::bind(&CUIGlobalMap::OnBtnMaximizedClick,this));
}

void	CUIGlobalMap::SendMessage			(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}


void CUIGlobalMap::SwitchTo(CUIGlobalMap::EState new_state)
{
	if (new_state!=m_State){

		switch (new_state){
		case stMinimized:		SetWndPos(0,0);FitToWidth(m_MinimizedSize.x);SetHeight(m_MinimizedSize.y);	break;
		case stNormal:			{
									SetWndPos(0,0);
									FitToWidth(m_NormalSize.x);
									if( MapWnd()->ActiveLevelMap()==this )
										MapWnd()->SetActiveMap(m_prev_active_map);
								}break;
		case stMaximized:{
					m_prev_active_map = MapWnd()->ActiveLevelMap()? MapWnd()->ActiveLevelMap()->MapName():"";
					OptimalFit(GetParent()->GetWndRect());
					MapWnd()->SetActiveMap( MapName() );
				};break;
		}
		m_State					= new_state;

		bool bEnable = (m_State != stMinimized);
			WINDOW_LIST& wl = GetChildWndList();
			for( WINDOW_LIST_it it = wl.begin(); it!= wl.end(); ++it ){
				CUIGlobalMapSpot* msp = smart_cast<CUIGlobalMapSpot*>(*it);
				if(msp)
					msp->Show(bEnable);
			}
		SetClipRect( GetAbsoluteRect() );
	}
}
void CUIGlobalMap::Update()
{
	inherited::Update();
	Frect clip = GetClipperRect	();
	Frect rect = GetAbsoluteRect();

	//set button(minimize and maximize) position relatively current clip position
	Fvector2 min_btn_pos, max_btn_pos;
	
	min_btn_pos = clip.lt;
	min_btn_pos.sub(rect.lt);
	min_btn_pos.add(m_minimized_btn_offset);

	max_btn_pos = clip.lt;
	max_btn_pos.sub(rect.lt);
	max_btn_pos.add(m_maximized_btn_offset);
	
	m_maximized_btn.SetWndPos(max_btn_pos);
	m_minimized_btn.SetWndPos(min_btn_pos);

}

void CUIGlobalMap::OnBtnMinimizedClick()
{
	switch (m_State){
	case stMinimized:								break;
	case stNormal:			SwitchTo(stMinimized);	break;
	case stMaximized:		SwitchTo(stNormal);		break;
	}
}

void CUIGlobalMap::OnBtnMaximizedClick()
{
	switch (m_State){
	case stMinimized:		SwitchTo(stNormal);		break;
	case stNormal:			SwitchTo(stMaximized);	break;
	case stMaximized:								break;
	}
}

void CUIGlobalMap::Draw					()
{
	inherited::Draw();
}

Fvector2 CUIGlobalMap::ConvertRealToLocal(const Fvector2& src)// pixels->pixels (relatively own left-top pos)
{
	Fvector2 res;
	res.x = (src.x-m_BoundRect.lt.x ) * m_zoom_factor;
	res.y = (src.y-m_BoundRect.lt.y ) * m_zoom_factor;
	return res;
}

//////////////////////////////////////////////////////////////////////////

CUILevelMap::CUILevelMap()
{
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
}

void CUILevelMap::Init	(shared_str name, CInifile& gameLtx, LPCSTR sh_name)
{
	inherited::Init(name, gameLtx, sh_name);
	Fvector4 tmp = gameLtx.r_fvector4(MapName(),"global_rect");
	m_GlobalRect.set(tmp.x, tmp.y, tmp.z, tmp.w);

	m_globalMapSpot->Init(inactiveLocalMapColor);

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
//		if ((*it).location->Update())
			(*it).location->UpdateMiniMap(this);
	}

}


///////////////////////////////////////////////////////////////////////////
CUIMapWnd::CUIMapWnd()
{
	m_activeLevelMap		= NULL;
	m_GlobalMap				= NULL;
	m_flags.zero();
	Show(false);
}

CUIMapWnd::~CUIMapWnd()
{
	delete_data(m_GameMaps);
}

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	if(!m_activeLevelMap)return;
	m_activeLevelMap->SetActivePoint(vNewPoint);
	UpdateScroll();
}

void CUIMapWnd::Init()
{
	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH, "pda_map.xml");
	R_ASSERT3(xml_result, "xml file not found", "pda_map.xml");

	// load map background
	CUIXmlInit xml_init;
	xml_init.InitWindow		(uiXml, "main_wnd", 0, this);



	AttachChild(&m_UIMainFrame);
	xml_init.InitFrameWindow(uiXml, "main_wnd:main_map_frame", 0, &m_UIMainFrame);

	xml_init.InitWindow(uiXml, "main_wnd:main_map_frame:level_frame", 0, &m_UILevelFrame);
	m_UIMainFrame.AttachChild(&m_UILevelFrame);

	Frect r = m_UILevelFrame.GetWndRect();

	m_UIMainScrollV.Init			(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	m_UIMainScrollV.SetWindowName	("scroll_v");
	m_UIMainScrollV.SetStepSize		(_max(1,iFloor(m_UILevelFrame.GetHeight()/10)));
	m_UIMainScrollV.SetPageSize		(iFloor(m_UILevelFrame.GetHeight()));
	m_UIMainFrame.AttachChild		(&m_UIMainScrollV);
	Register						(&m_UIMainScrollV);
	AddCallback						("scroll_v",SCROLLBAR_VSCROLL,boost::bind(&CUIMapWnd::OnScrollV,this));

	m_UIMainScrollH.Init			(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	m_UIMainScrollH.SetWindowName	("scroll_h");
	m_UIMainScrollH.SetStepSize		(_max(1,iFloor(m_UILevelFrame.GetWidth()/10)));
	m_UIMainScrollH.SetPageSize		(iFloor(m_UILevelFrame.GetWidth()));
	m_UIMainFrame.AttachChild		(&m_UIMainScrollH);
	Register						(&m_UIMainScrollH);
	AddCallback						("scroll_h",SCROLLBAR_HSCROLL,boost::bind(&CUIMapWnd::OnScrollH,this));

	AttachChild(&UIMainMapHeader);
	xml_init.InitFrameLine(uiXml, "map_header_frame_line", 0, &UIMainMapHeader);


// Load maps
	string256			gameLtxPath;
	FS.update_path		(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile gameLtx	(gameLtxPath);

	m_GlobalMap				= xr_new<CUIGlobalMap>(this);
	m_GlobalMap->SetAutoDelete				(true);
	m_GlobalMap->Init						("global_map",gameLtx,"hud\\default");
	m_UILevelFrame.AttachChild				(m_GlobalMap);
	m_GlobalMap->SwitchTo					(CUIGlobalMap::stNormal);


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

			l = xr_new<CUILevelMap>();
			
			l->Init(map_name, gameLtx, "hud\\set");

			l->OptimalFit( m_UILevelFrame.GetWndRect() );
			
			if(l->GlobalMapSpot())
				m_GlobalMap->AttachChild(l->GlobalMapSpot() );
		}
	}
}

void CUIMapWnd::Show(bool status)
{
	if (status)
	{
		m_GlobalMap->Show(true);
		if(m_activeLevelMap==NULL)
			SetActiveMap				(Level().name());
		SetActivePoint				( Level().CurrentEntity()->Position() );
		InventoryUtilities::SendInfoToActor("ui_pda_map_local");
	}else{
		if(m_GlobalMap)m_GlobalMap->Show(false);
		GetUICursor()->HoldMode(false);
		m_flags.set(lmMouseHold,FALSE);
	}

	inherited::Show(status);
}

void CUIMapWnd::SetActiveMap			(shared_str level_name)
{

	if(m_activeLevelMap	&& m_activeLevelMap!=m_GlobalMap)
		m_UILevelFrame.DetachChild		(m_activeLevelMap);

	//if Global map 
	if( level_name == m_GlobalMap->MapName() ){
		m_activeLevelMap = m_GlobalMap;
	}else{
		GameMapsPairIt it			= m_GameMaps.find(level_name);
		if( m_GameMaps.end()==it)	return;
		
		m_activeLevelMap						= it->second;
		m_UILevelFrame.AttachChild				(m_activeLevelMap);
	};
	m_activeLevelMap->Show(true);
	m_UILevelFrame.BringToTop	(m_GlobalMap);

	// set scroll range 
	m_UIMainScrollV.SetRange	(0,iFloor(m_activeLevelMap->GetHeight()));
	m_UIMainScrollH.SetRange	(0,iFloor(m_activeLevelMap->GetWidth()));
	UpdateScroll				();

	m_activeLevelMap->SetClipRect( m_UILevelFrame.GetAbsoluteRect() );
}


void CUIMapWnd::Draw()
{
	if(m_activeLevelMap)
		m_activeLevelMap->SetClipRect( m_UILevelFrame.GetAbsoluteRect() );

	inherited::Draw();

}


void CUIMapWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
	Fvector2 cursor_pos = GetUICursor()->GetPos();

	if(m_activeLevelMap && m_UILevelFrame.GetAbsoluteRect().in( cursor_pos ) ){
		switch (mouse_action){
		case WINDOW_MOUSE_MOVE:
			if(m_flags.test(lmMouseHold)){
				m_activeLevelMap->MoveWndDelta	(GetUICursor()->GetPosDelta());
				UpdateScroll					();
			}
		break;
		case WINDOW_LBUTTON_DOWN:
			GetUICursor()->HoldMode(true);
			m_flags.set(lmMouseHold,TRUE);
		break;
		case WINDOW_LBUTTON_UP:
			GetUICursor()->HoldMode(false);
			m_flags.set(lmMouseHold,FALSE);
		break;
		case WINDOW_MOUSE_WHEEL_UP:
			m_UIMainScrollV.TryScrollDec();
		break;
		case WINDOW_MOUSE_WHEEL_DOWN:
			m_UIMainScrollV.TryScrollInc();
		break;


		}	
	};
}

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIMapWnd::UpdateScroll()
{
	Fvector2 w_pos				= m_activeLevelMap->GetWndPos();
	m_UIMainScrollV.SetScrollPos(iFloor(-w_pos.y));
	m_UIMainScrollH.SetScrollPos(iFloor(-w_pos.x));
}


void CUIMapWnd::OnScrollV()
{
	if (m_activeLevelMap){
		int s_pos					= m_UIMainScrollV.GetScrollPos();
		Fvector2 w_pos				= m_activeLevelMap->GetWndPos();
		m_activeLevelMap->SetWndPos	(w_pos.x,float(-s_pos));
	}
}
void CUIMapWnd::OnScrollH()
{
	if (m_activeLevelMap){
		int s_pos					= m_UIMainScrollH.GetScrollPos();
		Fvector2 w_pos				= m_activeLevelMap->GetWndPos();
		m_activeLevelMap->SetWndPos	(float(-s_pos),w_pos.y);
	}
}

void CUIMapWnd::SetStatusInfo			(LPCSTR status_text)
{
	UIMainMapHeader.UITitleText.SetText(status_text);
}

void CUIMapWnd::Update()
{
	ShowHint();
	inherited::Update();
}

void CUIMapWnd::ShowHint()
{
	Fvector2 cursor_pos = GetUICursor()->GetPos();
	LPCSTR hint = NULL;

	if( m_GlobalMap->GetAbsoluteRect().in(cursor_pos) ){
		hint = m_GlobalMap->GetHint();
	}else
	if( m_activeLevelMap && m_activeLevelMap->GetAbsoluteRect().in(cursor_pos) ){
		hint = m_activeLevelMap->GetHint();
	}

	SetStatusInfo(hint);
}




CUIGlobalMapSpot::CUIGlobalMapSpot		(CUICustomMap* m)
{
	m_owner_map = smart_cast<CUILevelMap*>(m);
}

CUIGlobalMapSpot::~CUIGlobalMapSpot		()
{}


void CUIGlobalMapSpot::Draw		()
{
	inherited::Draw();
}

void CUIGlobalMapSpot::Init	(u32 color)
{
	inherited::Init(0.0f,0.0f,20.0f,20.0f);
	UIBorder.SetVisiblePart(CUIFrameRect::fmBK,FALSE);
	UIBorder.Init("ui_frame_very_small",0.0f,0.0f,20.0f,20.0f);
	UIBorder.SetColor(color);
	AttachChild(&UIBorder);
}

void CUIGlobalMapSpot::OnMouse	(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
		switch (mouse_action){
			case WINDOW_LBUTTON_DOWN:{
					CUIGlobalMap* globalMap = (CUIGlobalMap*)GetParent();
					globalMap->MapWnd()->SetActiveMap( m_owner_map->MapName() );
					globalMap->SwitchTo(CUIGlobalMap::stNormal);
				}break;
		}
}

void CUIGlobalMapSpot::Update()
{
	//rect, pos
	CUIGlobalMap* w = (CUIGlobalMap*)GetParent();

	Frect rect;
	Fvector2 tmp;

	tmp = w->ConvertRealToLocal(m_owner_map->GlobalRect().lt);
	rect.lt = tmp;

	tmp = w->ConvertRealToLocal(m_owner_map->GlobalRect().rb);
	rect.rb = tmp;
	
	SetWndRect				(rect);
	UIBorder.SetWndPos		(0.0f,0.0f);
	UIBorder.SetWidth		(rect.width());
	UIBorder.SetHeight		(rect.height());

	//highlight border
	if(w->MapWnd()->ActiveLevelMap()==m_owner_map)
		UIBorder.SetColor(ourLevelMapColor);
	else{
		Fvector2 cursor_pos = GetUICursor()->GetPos();
		if(GetAbsoluteRect().in( cursor_pos ) )
				UIBorder.SetColor(activeLocalMapColor);
		else
				UIBorder.SetColor(inactiveLocalMapColor);
	};
}
LPCSTR CUIGlobalMapSpot::GetHint()
{
	return *m_owner_map->MapName();
}
