////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "movement_manager_space.h"
#include "game_location_selector.h"
#include "level_location_selector.h"
#include "game_path_manager.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "enemy_location_predictor.h"
#include "patrol_path_manager.h"
#include "xrmessages.h"
#include "ai_object_location.h"

using namespace MovementManager;

const float verify_distance = 15.f;

CMovementManager::CMovementManager	()
{
	m_dwFrameLoad				= u32(-1);
	m_dwFrameReinit				= u32(-1);
	m_dwFrameReload				= u32(-1);
	m_dwFrameNetSpawn			= u32(-1);
	m_dwFrameNetDestroy			= u32(-1);

	m_base_game_selector		= xr_new<CBaseParameters>();
	m_base_level_selector		= xr_new<CBaseParameters>();
	m_game_location_selector	= xr_new<CGameLocationSelector	>();
	m_game_path_manager			= xr_new<CGamePathManager		>();
	m_level_location_selector	= xr_new<CLevelLocationSelector	>();
	m_level_path_manager		= xr_new<CLevelPathManager		>();
	m_detail_path_manager		= xr_new<CDetailPathManager		>();
	m_patrol_path_manager		= xr_new<CPatrolPathManager		>();
	m_enemy_location_predictor	= xr_new<CEnemyLocationPredictor>();

	extrapolate_path			(false);
}

CMovementManager::~CMovementManager	()
{
	xr_delete					(m_base_game_selector		);
	xr_delete					(m_base_level_selector		);
	xr_delete					(m_game_location_selector	);
	xr_delete					(m_game_path_manager		);
	xr_delete					(m_level_location_selector	);
	xr_delete					(m_level_path_manager		);
	xr_delete					(m_detail_path_manager		);
	xr_delete					(m_patrol_path_manager		);
	xr_delete					(m_enemy_location_predictor	);
}

void CMovementManager::Load			(LPCSTR section)
{
	if (!frame_check(m_dwFrameLoad))
		return;

	CRestrictedObject::Load			(section);
	CPhysicsShellHolder::Load		(section);
}

void CMovementManager::reinit		()
{
	if (!frame_check(m_dwFrameReinit))
		return;
	
	CRestrictedObject::reinit		();
	CPhysicsShellHolder::reinit		();
	
	m_time_work								= 300*CPU::cycles_per_microsec;
	m_speed									= 0.f;
	m_path_type								= ePathTypeNoPath;
	m_path_state							= ePathStateDummy;
	m_path_actuality						= true;
	m_speed									= 0.f;
	m_selector_path_usage					= false;//true;
	m_old_desirable_speed					= 0.f;
	m_refresh_rate							= 0;
	m_last_update							= Level().timeServer();
	m_build_at_once							= false;

	enable_movement								(true);
	game_location_selector().reinit				(this,&ai().game_graph());
	level_location_selector().reinit			(this,ai().get_level_graph());
	detail_path_manager().reinit				(this);
	game_path_manager().reinit					(this,&ai().game_graph());
	level_path_manager().reinit					(this,ai().get_level_graph());
	patrol_path_manager().reinit				(this);
	enemy_location_predictor().reinit			(this);

	game_location_selector().set_dest_path		(game_path_manager().m_path);
	level_location_selector().set_dest_path		(level_path_manager().m_path);
	level_location_selector().set_dest_vertex	(level_path_manager().m_dest_vertex_id);
}

void CMovementManager::reload		(LPCSTR section)
{
	if (!frame_check(m_dwFrameReload))
		return;
	
	CRestrictedObject::reload		(section);
	CPhysicsShellHolder::reload		(section);
}

BOOL CMovementManager::net_Spawn	(LPVOID data)
{
	if (!frame_check(m_dwFrameNetSpawn))
		return						(TRUE);

	return							(CRestrictedObject::net_Spawn(data) && CPhysicsShellHolder::net_Spawn(data));
}

void CMovementManager::net_Destroy	()
{
	if (!frame_check(m_dwFrameNetDestroy))
		return;

	CRestrictedObject::net_Destroy	();
	CPhysicsShellHolder::net_Destroy();
}

