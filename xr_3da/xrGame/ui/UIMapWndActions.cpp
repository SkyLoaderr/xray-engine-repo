#include "stdafx.h"
#include "UIMapWndActions.h"
#include "UIMapWndActionsSpace.h"
#include "UIMap.h"
#include "UIMapWnd.h"


using namespace UIMapWndActionsSpace;
const float			map_changing_time			= 1.0f;//sec
const float			map_moving_speed			= 0.003f;

CMapActionPlanner::CMapActionPlanner	()
{}

CMapActionPlanner::~CMapActionPlanner	()
{}

LPCSTR CMapActionPlanner::object_name	() const
{
	return						("");
}


void CMapActionPlanner::setup			(CUIMapWnd *object)
{
#ifdef LOG_ACTION
	set_use_log						(false);
#endif
	inherited::setup				(object);
	clear							();
	
	m_storage.set_property			(0, false);
	m_storage.set_property			(1, false);

//GlobalMap
	add_evaluator				(make_map_id(GLOBAL_MAP_IDX,ePropMapOpened),	xr_new<CEvaluatorGlobalMapOpened>(object->GlobalMap(),GLOBAL_MAP_IDX,"ePropGlobalMapOpened"));
	add_evaluator				(make_map_id(GLOBAL_MAP_IDX,ePropMapOpenedIdle),xr_new<CMapEvaluatorConst>		(object->GlobalMap(),false,GLOBAL_MAP_IDX,"ePropMapOpenedIdle"));

	_world_operator				*action = xr_new<CMapActionMapIdle>((CUILevelMap*)NULL,GLOBAL_MAP_IDX,"eOperatorGlobalMapOpenedIdle");
	add_condition				(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpened),		true);
	add_condition				(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpenedIdle),	false);
	add_effect					(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpenedIdle),	true);
	add_operator				(make_map_id(GLOBAL_MAP_IDX,eOperatorMapOpenIdle),action);

//LevelMaps
	const GameMaps& maps			= object->GameMaps();
	u32 sz = maps.size();
	for(u16 idx=0; idx<sz;++idx){
		CUILevelMap* map			= smart_cast<CUILevelMap*>(object->GetMapByIdx(idx));
		add_evaluators				(idx, map);
		add_actions					(idx, map);
	}



//foo
	add_evaluator				(ePropFoo,	xr_new<CMapEvaluatorConst>		((CUILevelMap*)NULL,false,u16(0),"ePropFoo"));
								action = xr_new<CSomeMapAction>		((CUILevelMap*)NULL,u16(0),"ePropFoo");
	add_condition				(action, ePropFoo, false);
	add_effect					(action, ePropFoo, true);
	add_operator				(eOperatorFoo,action);


	CWorldState					target_state;
	target_state.add_condition	(CWorldProperty(ePropFoo,true));
	set_target_state			(target_state);
}


void CMapActionPlanner::add_evaluators	(u16 map_idx, CUILevelMap* map)
{
	add_evaluator				(make_map_id(map_idx,ePropMapOpened),	 xr_new<CEvaluatorMapOpened>			(map,map_idx,"ePropMapOpened"));
	add_evaluator				(make_map_id(map_idx,ePropMapOpenedIdle),xr_new<CMapEvaluatorConst>				(map,false,map_idx,"ePropMapOpenedIdle"));
	add_evaluator				(make_map_id(map_idx,ePropGlobalMapCentered),xr_new<CEvaluatorGlobalMapCentered>(map,map_idx,"ePropGlobalMapCentered"));
	add_evaluator				(make_map_id(map_idx,ePropLocalMapCentered),xr_new<CEvaluatorLocalMapCentered>	(map,map_idx,"ePropLocalMapCentered"));
}

