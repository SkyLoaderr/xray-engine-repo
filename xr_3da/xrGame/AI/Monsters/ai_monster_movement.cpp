#include "stdafx.h"
#include "ai_monster_movement.h"
#include "ai_monster_utils.h"
#include "BaseMonster/base_monster.h"
#include "ai_monster_motion_stats.h"
#include "../../phmovementcontrol.h"
#include "ai_monster_debug.h"
#include "../../cover_evaluators.h"

//////////////////////////////////////////////////////////////////////////
// Construction / Destruction
//////////////////////////////////////////////////////////////////////////

CMonsterMovement::CMonsterMovement()
{
	m_selector_approach		= xr_new<CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
	m_cover_approach		= xr_new<CCoverEvaluatorCloseToEnemy>(this);
	
	m_object				= 0;

	m_dwFrameReinit			= u32(-1);
	m_dwFrameLoad			= u32(-1);
}

CMonsterMovement::~CMonsterMovement()
{
	xr_delete(m_selector_approach);
	xr_delete(MotionStats);
	xr_delete(m_cover_approach);
}

void CMonsterMovement::Load(LPCSTR section)
{
	if (!frame_check(m_dwFrameLoad))
		return;
	
	inherited::Load(section);

	m_selector_approach->Load(section,"selector_approach");
}

void CMonsterMovement::reinit()
{
	if (!frame_check(m_dwFrameReinit))
		return;
	
	inherited::reinit();

	b_try_min_time					= false;
	time_last_approach				= 0;
	b_use_dest_orient				= false;

	m_velocity_linear.set			(0.f,0.f);
	m_velocity_angular				= 0.f;

	initialize_movement				();
}

void CMonsterMovement::InitExternal(CBaseMonster *pM)
{
	m_object	= pM;
	VERIFY		(m_object);

	MotionStats	= xr_new<CMotionStats> (m_object);
}

//////////////////////////////////////////////////////////////////////////
// Services

void CMonsterMovement::InitSelector(CAbstractVertexEvaluator &S, Fvector target_pos)
{
	S.m_dwCurTime		= m_object->m_dwCurrentTime;
	S.m_tMe				= m_object;
	S.m_tpMyNode		= level_vertex();
	S.m_tMyPosition		= Position();
	S.m_dwStartNode		= level_vertex_id();
	S.m_tStartPosition	= Position();

	S.m_tEnemyPosition	= target_pos;
	S.m_tEnemy			= 0;
}

bool CMonsterMovement::IsMoveAlongPathFinished()
{
	return (CDetailPathManager::completed(Position()));
}

bool CMonsterMovement::IsMovingOnPath()
{
	return (!IsMoveAlongPathFinished() && CMovementManager::enabled() && b_enable_movement);
}

//-------------------------------------------------------------------------------------------

bool CMonsterMovement::IsPathEnd(u32 n_points)
{
	if (CDetailPathManager::path().empty() || !IsMovingOnPath() || (curr_travel_point_index() + n_points >= CDetailPathManager::path().size())) return true;
	return false;
}

bool CMonsterMovement::IsPathEnd(float dist_to_end)
{
	if (CDetailPathManager::path().size() < 2) return true;

	float cur_dist_to_end = 0.f;
	for (u32 i=CDetailPathManager::curr_travel_point_index(); i<CDetailPathManager::path().size()-1; i++) {
		cur_dist_to_end += CDetailPathManager::path()[i].position.distance_to(CDetailPathManager::path()[i+1].position);
		if (cur_dist_to_end > dist_to_end) break;
	}

	if (!IsMovingOnPath() || (cur_dist_to_end < dist_to_end)) return true;
	return false;
}

bool CMonsterMovement::IsPathEnd(u32 n_points, float dist_to_end)
{
	return (IsPathEnd(n_points) || IsPathEnd(dist_to_end));
}

//-------------------------------------------------------------------------------------------


bool CMonsterMovement::ObjectNotReachable(const CEntity *entity) 
{
	if (!object_position_valid(entity)) return true;
	
	// Commented because of Acceleration Conflict
	//if (m_object->MotionMan.BadMotionFixed()) return true;

	return false;
}


void CMonsterMovement::SetPathParams(u32 dest_vertex_id, const Fvector &dest_pos)
{
	set_level_dest_vertex(dest_vertex_id);
	set_dest_position(dest_pos);
	set_path_type (MovementManager::ePathTypeLevelPath);
}

void CMonsterMovement::SetSelectorPathParams()
{
	set_path_type (MovementManager::ePathTypeLevelPath);
	
	// использовать при установке селектора: true - использовать путь найденный селектором, false - селектор находит тольтко ноду, путь строит BuildLevelPath
	use_selector_path(true);

}

