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

void CUICustomMap::Init	(shared_str name, CInifile& gameLtx)
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
	CUIStatic::Init		(tex, 0, 0, iFloor(m_BoundRect.width()), iFloor(m_BoundRect.height()) );
	SetStretchTexture	(true);
	ClipperOn			();
	SetWindowName(*m_name);
}


void CUICustomMap::MoveWndDelta(const Ivector2& d)
{
	CUIWindow::MoveWndDelta	(d);
	Irect clip			= GetClipperRect();
	Irect r				= GetWndRect();
	if (r.x2<clip.width())	r.x1 += clip.width()-r.x2;
	if (r.y2<clip.height())	r.y1 += clip.height()-r.y2;
	if (r.x1>0)				r.x1 = 0;
	if (r.y1>0)				r.y1 = 0;
	SetWndPos			(r.x1, r.y1);


}

void rotation_(int x, int y, const float angle, int& x_, int& y_)
{
	x_= iFloor(float(x)*_cos(angle)+float(y)*_sin(angle));
	y_= iFloor(float(y)*_cos(angle)-float(x)*_sin(angle));
}

Ivector2 CUICustomMap::ConvertRealToLocal  (const Fvector2& src)// meters->pixels (relatively own left-top pos)
{
	Ivector2 res;
	if( !Heading() ){
		res.x = iFloor( (src.x-m_BoundRect.lt.x) * m_zoom_factor);
		res.y = iFloor( (m_BoundRect.height()-(src.y-m_BoundRect.lt.y)) * m_zoom_factor);
		return res;
	}else
	{
		Ivector2 heading_pivot = GetStaticItem()->GetHeadingPivot();
	
		res = ConvertRealToLocalNoTransform(src);
		res.sub(heading_pivot);
		rotation_(res.x, res.y, GetHeading(), res.x, res.y);
		res.add(heading_pivot);
		return res;
	};
}

Ivector2 CUICustomMap::ConvertRealToLocalNoTransform  (const Fvector2& src)// meters->pixels (relatively own left-top pos)
{
	Ivector2 res;
	res.x = iFloor( (src.x-m_BoundRect.lt.x) * m_zoom_factor);
	res.y = iFloor( (m_BoundRect.height()-(src.y-m_BoundRect.lt.y)) * m_zoom_factor);

	return res;
}

//position and heading for drawing pointer to src pos
bool CUICustomMap::GetPointerTo(const Ivector2& src, int item_radius, Ivector2& pos, float& heading)
{
	Irect		clip_rect_abs			= GetClipperRect(); //absolute rect coords
	Irect		map_rect_abs			= GetAbsoluteRect();

	Irect		rect;
	BOOL res = rect.intersection(clip_rect_abs, map_rect_abs);
	VERIFY(res);
	
	rect = clip_rect_abs;
	rect.sub(map_rect_abs.lt.x,map_rect_abs.lt.y);

	Fbox2 f_clip_rect_local;
	f_clip_rect_local.set(rect.x1, rect.y1, rect.x2, rect.y2);

	Fvector2 f_center;
	f_clip_rect_local.getcenter(f_center);

	Fvector2 f_dir, f_src;

	f_src.set(src.x, src.y);
	f_dir.sub(f_center, f_src );
	f_dir.normalize_safe();
	Fvector2 f_intersect_point;
	res = f_clip_rect_local.Pick2(f_src,f_dir,f_intersect_point);
	VERIFY(res);


	heading = -f_dir.getH();

	f_intersect_point.mad(f_intersect_point,f_dir,item_radius);

	pos.set( iFloor(f_intersect_point.x), iFloor(f_intersect_point.y) );
	return true;
}


void CUICustomMap::FitToWidth	(u32 width)
{
	float k			= m_BoundRect.width()/m_BoundRect.height();
	int w			= width;
	int h			= iFloor(float(width)/k);
	SetWndRect		(0,0,w,h);
	
	m_zoom_factor	= w/m_BoundRect.width();

}

void CUICustomMap::FitToHeight	(u32 height)
{
	float k			= m_BoundRect.width()/m_BoundRect.height();
	int h			= height;
	int w			= iFloor(k*height);
	SetWndRect		(0,0,w,h);
	
	m_zoom_factor	= h/m_BoundRect.height();

}

void CUICustomMap::SetZoomFactor(float z)
{
	m_zoom_factor	= z;
	SetWidth		( iFloor(m_zoom_factor*m_BoundRect.width())	);
	SetHeight		( iFloor(m_zoom_factor*m_BoundRect.height())	);
}

void CUICustomMap::OptimalFit(const Irect& r)
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

	Ivector2	pos_on_map		= ConvertRealToLocalNoTransform(pos);
	Irect		map_abs_rect	= GetAbsoluteRect();
	Ivector2	pos_abs;

	pos_abs.set(map_abs_rect.lt);
	pos_abs.add(pos_on_map);

	Irect		clip_abs_rect	= GetClipperRect();
	Ivector2	clip_center;
	clip_abs_rect.getcenter(clip_center);
	clip_center.sub(pos_abs);

	MoveWndDelta				(clip_center);
	SetHeadingPivot				(pos_on_map);
}

LPCSTR	CUICustomMap::GetHint()
{
	Ivector2 cursor_pos = GetUICursor()->GetPos();
	LPCSTR hint = NULL;
	WINDOW_LIST& wl = GetChildWndList();
	xr_list<CUIWindow*>::reverse_iterator it = wl.rbegin();
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
}