void CMapActionPlanner::add_actions		(u16 map_idx, CUILevelMap* map)
{
	//open
	_world_operator				*action = xr_new<CMapActionOpen>(map,map_idx,"eOperatorMapOpen");
	add_condition				(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpened),	true);
	add_condition				(action,make_map_id(map_idx,ePropGlobalMapCentered),true);
	add_condition				(action,make_map_id(map_idx,ePropMapOpened),		false);
	add_effect					(action,make_map_id(map_idx,ePropMapOpened),		true);
	add_effect					(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpened), false);
	add_operator				(make_map_id(map_idx,eOperatorMapOpen),action);


	//close
								action = xr_new<CMapActionClose>(map,map_idx,"eOperatorMapClose");
	add_condition				(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpened),	false);
	add_condition				(action,make_map_id(map_idx,ePropMapOpened),		true);
	add_effect					(action,make_map_id(map_idx,ePropMapOpened),		false);
	add_effect					(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpened),	true);
	add_operator				(make_map_id(map_idx,eOperatorMapClose),action);

	//center global
								action = xr_new<CMapActionGlobalMapCenter>(map,map_idx,"eOperatorGlobalMapCenter");
	add_condition				(action,make_map_id(GLOBAL_MAP_IDX,ePropMapOpened),	true);
	add_condition				(action,make_map_id(map_idx,ePropGlobalMapCentered),false);
	add_effect					(action,make_map_id(map_idx,ePropGlobalMapCentered),true);
	add_operator				(make_map_id(map_idx,eOperatorGlobalMapCenter),action);

	//center local
								action = xr_new<CMapActionLocalMapCenter>(map,map_idx,"eOperatorLocalMapCenter");
	add_condition				(action,make_map_id(map_idx,ePropMapOpened),true);
	add_condition				(action,make_map_id(map_idx,ePropLocalMapCentered),false);
	add_effect					(action,make_map_id(map_idx,ePropLocalMapCentered),true);
	add_operator				(make_map_id(map_idx,eOperatorLocalMapCenter),action);


								action = xr_new<CMapActionMapIdle>((CUILevelMap*)NULL,map_idx,"ePropMapOpenedIdle");
	add_condition				(action,make_map_id(map_idx,ePropMapOpened),		true);
	add_condition				(action,make_map_id(map_idx,ePropLocalMapCentered),	true);
	add_condition				(action,make_map_id(map_idx,ePropMapOpenedIdle),	false);
	add_effect					(action,make_map_id(map_idx,ePropMapOpenedIdle),	true);
	add_operator				(make_map_id(map_idx,eOperatorMapOpenIdle),action);

}

// actions and evaluators implementation
CSomeMapAction::CSomeMapAction(CUILevelMap *object, u16 idx, LPCSTR action_name)
:inherited((CUIMapWnd*)NULL, action_name), m_map(object)
{
#ifdef LOG_ACTION
	name.sprintf("%d-%s", idx, action_name);
	m_action_name = *name;
#endif
}
CSomeMapEvaluator::CSomeMapEvaluator(CUICustomMap *object, u16 idx, LPCSTR evaluator_name)
:inherited((CUIMapWnd*)NULL,evaluator_name),m_map(object)
{	
#ifdef LOG_ACTION
	name.sprintf("%d-%s", idx, evaluator_name); 
	m_evaluator_name = *name; 
#endif
};

void calcRectLerp(float time_to, const Frect& desired_rect, Frect& current_rect)
{
	float gdt				= Device.fTimeDelta;
	gdt						= _min(gdt,time_to);

	current_rect.x1		+= ((desired_rect.x1-current_rect.x1)/time_to)*gdt;
	current_rect.y1		+= ((desired_rect.y1-current_rect.y1)/time_to)*gdt;
	current_rect.x2		+= ((desired_rect.x2-current_rect.x2)/time_to)*gdt;
	current_rect.y2		+= ((desired_rect.y2-current_rect.y2)/time_to)*gdt;
}