void CMonsterMovement::update_velocity()
{
	// Обновить линейную скорость движения
	float t_accel = ((m_velocity_linear.target < m_velocity_linear.current) ? 
										m_object->MotionMan.accel_get(eAV_Braking) :
										m_object->MotionMan.accel_get(eAV_Accel));
		
	velocity_lerp		(m_velocity_linear.current, m_velocity_linear.target, t_accel, Device.fTimeDelta);
	
	// установить линейную скорость движения
	set_desirable_speed	(m_object->m_fCurSpeed = m_velocity_linear.current);

	// установить угловую скорость движения
	if (!fis_zero(m_velocity_linear.current) && !fis_zero(m_velocity_linear.target))
		m_body.speed	= m_velocity_angular * m_velocity_linear.current / (m_velocity_linear.target + EPS_L);
	else 
		m_body.speed	= m_velocity_angular;
}

void CMonsterMovement::set_dest_direction(const Fvector &dir)
{
	CDetailPathManager::set_dest_direction(dir);
}

void CMonsterMovement::stop_now()
{
	enable_movement						(false);
	disable_path						();
	m_object->m_velocity_linear.target	= 0.f;
	m_object->m_velocity_linear.current = 0.f;
}

//////////////////////////////////////////////////////////////////////////
// Special Build Path
//////////////////////////////////////////////////////////////////////////
bool CMonsterMovement::build_special(const Fvector &target, u32 node, u32 vel_mask, bool linear)
{
	if (node == u32(-1)) {
		// нода в прямой видимости?
		CRestrictedObject::add_border(Position(), target);
		node = ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),target);
		CRestrictedObject::remove_border();
		
		if (!ai().level_graph().valid_vertex_id(node) || !accessible(node)) return false;
	}
	
	enable_movement									(true);
	
	set_velocity_mask								(vel_mask);	
	set_desirable_mask								(vel_mask);

	CDetailPathManager::set_try_min_time			(false); 
	CDetailPathManager::set_use_dest_orientation	(false);
	
	CLevelLocationSelector::set_evaluator			(0);
	CDetailPathManager::set_path_type				(eDetailPathTypeSmooth);
	set_path_type									(MovementManager::ePathTypeLevelPath);

	set_dest_position								(target);
	set_level_dest_vertex							(node);
	
	set_build_path_at_once							();
	update_path										();	
	
	if (is_path_built())							return true;

	enable_movement									(false);
	return false;
}

//////////////////////////////////////////////////////////////////////////

float CMonsterMovement::get_path_angle()
{
	if (CDetailPathManager::path().size() < 3) return 0.f;

	Fvector		dir;
	float		h1,h2,p;

	dir.sub		(CDetailPathManager::path()[1].position, CDetailPathManager::path()[0].position);
	dir.getHP	(h1,p);
	dir.sub		(CDetailPathManager::path()[CDetailPathManager::path().size() - 1].position, CDetailPathManager::path()[0].position);
	dir.getHP	(h2,p);

	return	(angle_difference(h1,h2));
}

bool CMonsterMovement::is_path_built()
{
	return (!CMovementManager::path_completed() && (CDetailPathManager::time_path_built() >= Level().timeServer()));
}

void CMonsterMovement::set_velocity_from_path() 
{
	if (!IsMovingOnPath()) return;
	
	u32 cur_point_velocity_index	= CDetailPathManager::path()[curr_travel_point_index()].velocity;
	u32 next_point_velocity_index	= u32(-1);

	if (CDetailPathManager::path().size() > curr_travel_point_index() + 1) 
		next_point_velocity_index = CDetailPathManager::path()[curr_travel_point_index() + 1].velocity;

	const CDetailPathManager::STravelParams &current_velocity	= CDetailPathManager::velocity(cur_point_velocity_index);
	if (fis_zero(current_velocity.linear_velocity) && (next_point_velocity_index != u32(-1))) {
		const CDetailPathManager::STravelParams &next_velocity	= CDetailPathManager::velocity(next_point_velocity_index);
		m_velocity_linear.target	= _abs(next_velocity.linear_velocity);
		m_velocity_angular			= next_velocity.real_angular_velocity;
	} else {
		m_velocity_linear.target	= _abs(current_velocity.linear_velocity);
		m_velocity_angular			= current_velocity.real_angular_velocity;
	}

	if (fis_zero(m_velocity_linear.target)) stop_linear();
}

// Get Point From Node in Radius
bool CMonsterMovement::GetNodeInRadius(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node)
{
	Fvector vertex_position = ai().level_graph().vertex_position(src_node);

	for (u32 i=0; i<attempts; i++) {

		Fvector dir;
		dir.random_dir	();
		dir.normalize	();

		Fvector new_pos;
		new_pos.mad(vertex_position, dir, Random.randF(min_radius, max_radius));

		dest_node = ai().level_graph().check_position_in_direction(src_node, vertex_position, new_pos);
		if (dest_node != u32(-1) && accessible(dest_node)) return (true);
	}
	return false;
}