bool CUICustomMap::IsRectVisible(Irect r)
{
	Irect map_visible_rect = GetClipperRect();
	Ivector2 pos = GetAbsolutePos();
	r.add(pos.x,pos.y);

	return !!map_visible_rect.intersected(r);
}

bool CUICustomMap::NeedShowPointer(Irect r)
{
	Irect map_visible_rect = GetClipperRect();
	map_visible_rect.shrink(5,5);
	Ivector2 pos = GetAbsolutePos();
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

void CUIGlobalMap::Init		(shared_str _name, CInifile& gameLtx)
{
	inherited::Init(_name, gameLtx);

	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH, "pda_map.xml");
	R_ASSERT3(xml_result, "xml file not found", "global_map.xml");



	CUIXmlInit xml_init;
	string256				path;
	// load map state definition
	strcpy					(path,"main_wnd:global_map:background:minimized_size");
	R_ASSERT3				(uiXml.NavigateToNode(path,0), "XML node not found", path);
	m_MinimizedSize.x		= uiXml.ReadAttribInt(path, 0, "w", 0);
	m_MinimizedSize.y		= uiXml.ReadAttribInt(path, 0, "h", 0);
	strcpy					(path,"main_wnd:global_map:background:normal_size");
	R_ASSERT3				(uiXml.NavigateToNode(path,0), "XML node not found", path);
	m_NormalSize.x			= uiXml.ReadAttribInt(path, 0, "w", 0);
	m_NormalSize.y			= uiXml.ReadAttribInt(path, 0, "h", 0);

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
	Irect clip = GetClipperRect	();
	Irect rect = GetAbsoluteRect();

	//set button(minimize and maximize) position relatively current clip position
	Ivector2 min_btn_pos, max_btn_pos;
	
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


//////////////////////////////////////////////////////////////////////////

CUILevelMap::CUILevelMap()
:m_globalMapSpot(this)
{
	Show(false);
}

CUILevelMap::~CUILevelMap()
{}

void CUILevelMap::Init	(shared_str name, CInifile& gameLtx)
{
	inherited::Init(name, gameLtx);

	Fvector4 tmp = gameLtx.r_fvector4(MapName(),"global_rect");
	m_GlobalRect.set(tmp.x, tmp.w, tmp.z, tmp.y);

	m_globalMapSpot.Init(inactiveLocalMapColor);
}



void CUILevelMap::UpdateSpots		()
{
	DetachAll();
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



void CUIMiniMap::Init(shared_str name, CInifile& gameLtx)
{
	inherited::Init(name, gameLtx);
}
void CUIMiniMap::MoveWndDelta(const Ivector2& d)
{
	CUIWindow::MoveWndDelta	(d);
}
void CUIMiniMap::UpdateSpots()
{
	DetachAll();
	Locations& ls =Level().MapManager().Locations();
	for(Locations_it it=ls.begin(); it!=ls.end(); ++it){
		if ((*it).location->Update())
			(*it).location->UpdateMiniMap(this);
	}

}


///////////////////////////////////////////////////////////////////////////
CUIMapWnd::CUIMapWnd()
{
	m_activeLevelMap		= NULL;
	m_flags.zero();
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
	R_ASSERT3(xml_result, "xml file not found", "global_map.xml");

	// load map background
	CUIXmlInit xml_init;
	xml_init.InitWindow		(uiXml, "main_wnd", 0, this);



	AttachChild(&m_UIMainFrame);
	xml_init.InitFrameWindow(uiXml, "main_wnd:main_map_frame", 0, &m_UIMainFrame);

	xml_init.InitStatic(uiXml, "main_wnd:main_map_frame:level_frame", 0, &m_UILevelFrame);
	m_UIMainFrame.AttachChild(&m_UILevelFrame);

	Irect r = m_UILevelFrame.GetWndRect();

	m_UIMainScrollV.Init			(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	m_UIMainScrollV.SetWindowName	("scroll_v");
	m_UIMainScrollV.SetStepSize		(_max(1,m_UILevelFrame.GetHeight()/10));
	m_UIMainScrollV.SetPageSize		(m_UILevelFrame.GetHeight());
	m_UIMainFrame.AttachChild		(&m_UIMainScrollV);
	Register						(&m_UIMainScrollV);
	AddCallback						("scroll_v",SCROLLBAR_VSCROLL,boost::bind(&CUIMapWnd::OnScrollV,this));

	m_UIMainScrollH.Init			(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	m_UIMainScrollH.SetWindowName	("scroll_h");
	m_UIMainScrollH.SetStepSize		(_max(1,m_UILevelFrame.GetWidth()/10));
	m_UIMainScrollH.SetPageSize		(m_UILevelFrame.GetWidth());
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
	m_GlobalMap->Init						("global_map",gameLtx);
	m_UILevelFrame.AttachChild				(m_GlobalMap);
	m_GlobalMap->SwitchTo					(CUIGlobalMap::stNormal);


	// initialize local maps
	if (gameLtx.section_exist("level_maps_single")){
		CInifile::Sect& S		= gameLtx.r_section("level_maps_single");
		CInifile::SectIt	it	= S.begin(), end = S.end();
		for (;it!=end; it++){
			shared_str map_name = it->first;
			xr_strlwr(map_name);
			R_ASSERT2	(m_GameMaps.end() == m_GameMaps.find(map_name), "Duplicate level name not allowed");
			
			CUICustomMap*& l = m_GameMaps[map_name];

			l = xr_new<CUILevelMap>();
			
			l->Init(map_name, gameLtx);

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
		InitGlobalMapObjectives			();
		InitLocalMapObjectives			();
		if(m_activeLevelMap==NULL)
			SetActiveMap				(Level().name());
			SetActivePoint				( Level().CurrentEntity()->Position() );
	}else{
		m_GlobalMap->Show(false);
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
	m_UIMainScrollV.SetRange	(0,m_activeLevelMap->GetHeight());
	m_UIMainScrollH.SetRange	(0,m_activeLevelMap->GetWidth());
	UpdateScroll				();

	m_activeLevelMap->SetClipRect( m_UILevelFrame.GetAbsoluteRect() );
}

void CUIMapWnd::InitGlobalMapObjectives	()
{}

void CUIMapWnd::InitLocalMapObjectives	()
{}

void CUIMapWnd::Draw()
{
	if(m_activeLevelMap)
		m_activeLevelMap->SetClipRect( m_UILevelFrame.GetAbsoluteRect() );

	inherited::Draw();

}


void CUIMapWnd::OnMouse(int x, int y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
	Ivector2 cursor_pos = GetUICursor()->GetPos();

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
	Ivector2 w_pos				= m_activeLevelMap->GetWndPos();
	m_UIMainScrollV.SetScrollPos(-w_pos.y);
	m_UIMainScrollH.SetScrollPos(-w_pos.x);
}


void CUIMapWnd::OnScrollV()
{
	if (m_activeLevelMap){
		int s_pos					= m_UIMainScrollV.GetScrollPos();
		Ivector2 w_pos				= m_activeLevelMap->GetWndPos();
		m_activeLevelMap->SetWndPos	(w_pos.x,-s_pos);
	}
}
void CUIMapWnd::OnScrollH()
{
	if (m_activeLevelMap){
		int s_pos					= m_UIMainScrollH.GetScrollPos();
		Ivector2 w_pos				= m_activeLevelMap->GetWndPos();
		m_activeLevelMap->SetWndPos	(-s_pos,w_pos.y);
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
	Ivector2 cursor_pos = GetUICursor()->GetPos();
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
	inherited::Init(0,0,20,20);
	UIBorder.Init("ui\\ui_frame_very_small",0,0,20,20);
	UIBorder.SetColor(color);
	AttachChild(&UIBorder);
}

void CUIGlobalMapSpot::OnMouse	(int x, int y, EUIMessages mouse_action)
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

	Irect rect;
	Ivector2 tmp;

	tmp = w->ConvertRealToLocal(m_owner_map->GlobalRect().lt);
	rect.lt = tmp;

	tmp = w->ConvertRealToLocal(m_owner_map->GlobalRect().rb);
	rect.rb = tmp;
	
	SetWndRect				(rect);
	UIBorder.SetWndPos		(0,0);
	UIBorder.SetWidth		(rect.width());
	UIBorder.SetHeight		(rect.height());
	UIBorder.SetClipper(true, w->GetClipperRect() );

	//highlight border
	if(w->MapWnd()->ActiveLevelMap()==m_owner_map)
		UIBorder.SetColor(ourLevelMapColor);
	else{
		Ivector2 cursor_pos = GetUICursor()->GetPos();
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

/*
using namespace InventoryUtilities;

#define				MAP_LEFT					89
#define				MAP_TOP						93
#define				MAP_RADIUS					80


//-----------------------------------------------------------------------------/
//	Задаются координаты карты в game.ltx запись ui_map_dims
//	лево, право, низ, верх
//-----------------------------------------------------------------------------/

const char * const	PDA_MAP_XML					= "map.xml";
const				SCROLLBARS_SHIFT			= 5;
const				VSCROLLBAR_STEP				= 20; // В пикселях
const				HSCROLLBAR_STEP				= 20; // В пикселях

//-----------------------------------------------------------------------------/
//	Global map stuff
//-----------------------------------------------------------------------------/

const char * const	GLOBAL_MAP_LOCATIONS_LTX	= "global_map_locations";

const u32			activeLocalMapColor			= 0xffc80000;
const u32			inactiveLocalMapColor		= 0xff438cd1;
const u32			inactiveSmallLocalMapColor	= 0x00000000;
const u32			ourLevelMapColor			= 0xffffffff;
const int			globalMapButtonWidth		= 128;
const int			globalMapButtonHeight		= 256;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapWnd::CUIMapWnd()
	:	m_eCurrentMode		(emmLocal),
		m_pCurrentMap		(&UILocalMapBackground),
		m_GlobalMapHeight	(0.0f),
		m_GlobalMapWidth	(0.0f),
		m_GlobalMapShader	(NULL)
{
	Show					(false);
	SetFont					(HUD().Font().pFontSmall);
	m_rDefault.left			= 0;
	m_rDefault.top			= 0;
	m_rDefault.bottom		= 100;
	m_rDefault.right		= 100;
}

//////////////////////////////////////////////////////////////////////////

CUIMapWnd::~CUIMapWnd()
{
	UILocalMapBackground.RemoveAllSpots	();
	UIGlobalMapBackground.RemoveAllSpots();
	DeleteLocalMapsData					();
	m_GlobalMapShader.destroy			();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Show(bool status)
{
	if (status)
	{
		InitGlobalMapObjectives			();
		InitLocalMapObjectives			();
		m_eCurrentMode = emmGlobal;
		SwitchMapMode					(emmLocal);
		SetLocalMap						(Level().name());
	}
	inherited::Show(status);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Init()
{
	CUIXmlInit xml_init;
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, PDA_MAP_XML);

	inherited::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIMainMapFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainMapFrame);
	UIMainMapFrame.AttachChild(&UIPDAHeader);
	xml_init.InitFrameLine(uiXml, "map_header_frame_line", 0, &UIPDAHeader);

	string128 buf;
	strconcat(buf, ALL_PDA_HEADER_PREFIX, "/Map");

	UIPDAHeader.UITitleText.SetText(buf);

	//информация о точке на карте на которую наводит игрок
	AttachChild(&UIStaticInfo);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticInfo);

	UIStaticInfo.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0, UIStaticInfo.GetWidth(), UIStaticInfo.GetHeight(), "map_character.xml");
	UIStaticInfo.Enable(false);
	
	UIMainMapFrame.AttachChild(&UILocalMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UILocalMapBackground);
	UILocalMapBackground.SetMessageTarget(this);
	UILocalMapBackground.Enable(true);

	UIMainMapFrame.AttachChild(&UIGlobalMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UIGlobalMapBackground);
	UIGlobalMapBackground.SetMessageTarget(this);
	UIGlobalMapBackground.Enable(true);
	UIMainMapFrame.DetachChild(&UIGlobalMapBackground);

	// Descriptions
	UIGlobalMapBackground.AttachChild(&UIMapName);
	xml_init.InitStatic(uiXml, "map_name_static", 0, &UIMapName);
	UIMapName.Enable(false);
	UIMapName.Show(false);

	UIGlobalMapBackground.AttachChild(&UIMapGoals);
	xml_init.InitListWnd(uiXml, "map_goals_list", 0, &UIMapGoals);
	UIMapGoals.Enable(false);
	UIMapGoals.Show(false);

	Irect r = UILocalMapBackground.GetWndRect();

	UIMainMapFrame.AttachChild(&UIMapBgndV);
	UIMapBgndV.Init(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	UIMapBgndV.SetMessageTarget(this);

	UIMainMapFrame.AttachChild(&UIMapBgndH);
	UIMapBgndH.Init(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	UIMapBgndH.SetMessageTarget(this);

	// Кнопка переключения глобальная/локальная карта
	UILocalMapBackground.AttachChild(&UIMapTypeSwitch);
	xml_init.InitButton(uiXml, "gl_button", 0, &UIMapTypeSwitch);
	UIMapTypeSwitch.SetMessageTarget(this);

	// Имя текущей локальной карты
	UILocalMapBackground.AttachChild(&UILocalMapName);
	xml_init.InitStatic(uiXml, "local_map_name_static", 0, &UILocalMapName);
	UILocalMapName.Enable(false);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	// Инициализируем карты
	InitMaps();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SetLocalMap(const shared_str &levelName)
{
	UILocalMapBackground.m_pActiveMapSpot = NULL;

	LocalMapsPairIt currentMapIt = m_LocalMaps.find(levelName);
	R_ASSERT3(m_LocalMaps.end() != currentMapIt, "Map data for current level is absent.\nMost possible reason - this level not included in game graph", *levelName);

	R_ASSERT(currentMapIt->second.mapTexture);
	UILocalMapBackground.InitMapBackground(currentMapIt->second.mapTexture);

	// Теперь выводим имя карты
	UILocalMapName.SetText(*CStringTable()(levelName));
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitLocalMapObjectives()
{
	///убрать все интерактивные отметки на карте 
	UILocalMapBackground.RemoveAllSpots();

	Ivector2 P;
	Fvector src;

	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

	if (!pActor) return;

	int left	= UILocalMapBackground.GetWndRect().left;
	int top		= UILocalMapBackground.GetWndRect().top;

	////////////////////////////////////////////
	//добавить локации, имеющиеся в памяти PDA
	////////////////////////////////////////////

	for(LOCATIONS_PTR_VECTOR_IT it = Level().MapLocations().begin();
		Level().MapLocations().end() != it; 
		it++)
	{
		SMapLocation& map_location = *(*it);
		if(map_location.type_flags.test(eMapLocationPDAContact))	
			continue;

		
		CUIMapSpot * map_spot;
		map_spot = xr_new<CUIMapSpot>();
		
		if(map_location.attached_to_object)
		{
			map_spot->SetObjectID(map_location.object_id);
			src = map_spot->MapPos();
		}
		else
		{
			src.x = map_location.x;
			src.y = 0;
			src.z = map_location.y;

			map_spot->SetObjectID(0xffff);
			map_spot->m_vWorldPos.set(map_location.x,0.f,map_location.y);
		}

		map_spot->arrow_color		= map_location.icon_color;
		map_spot->m_bArrowEnabled	= map_location.marker_show;
		map_spot->m_LevelName		= map_location.level_name;

		UILocalMapBackground.ConvertToLocal(src,P);

		if (map_location.GetShader())
			map_spot->SetShader(map_location.GetShader());
		else
        	map_spot->SetShader(GetMapIconsShader());

		map_spot->SetWndPos(P.x + left, P.y + top);
		map_spot->SetWidth(MAP_ICON_WIDTH*map_location.icon_width);
		map_spot->SetHeight(MAP_ICON_HEIGHT*map_location.icon_height);
		map_spot->GetUIStaticItem().SetOriginalRect(
			map_location.icon_x*MAP_ICON_GRID_WIDTH,
			map_location.icon_y*MAP_ICON_GRID_HEIGHT,
			map_location.icon_width*(MAP_ICON_GRID_WIDTH + MAP_ICON_WIDTH),
			map_location.icon_height*(MAP_ICON_GRID_HEIGHT + MAP_ICON_HEIGHT));

		map_spot->m_eAlign = CUIMapSpot::eCenter;
		map_spot->m_sDescText.SetText("");
		map_spot->m_sNameText.SetText("");
		map_spot->m_bArrowEnabled = true;
		if (map_location.dynamic_manifestation) map_spot->DynamicManifestation(true);

		if(xr_strlen(map_location.text)>1)
		{
			map_spot->m_sDescText.SetText(*map_location.text);
			map_spot->m_sNameText.SetText(*map_location.name);
		}

		UILocalMapBackground.m_vMapSpots.push_back(CUIMapBackground::MapSpotPtr(map_spot));
		UILocalMapBackground.AttachChild(map_spot);
	}

	//	Добавить на карту актера только если мы отображаем текущую карту

	UILocalMapBackground.ConvertToLocal(pActor->Position(),P);
	CUIMapSpot* map_spot;

	map_spot = xr_new<CUIMapSpot>();
	map_spot->SetObjectID(pActor->ID());

	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	map_spot->m_fHeading = h;
	map_spot->m_bHeading = true;

	map_spot->InitTexture(pSettings->r_string("game_map", "actor_arrow"));
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);
	map_spot->SetObjectID(0xffff);

	map_spot->SetWndPos(P.x + left, P.y + top);
	map_spot->m_eAlign = CUIMapSpot::eCenter;
	map_spot->m_sDescText.SetText("It's you.");
	map_spot->m_sNameText.SetText("You");
	map_spot->m_vWorldPos.set(pActor->Position());
	map_spot->m_LevelName = Level().name();

	UILocalMapBackground.m_vMapSpots.push_back(CUIMapBackground::MapSpotPtr(map_spot));
	UILocalMapBackground.AttachChild(map_spot);
	m_pActorSpot = map_spot;

	//!!! предварительный рендер неизвестно зачем
//	m_pActorSpot->GetStaticItem()->Render();

	//информация о выбранном объекте
	UIStaticInfo.Show(false);
	UICharacterInfo.Show(false);

	this->ApplyFilterToObjectives(Level().name());    
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitGlobalMapObjectives()
{
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

	if (!pActor) return;
	LocalMapsPairIt mapIt;

	for (mapIt = m_LocalMaps.begin(); mapIt != m_LocalMaps.end(); ++mapIt)
	{
		LocalMapDef &l = mapIt->second;
		R_ASSERT				(l.mapSpot);
		l.obj.clear				();
		l.mapSpot->SetColor		(inactiveLocalMapColor);
		l.smallFrame->SetColor	(inactiveSmallLocalMapColor);
	}

	// Добавить отметки на карте, которые актер помнит в info_portions
	if(pActor->m_known_info_registry->registry().objects_ptr())
	{
		const KNOWN_INFO_VECTOR& know_info = *pActor->m_known_info_registry->registry().objects_ptr();
		for(KNOWN_INFO_VECTOR::const_iterator it = know_info.begin();
			know_info.end() != it; it++)
		{
			CInfoPortion info_portion;
			info_portion.Load((*it).id);

			// Добавить отметки на карте
			for(u32 i=0; i<info_portion.MapLocations().size(); i++)
			{
				const SMapLocation& map_location = info_portion.MapLocations()[i];
				if(xr_strlen(*map_location.level_name) > 0)
				{
					mapIt = m_LocalMaps.find(map_location.level_name);
					if (m_LocalMaps.end() != mapIt)
				 	{
						if(map_location.global_map_text)
							mapIt->second.obj.push_back(map_location.text);
						Irect r	= mapIt->second.smallFrame->GetWndRect();
						r.top	= globalMapButtonHeight - r.top;
						mapIt->second.visible = !(r.left == m_rDefault.left && r.top == m_rDefault.top);
					}
				}
			}
		}
	}

	// Даже если никаких локаций/квестов на карте на которой мы находимся нет, то все равно показать
	// карту
	mapIt = m_LocalMaps.find(Level().name());
	if (m_LocalMaps.end() != mapIt)
	{
		Irect r	= mapIt->second.smallFrame->GetWndRect();
		r.top	= globalMapButtonHeight - r.top;
		mapIt->second.visible = !(r.left == m_rDefault.left && r.top == m_rDefault.top);
		mapIt->second.smallFrame->SetColor(ourLevelMapColor);
		mapIt->second.mapSpot->SetColor(ourLevelMapColor);
	}

	// Показать ищвестные нам локации
	for (mapIt = m_LocalMaps.begin(); mapIt != m_LocalMaps.end(); ++mapIt)
	{
		LocalMapDef &l = mapIt->second;
		if (l.visible)
		{
			if (!UIGlobalMapBackground.IsChild(l.mapSpot.get()))
				UIGlobalMapBackground.AttachChild(l.mapSpot.get());
			if (!UIMapTypeSwitch.IsChild(l.smallFrame))
				UIMapTypeSwitch.AttachChild(l.smallFrame);
		}
		else
		{
			if (UIGlobalMapBackground.IsChild(l.mapSpot.get()))
				UIGlobalMapBackground.DetachChild(l.mapSpot.get());
			if (UIMapTypeSwitch.IsChild(l.smallFrame))
				UIMapTypeSwitch.DetachChild(l.smallFrame);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::AddObjectSpot(CGameObject* pGameObject)
{	
	//!!! пока только для CEntity (чтоб было откуда брать иконку)
	CEntity* pEntity = smart_cast<CEntity*>(pGameObject);
	if(!pEntity) return;


	int left	= UILocalMapBackground.GetWndRect().left;
	int top		= UILocalMapBackground.GetWndRect().top;

	Ivector2 P;
    UILocalMapBackground.ConvertToLocal(pGameObject->Position(),P);
	
	CUIMapSpot* map_spot;
	map_spot = xr_new<CUIMapSpot>();
	map_spot->SetObjectID(pEntity->ID());
	
	map_spot->SetShader(GetCharIconsShader());
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);
	map_spot->SetWndPos(P.x + left, P.y + top);
	map_spot->m_eAlign = CUIMapSpot::eCenter;
	map_spot->m_sDescText.SetText("");
	map_spot->m_sNameText.SetText("");
	map_spot->m_LevelName = Level().name();

	map_spot->GetUIStaticItem().SetOriginalRect(
						pEntity->GetMapIconX()*ICON_GRID_WIDTH + MAP_ICON_WIDTH / 2,
						pEntity->GetMapIconY()*ICON_GRID_HEIGHT + MAP_ICON_HEIGHT,
						(pEntity->GetMapIconX()+1)*ICON_GRID_WIDTH,
						(pEntity->GetMapIconY()+1)*ICON_GRID_HEIGHT);

	UILocalMapBackground.m_vMapSpots.push_back(CUIMapBackground::MapSpotPtr(map_spot));
	UILocalMapBackground.AttachChild(map_spot);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	static u16 prevScrollPosV = 0;
	static u16 prevScrollPosH = 0;

	CStringTable	stbl;

	if(pWnd == &UILocalMapBackground){
		if (MAPSPOT_FOCUS_RECEIVED == msg || MAPSPOT_ARROW_FOCUS_RECEIVED == msg){
			if (m_pCurrentMap->m_pActiveMapSpot){
				UIStaticInfo.Show(true);
				Irect r	= m_pCurrentMap->m_pActiveMapSpot->GetAbsoluteRect();
				Irect r2	= UIStaticInfo.GetParent()->GetAbsoluteRect();
				
				int left = r.right - r2.left;
				int top = r.top + 2 - r2.top;
				UIStaticInfo.SetWndPos(left, top);
			
				//CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_pCurrentMap->m_pActiveMapSpot->m_pObject);

				if(xr_strlen(m_pCurrentMap->m_pActiveMapSpot->m_sDescText.GetBuf())>1){
					UICharacterInfo.Show(true);
					UICharacterInfo.ResetAllStrings();

					UICharacterInfo.UIName.SetText(m_pCurrentMap->m_pActiveMapSpot->m_sNameText);
					UICharacterInfo.UIRank.SetText(m_pCurrentMap->m_pActiveMapSpot->m_sDescText);
					
				}else{
					UICharacterInfo.ResetAllStrings();
					UICharacterInfo.Show(false);
				}
            }
		}else if(MAPSPOT_FOCUS_LOST == msg || MAPSPOT_ARROW_FOCUS_LOST == msg){
			UIStaticInfo.Show(false);
		}
	}

	if (&UIGlobalMapBackground == pWnd){
		static CUIMapSpot *prevSpot = NULL;
		if (MAPSPOT_FOCUS_RECEIVED == msg){
			if (UIGlobalMapBackground.m_pActiveMapSpot && prevSpot != UIGlobalMapBackground.m_pActiveMapSpot){
				CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
				R_ASSERT(pGML);
				pGML->SetColor(activeLocalMapColor);
				UIMapName.SetText(*stbl(pGML->m_strMapName));
				// Show map goals
				for (Objectives_it it = m_LocalMaps[pGML->m_strMapName].obj.begin(); it != m_LocalMaps[pGML->m_strMapName].obj.end(); ++it)
				{
					UIMapGoals.AddItem<CUIListItem>(static_cast<const char *>(*(*it)));
				}
				prevSpot = UIGlobalMapBackground.m_pActiveMapSpot;
			}
			UIMapGoals.Show(true);
			UIMapName.Show(true);
		}

		if (MAPSPOT_FOCUS_LOST == msg){
			UIMapGoals.Show(false);
			UIMapGoals.RemoveAll();
			prevSpot = NULL;
			if (UIGlobalMapBackground.m_pActiveMapSpot){
				CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
				R_ASSERT(pGML);
				if (Level().name() == pGML->m_strMapName)
					pGML->SetColor(ourLevelMapColor);
				else
					pGML->SetColor(inactiveLocalMapColor);
			}
			UIMapName.Show(false);
		}

		if (MAPSPOT_CLICKED == msg){
			CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
			if (pGML){
				SetLocalMap(pGML->m_strMapName);
				SwitchMapMode(emmLocal);
			}
		}
	}

	if (&UIMapBgndV == pWnd){
		if (SCROLLBAR_VSCROLL == msg){
			m_pCurrentMap->MoveMap(0, -VSCROLLBAR_STEP * (UIMapBgndV.GetScrollPos() - prevScrollPosV));
			prevScrollPosV = UIMapBgndV.GetScrollPos();
			m_pCurrentMap->Update();
		}
	}

	if (&UIMapBgndH == pWnd){
		if (SCROLLBAR_HSCROLL == msg){
			m_pCurrentMap->MoveMap(-HSCROLLBAR_STEP * (UIMapBgndH.GetScrollPos() - prevScrollPosH), 0);
			prevScrollPosH = UIMapBgndH.GetScrollPos();
			m_pCurrentMap->Update();
		}
	}

	if (m_pCurrentMap == pWnd && MAP_MOVED == msg){
		Fvector2	dest;
		m_pCurrentMap->ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);
		if (UIMapBgndV.IsShown())
			UIMapBgndV.SetScrollPos(static_cast<s16>(UIMapBgndV.GetMaxRange() - iCeil((m_pCurrentMap->m_fMapHeightMeters - m_pCurrentMap->m_fMapViewHeightMeters - m_pCurrentMap->m_fMapY) / dest.y)));
		if (UIMapBgndH.IsShown())
			UIMapBgndH.SetScrollPos(static_cast<s16>(UIMapBgndH.GetMaxRange() - iCeil((m_pCurrentMap->m_fMapWidthMeters - m_pCurrentMap->m_fMapViewWidthMeters - m_pCurrentMap->m_fMapX) / dest.x)));

		prevScrollPosH = UIMapBgndH.GetScrollPos();
		prevScrollPosV = UIMapBgndV.GetScrollPos();
		m_pCurrentMap->Update();
	}
	
	if (&UIMapTypeSwitch == pWnd && BUTTON_CLICKED == msg){
		SwitchMapMode(emmGlobal);
		for (WINDOW_LIST_it it = UIMapTypeSwitch.GetChildWndList().begin();
							it != UIMapTypeSwitch.GetChildWndList().end();
							++it)
		{
			Irect r = (*it)->GetWndRect();
			(*it)->SetWndPos(r.left - UIMapTypeSwitch.GetPushOffsetX(), r.top - UIMapTypeSwitch.GetPushOffsetY());
		}
	}else if (&UIMapTypeSwitch == pWnd && BUTTON_DOWN == msg){
		for (WINDOW_LIST_it it = UIMapTypeSwitch.GetChildWndList().begin();
							it != UIMapTypeSwitch.GetChildWndList().end();
							++it)
		{
			Irect r = (*it)->GetWndRect();
			(*it)->SetWndPos(r.left + UIMapTypeSwitch.GetPushOffsetX(), r.top + UIMapTypeSwitch.GetPushOffsetY());
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Update()
{
	float h,p;

	Device.vCameraDirection.getHP	(h,p);
	m_pActorSpot->m_fHeading = h;
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
	m_pActorSpot->m_vWorldPos.set(pActor->Position());

	inherited::Update();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Draw()
{
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////
//	перевод из мировой системы координат в координаты карты
//	с началом координат в левом верхнем углу
//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = iFloor(0.5f + m_iMapWidth*(src.x - m_fWorldMapLeft)/m_fWorldMapWidth);
	dest.y = m_iMapHeight - iFloor(0.5f + m_iMapHeight*(src.z - m_fWorldMapTop)/m_fWorldMapHeight);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	UILocalMapBackground.SetActivePos(vNewPoint);
	UILocalMapBackground.m_bNoActorFocus = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SwitchMapMode(const EMapModes mode)
{
	// Проверяем на переключение в текущий режим
	if (mode == m_eCurrentMode) return;

	m_eCurrentMode = mode;

	CStringTable st;

	UIMainMapFrame.DetachChild(m_pCurrentMap);

	switch (m_eCurrentMode)
	{
	case emmLocal:
//		UIMapTypeSwitch.SetText(*st("Global Map"));
		m_pCurrentMap = &UILocalMapBackground;
		UILocalMapBackground.Show(true);
		SendInfoToActor("ui_pda_map_local");
		break;
	case emmGlobal:
//		UIMapTypeSwitch.SetText(*st("Local Map"));
		m_pCurrentMap = &UIGlobalMapBackground;
		UIGlobalMapBackground.Show(true);
		SendInfoToActor("ui_pda_map_global");

		for (LocalMapsPairIt it = m_LocalMaps.begin(); it != m_LocalMaps.end(); ++it)
		{
			if (Level().name() == it->first)
				it->second.mapSpot->SetColor(ourLevelMapColor);
			else
				it->second.mapSpot->SetColor(inactiveLocalMapColor);
		}
		break;
	default:
		NODEFAULT;
	}    

	UIMainMapFrame.AttachChild(m_pCurrentMap);


	const Fbox& level_box	= Level().ObjectSpace.GetBoundingVolume();

	m_fWorldMapWidth = level_box.x2 - level_box.x1;
	m_fWorldMapHeight = level_box.z2 - level_box.z1;

	m_fWorldMapLeft = level_box.x1;
	m_fWorldMapTop = level_box.z2;

	Fvector2	dest;
	m_pCurrentMap->ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);

	// Настраиваем скроллбоксы
	if (m_pCurrentMap->m_fMapHeightMeters > m_pCurrentMap->m_fMapViewHeightMeters)
	{
		UIMapBgndV.Show(true);
		UIMapBgndV.SetRange(0, static_cast<s16>(iCeil((m_pCurrentMap->m_fMapHeightMeters - m_pCurrentMap->m_fMapViewHeightMeters) / dest.y)));
	}
	else
		UIMapBgndV.Show(false);

	if (m_pCurrentMap->m_fMapWidthMeters > m_pCurrentMap->m_fMapViewWidthMeters)
	{
		UIMapBgndH.Show(true);
		UIMapBgndH.SetRange(0, static_cast<s16>(iCeil((m_pCurrentMap->m_fMapWidthMeters - m_pCurrentMap->m_fMapViewWidthMeters) / dest.x)));
	}
	else
		UIMapBgndH.Show(false);

	SendMessage(m_pCurrentMap, MAP_MOVED, NULL);

	m_iMapWidth = m_pCurrentMap->GetWidth();
	m_iMapHeight = m_pCurrentMap->GetHeight();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::AddLocalMap(const shared_str& levelName, const Frect &l, const Frect &g, shared_str textureName)
{
	// If level with this name already exists
	R_ASSERT2	(m_LocalMaps.end() == m_LocalMaps.find(levelName), "Duplicate level name not allowed");
	LocalMapDef &currentLevelMap = m_LocalMaps[levelName];

	// Имя текстуры фрейма для локальных карт
	const char * const localMapFrameTexture = "ui\\ui_frame_very_small";

	Ivector4	iTmpV;
	iTmpV[2]	= iFloor(v[2] * UIGlobalMapBackground.m_iMapViewWidthPixels	/ m_GlobalMapWidth);
	iTmpV[3]	= iFloor(v[3] * UIGlobalMapBackground.m_iMapViewHeightPixels	/ m_GlobalMapHeight);

	float		xSmallMapFactor		= m_GlobalMapWidth	/ globalMapButtonWidth;
	float		ySmallMapFactor		= m_GlobalMapHeight / globalMapButtonHeight;

	// Create local map rect on global map
	LocalMapDef::GMLptr pGML		(xr_new<CUIGlobalMapLocation>());
	pGML->Init(iTmpV[2], iTmpV[3], UIGlobalMapBackground.GetAbsoluteRect(), localMapFrameTexture);
	pGML->m_vWorldPos.x				= float(v[0]);
	pGML->m_vWorldPos.z				= float(v[1]);
	pGML->SetColor(inactiveLocalMapColor);
	pGML->m_strMapName				= CStringTable()(levelName);
	currentLevelMap.mapSpot			= pGML;
	currentLevelMap.level_box		= l;
	UIGlobalMapBackground.m_vMapSpots.push_back(pGML);

	// Add map locations to global map button
	iTmpV[0] = iFloor(v[0] / xSmallMapFactor);
	iTmpV[1] = iFloor(v[1] / ySmallMapFactor);
	iTmpV[2] = iFloor(v[2] / xSmallMapFactor);
	iTmpV[3] = iFloor(v[3] / ySmallMapFactor);

	currentLevelMap.smallFrame = xr_new<CUIFrameWindow>();
	currentLevelMap.smallFrame->Init(localMapFrameTexture, iTmpV[0], globalMapButtonHeight - iTmpV[1], iTmpV[2], iTmpV[3]);
	currentLevelMap.smallFrame->SetColor(inactiveLocalMapColor);

	// Initialize map itself
	currentLevelMap.mapTexture.create("hud\\default", *textureName);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitMaps()
{
	string256			gameLtxPath;
	FS.update_path		(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile gameLtx	(gameLtxPath);

	// Инициализируем параметры глобальной карты
	m_GlobalMapWidth	= gameLtx.r_float("ui_global_map", "width");
	m_GlobalMapHeight	= gameLtx.r_float("ui_global_map", "height");

	UIGlobalMapBackground.InitMapBackground(m_GlobalMapShader);
	
	// initialize local maps
	if (gameLtx.section_exist("ui_level_maps_single")){
		CInifile::Sect& S		= gameLtx.r_section("ui_level_maps_single");
		CInifile::SectIt	it	= S.begin(), end = S.end();
		for (;it!=end; it++){
			LPCSTR line	= *it->second;
			string_path	t_name;
			Frect		localMapCoords;
			Frect		globalMapCoords;
			u32 v_cnt	= sscanf(line,"%[^,],%f,%f,%f,%f,%f,%f,%f,%f",t_name,
							&localMapCoords.x1,&localMapCoords.y1,	&localMapCoords.x2,&localMapCoords.y2,
							&globalMapCoords.x1,&globalMapCoords.y1,&globalMapCoords.x2,&globalMapCoords.y2);
			R_ASSERT	(v_cnt==9);
			AddLocalMap	(it->first, localMapCoords, globalMapCoords, t_name);
		}
	}
}
//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::DeleteLocalMapsData()
{
	for (LocalMapsPairIt it = m_LocalMaps.begin(); it != m_LocalMaps.end(); ++it){
		if (it->second.smallFrame->GetParent())
			it->second.smallFrame->GetParent()->DetachChild(it->second.smallFrame);
		xr_delete(it->second.smallFrame);
	}

	m_LocalMaps.clear();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::ApplyFilterToObjectives(const shared_str &levelName)
{
	for (CUIMapBackground::MapSpotPtrVecIt mapSpotIt = UILocalMapBackground.m_vMapSpots.begin();
											  mapSpotIt != UILocalMapBackground.m_vMapSpots.end();
											  ++mapSpotIt)
	{
		(*mapSpotIt)->Show(levelName == (*mapSpotIt)->m_LevelName);
	}	
}
void CUIMapWnd::OnMouseWheel(int direction)
{
	if(direction<0)
		UIMapBgndV.TryScrollInc();
	else
		UIMapBgndV.TryScrollDec();
}
*/