void CMapActionOpenClose::execute		()
{
	inherited::execute();
	if(m_endMovingTime > Device.fTimeGlobal){
		float gt				= Device.fTimeGlobal;
		float time_to			= m_endMovingTime-gt;
		float k					= _sqrt((gt-m_startMovingTime)/time_to);
		//levelMap
		if(!m_bOpening)
			k = 1.0f - k;
		u32 clr					= color_rgba_f(1.0f,1.0f,1.0f,k);
		m_map->SetColor			(clr);

		Frect curr_map_rect		= m_map->GetWndRect();
		calcRectLerp			(time_to,m_desiredLevelMapRect,curr_map_rect);
		m_map->SetWndRect		(curr_map_rect);

		//GlobalMap	
		CUIGlobalMap*			gm = m_object->GlobalMap();
		curr_map_rect			= gm->GetWndRect();
		calcRectLerp			(time_to,m_desiredGlobalMapRect,curr_map_rect);
		gm->SetWndRect			(curr_map_rect);
	}else{
		float k = 1.0f;
		if(!m_bOpening)
			k = 1.0f - k;

		u32 clr					= color_rgba_f(1.0f,1.0f,1.0f,k);
		//levelMap		
		m_map->SetColor			(clr);
		m_map->SetWndRect		(m_desiredLevelMapRect);
		
		//GlobalMap	
		CUIGlobalMap*			gm = m_object->GlobalMap();
		gm->SetWndRect			(m_desiredGlobalMapRect);
		
		if(!m_bOpening)
			m_object->SetActiveMap	(gm);
		else
			m_object->SetActiveMap	(m_map);

	}	
}

void CMapActionOpen::initialize()
{
	inherited::initialize();
	m_object->RemoveMapToRender		(m_object->ActiveMap());

	m_object->AddMapToRender		(m_map);

	Frect rect_on_global			= m_map->CalcWndRectOnGlobal();
	m_map->SetWndRect				(rect_on_global);
	m_map->m_prevRect				= rect_on_global;

	Fvector2 						destLevelMapCP = m_map->TargetCenter();
	float map_zoom					= 1.0f;
	m_map->CalcOpenRect				(destLevelMapCP, map_zoom, m_desiredLevelMapRect, m_desiredGlobalMapRect);
	
	m_object->GlobalMap()->m_prevRect = m_object->GlobalMap()->GetWndRect();//store
	
	m_startMovingTime				= Device.fTimeGlobal;
	m_endMovingTime					= m_startMovingTime + map_changing_time;
	m_map->SetLocked				(true);
	m_object->GlobalMap()->SetLocked(true);
}


void CMapActionOpen::finalize		()
{
	inherited::finalize				();
	m_map->SetLocked				(false);
	m_object->GlobalMap()->SetLocked(false);
	m_storage->set_property(0, false);
}


void CMapActionClose::initialize()
{
	inherited::initialize			();

	m_desiredLevelMapRect			= m_map->m_prevRect;
	
	m_desiredGlobalMapRect			= m_object->GlobalMap()->m_prevRect;

	m_startMovingTime				= Device.fTimeGlobal;
	m_endMovingTime					= m_startMovingTime + map_changing_time;
	m_map->SetLocked				(true);
	m_object->GlobalMap()->SetLocked(true);

	m_storage->set_property(0, false);
}



void CMapActionClose::finalize	()
{
	inherited::finalize			();

	m_map->SetLocked				(false);
	m_object->GlobalMap()->SetLocked(false);
	m_object->RemoveMapToRender		(m_map);
}

void CMapActionGlobalMapCenter::initialize	()
{
	inherited::initialize			();

	CUIGlobalMap* globalMap			= m_object->GlobalMap();
	Fvector2						level_rect_center;
	Fvector2						global_map_center;
	Fvector2						global_map_dest_center;
	Frect							r;
	r								= m_map->GlobalRect();
	r.getcenter						(level_rect_center);

	global_map_dest_center			= globalMap->ConvertRealToLocal(level_rect_center);
	global_map_dest_center.add		(globalMap->GetAbsolutePos());
	r								= m_object->ActiveMapRect();
	r.getcenter						(global_map_center);

	m_desiredGlobalMapRect			= globalMap->GetWndRect();
	m_desiredGlobalMapRect.add		(	global_map_center.x - global_map_dest_center.x,
										global_map_center.y - global_map_dest_center.y);

	float _t =						global_map_dest_center.distance_to(global_map_center);
	_t								*=map_moving_speed;
	m_startMovingTime				= Device.fTimeGlobal;
	m_endMovingTime					= m_startMovingTime+_t;
	globalMap->SetLocked			(true);
}

void CMapActionGlobalMapCenter::execute						()
{
	inherited::execute			();

	CUIGlobalMap* globalMap		= m_object->GlobalMap();

	if(m_endMovingTime > Device.fTimeGlobal){
		float gt				= Device.fTimeGlobal;
		float time_to			= m_endMovingTime-gt;

		Frect					curr_map_rect;
		curr_map_rect			= globalMap->GetWndRect();
		calcRectLerp			(time_to,m_desiredGlobalMapRect,curr_map_rect);
		globalMap->SetWndRect	(curr_map_rect);
	}else{
		globalMap->SetWndRect	(m_desiredGlobalMapRect);
	}
}