void CMovementManager::Hit			(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	CPhysicsShellHolder::Hit		(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

CMovementManager::EPathType CMovementManager::path_type() const
{
	VERIFY					(m_path_type != MovementManager::ePathTypeDummy);
	return					(m_path_type);
}

void CMovementManager::set_game_dest_vertex	(const ALife::_GRAPH_ID game_vertex_id)
{
	game_path_manager().set_dest_vertex(game_vertex_id);
	m_path_actuality		= m_path_actuality && game_path_manager().actual();
}

ALife::_GRAPH_ID CMovementManager::game_dest_vertex_id() const
{
	return					(ALife::_GRAPH_ID(game_path_manager().dest_vertex_id()));
}

void CMovementManager::set_level_dest_vertex(const u32 level_vertex_id)
{
	VERIFY					(accessible(level_vertex_id));
	level_path_manager().set_dest_vertex(level_vertex_id);
	m_path_actuality		= m_path_actuality && level_path_manager().actual();
}

u32	 CMovementManager::level_dest_vertex_id() const
{
	return					(level_path_manager().dest_vertex_id());
}

const xr_vector<DetailPathManager::STravelPathPoint>	&CMovementManager::path	() const
{
	return					(detail_path_manager().path());
}

void CMovementManager::update_path	()
{
	if (!enabled() || ((/**actual_all() || /**/(m_last_update > Level().timeServer())) && !path_completed()))
		return;

	m_last_update			= Level().timeServer() + m_refresh_rate;

	time_start				();
	
	if (!game_path_manager().evaluator()) 		
		game_path_manager().set_evaluator	(base_game_selector());

	if (!level_path_manager().evaluator()) 
		level_path_manager().set_evaluator	(base_level_selector());

	if (!actual()) {

		game_path_manager().make_inactual();
		level_path_manager().make_inactual();
		patrol_path_manager().make_inactual();
		switch (m_path_type) {
			case ePathTypeGamePath : {
				m_path_state	= ePathStateSelectGameVertex;
				break;
			}
			case ePathTypeLevelPath : {
				m_path_state	= ePathStateSelectLevelVertex;
				break;
			}
			case ePathTypeEnemySearch : {
				m_path_state	= ePathStatePredictEnemyVertices;
				break;
			}
			case ePathTypePatrolPath : {
				m_path_state	= ePathStateSelectPatrolPoint;
				break;
			}
			case ePathTypeNoPath : {
				m_path_state	= ePathStateDummy;
				break;
			}
			default :			NODEFAULT;
		}
		m_path_actuality	= true;
	}

	switch (m_path_type) {
		case ePathTypeGamePath : {
			process_game_path	();
			break;
		}
		case ePathTypeLevelPath : {
			process_level_path	();
			break;
		}
		case ePathTypeEnemySearch : {
			process_enemy_search();
			break;
									}
		case ePathTypePatrolPath : {
			process_patrol_path();
			break;
		}
		case ePathTypeNoPath : {
			break;
		}
		default :				NODEFAULT;
	}

	if (accessible(Position()))
		verify_detail_path		();

	m_build_at_once				= false;
}

bool CMovementManager::actual_all() const
{
	if (!m_path_actuality)
		return			(false);
	switch (m_path_type) {
		case ePathTypeGamePath : 
			return			(
				game_path_manager().actual() && 
				level_path_manager().actual() &&
				detail_path_manager().actual()
			);
		case ePathTypeLevelPath :
			return			(
				level_path_manager().actual() &&
				detail_path_manager().actual()
			);
		case ePathTypeEnemySearch :
			return			(
//				enemy_location_predictor().actual() && 
				level_path_manager().actual() &&
				detail_path_manager().actual()
			);
		case ePathTypePatrolPath :
			return			(
				patrol_path_manager().actual() && 
				level_path_manager().actual() &&
				detail_path_manager().actual()
			);
		case ePathTypeNoPath :
			return			(
				detail_path_manager().actual()
			);
		default : NODEFAULT;
	}
#ifdef DEBUG
	return					(true);
#endif
}

void CMovementManager::verify_detail_path	()
{
	if (detail_path_manager().path().empty() || detail_path_manager().completed(detail_path_manager().dest_position()))
		return;

	float distance = 0.f;
	for (u32 i=detail_path_manager().curr_travel_point_index() + 1, n=detail_path_manager().path().size(); i<n; ++i) {
		if (!accessible(detail_path_manager().path()[i].position,EPS_L)) {
			m_path_actuality	= false;
			return;
		}
		distance	+= detail_path_manager().path()[i].position.distance_to(detail_path_manager().path()[i-1].position);
		if (distance >= verify_distance)
			break;
	}
}

void CMovementManager::teleport				(u32 game_vertex_id)
{
	NET_Packet			net_packet;
	ALife::_GRAPH_ID	_game_vertex_id = (ALife::_GRAPH_ID)game_vertex_id;
	u32					_level_vertex_id = ai().game_graph().vertex(_game_vertex_id)->level_vertex_id();
	Fvector				position = ai().game_graph().vertex(_game_vertex_id)->level_point();
	u_EventGen			(net_packet,GE_TELEPORT_OBJECT,ID());
	net_packet.w		(&_game_vertex_id,sizeof(_game_vertex_id));
	net_packet.w		(&_level_vertex_id,sizeof(_level_vertex_id));
	net_packet.w_vec3	(position);
	Level().Send		(net_packet,net_flags(TRUE,TRUE));
}

void CMovementManager::UpdateCL			()
{
	CPhysicsShellHolder::UpdateCL();
}