void CMapActionGlobalMapCenter::finalize	()
{
	inherited::finalize					();

	m_object->GlobalMap()->SetLocked	(false);
	m_storage->set_property				(0, true);
}

void CMapActionMapIdle::initialize			()
{
	m_storage->set_property				(0, true);
	m_storage->set_property				(1, true);
}

void CMapActionMapIdle::finalize			()
{
}

void CMapActionMapIdle::execute				()
{
}

void CMapActionLocalMapCenter::initialize	()
{
	inherited::initialize			();

	m_desiredLevelMapRect			= m_map->GetWndRect();

	Fvector2 ppp					= m_map->ConvertRealToLocalNoTransform(m_map->TargetCenter());
	ppp.add							(m_map->GetAbsolutePos());
	Frect	vis_abs_rect			= m_object->ActiveMapRect();
	float dw						= vis_abs_rect.lt.x + vis_abs_rect.width()/2.0f - ppp.x;
	float dh						= vis_abs_rect.lt.y + vis_abs_rect.height()/2.0f - ppp.y;
	m_desiredLevelMapRect.add		(dw,dh);

	float _t =						_sqrt(dw*dw+dh*dh);
	_t								*=map_moving_speed;
	m_startMovingTime				= Device.fTimeGlobal;
	m_endMovingTime					= m_startMovingTime+_t;
}

void CMapActionLocalMapCenter::finalize		()
{
	inherited::finalize			();
}

void CMapActionLocalMapCenter::execute		()
{
	inherited::execute			();
	if(m_endMovingTime > Device.fTimeGlobal){
		float gt				= Device.fTimeGlobal;
		float time_to			= m_endMovingTime-gt;

		Frect					curr_map_rect;
		curr_map_rect			= m_map->GetWndRect();
		calcRectLerp			(time_to,m_desiredLevelMapRect,curr_map_rect);
		m_map->SetWndRect	(curr_map_rect);
	}else{
		m_map->SetWndRect	(m_desiredLevelMapRect);
	}
}


bool CEvaluatorMapOpened::evaluate()
{
	return (m_map == m_object->ActiveMap());
}

bool CEvaluatorGlobalMapOpened::evaluate()
{
	return ( m_map == m_object->ActiveMap() );
}

bool CEvaluatorGlobalMapCentered::evaluate	()
{
	if(m_storage->property(0)) return true;
	CUIGlobalMap* globalMap			= m_object->GlobalMap();
	CUILevelMap*  levelMap			= (CUILevelMap*)m_map; VERIFY(smart_cast<CUILevelMap*>(m_map));
	Fvector2						level_rect_center;
	Fvector2						visible_center;
	Fvector2						global_map_dest_center;
	Frect							r;
	r								= levelMap->GlobalRect();
	r.getcenter						(level_rect_center);
	

	global_map_dest_center			= globalMap->ConvertRealToLocal(level_rect_center);
	global_map_dest_center.add		(globalMap->GetAbsolutePos());

	r								= m_object->ActiveMapRect();
	r.getcenter						(visible_center);

	float _dist =					global_map_dest_center.distance_to(visible_center);
	return (_dist<5.0f);
}

bool CEvaluatorLocalMapCentered::evaluate		()
{
	if(m_storage->property(1)) return true;
	CUILevelMap*  levelMap			= (CUILevelMap*)m_map; VERIFY(smart_cast<CUILevelMap*>(m_map));

	Fvector2 ppp					= levelMap->ConvertRealToLocalNoTransform(levelMap->TargetCenter());
	ppp.add							(levelMap->GetAbsolutePos());
	Frect	vis_abs_rect			= m_object->ActiveMapRect();
	float dw						= vis_abs_rect.lt.x + vis_abs_rect.width()/2.0f - ppp.x;
	float dh						= vis_abs_rect.lt.y + vis_abs_rect.height()/2.0f - ppp.y;

	return (_abs(dw)<5.0f && _abs(dh)<5.0f);
}