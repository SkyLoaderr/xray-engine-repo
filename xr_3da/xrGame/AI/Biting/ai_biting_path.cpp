////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../graph_engine.h"
#include "../ai_monsters_misc.h"
#include "../../game_graph.h"
#include "../../game_level_cross_table.h"

#define MIN_RANGE_SEARCH_TIME_INTERVAL	 4000		// 2 сек
#define TIME_TO_SEARCH					60000
#define DODGE_AMPLITUDE					.5f
#define MAX_DODGE_DISTANCE				1.5f


/////////////////////////////////////////////////////////////////////////////////////
// Функция InitSelector
void CAI_Biting::vfInitSelector(PathManagers::CAbstractVertexEvaluator &S, bool hear_sound)
{
	S.m_dwCurTime		= m_current_update;
	S.m_tMe				= this;
	S.m_tpMyNode		= level_vertex();
	S.m_tMyPosition		= Position();

	if (!hear_sound && m_tEnemy.obj) {
		S.m_tEnemy			= m_tEnemy.obj;
		S.m_tEnemyPosition	= m_tEnemy.position;
		S.m_dwEnemyNode		= m_tEnemy.node_id;
		S.m_tpEnemyNode		= m_tEnemy.vertex;
	}
	
	INIT_SQUAD_AND_LEADER;

	S.m_taMembers		= &(Squad.Groups[g_Group()].Members);
	S.m_dwStartNode		= level_vertex_id();		// текущий узел
	S.m_tStartPosition	= Position();


}

// high level 
void CAI_Biting::Path_GetAwayFromPoint(CEntity *pE, Fvector position, float dist)
{
	if (pE) {
		m_tEnemy.Set(pE,0);									// forse enemy selection
		vfInitSelector(*m_tSelectorGetAway, false);
	} else {
		vfInitSelector(*m_tSelectorGetAway, true);
		m_tSelectorGetAway->m_tEnemyPosition = position;
	}
	
	float dist_to_point = position.distance_to(Position());

	m_tSelectorGetAway->m_fMinEnemyDistance = dist_to_point + 3.f;
	m_tSelectorGetAway->m_fMaxEnemyDistance = m_tSelectorGetAway->m_fMinEnemyDistance + m_tSelectorGetAway->m_fSearchRange + dist;
	m_tSelectorGetAway->m_fOptEnemyDistance = m_tSelectorGetAway->m_fMaxEnemyDistance;

	CLevelLocationSelector::set_evaluator(m_tSelectorGetAway);
}

void CAI_Biting::Path_CoverFromPoint(CEntity *pE, Fvector position)
{
//	if (pE) {
//		m_tEnemy.Set(pE,0); 									// forse enemy selection
//		vfInitSelector(*m_tSelectorCover, false);
//	} else {
//		vfInitSelector(*m_tSelectorCover, true);
//		m_tSelectorCover->m_tEnemyPosition = position;
//	}
//
//	float dist_to_point = position.distance_to(Position());
//
//	m_tSelectorCover->m_fMinEnemyDistance = dist_to_point + 3.f;
//	m_tSelectorCover->m_fMaxEnemyDistance = dist_to_point + m_tSelectorCover->m_fSearchRange;
//	m_tSelectorCover->m_fOptEnemyDistance = m_tSelectorCover->m_fMaxEnemyDistance;
//
//	vfChoosePointAndBuildPath(m_tSelectorCover, 0, true, 0, rebuild_time);
}

void CAI_Biting::Path_ApproachPoint(CEntity *pE, Fvector position)
{
	if (pE) {
		m_tEnemy.Set(pE,0); 									// forse enemy selection
		vfInitSelector(*m_tSelectorApproach, false);
	} else {
		vfInitSelector(*m_tSelectorApproach, true);
		m_tSelectorApproach->m_tEnemyPosition = position;
	}

	CLevelLocationSelector::set_evaluator(m_tSelectorApproach);
}

void CAI_Biting::Path_WalkAroundObj(CEntity *pE, Fvector position)
{
	if (pE) {
		m_tEnemy.Set(pE,0); 									// forse enemy selection
		vfInitSelector(*m_tSelectorWalkAround, false);
	} else {

		vfInitSelector(*m_tSelectorWalkAround, true);
		m_tSelectorWalkAround->m_tEnemyPosition = position;
	}

	CLevelLocationSelector::set_evaluator(m_tSelectorWalkAround);
}



// Развернуть объект в направление движения по пути
void CAI_Biting::SetDirectionLook(bool bReversed)
{
	float yaw,pitch;
	
	// get prev
	CDetailPathManager::direction().getHP(yaw,pitch);

	m_body.target.yaw = -yaw;
	m_body.target.pitch = 0;

	if (bReversed) m_body.target.yaw = angle_normalize(m_body.target.yaw + PI);
	else m_body.target.yaw = angle_normalize(m_body.target.yaw);

	m_head.target = m_body.target;
}

// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)
void CAI_Biting::LookPosition(Fvector to_point, float angular_speed)
{
	// по-умолчанию просто изменить m_body.target.yaw
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());	
	
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,p;
	dir.getHP(yaw,p);

	// установить текущий угол
	m_body.target.yaw = angle_normalize(-yaw);
}

// проверить, находится ли объект entity на ноде
// возвращает позицию объекта, если он находится на ноде, или центр его ноды
Fvector CAI_Biting::GetValidPosition(CEntity *entity, const Fvector &actual_position)
{
	if (ai().level_graph().inside(entity->level_vertex(), actual_position)) return actual_position;
	else return ai().level_graph().vertex_position(entity->level_vertex());
}

void CAI_Biting::MoveToTarget(CEntity *entity, u32 vel_mask, u32 des_mask) 
{
	SetPathParams(
		CMovementManager::ePathTypeLevelPath, 
		entity->level_vertex_id(),
		GetValidPosition(entity, entity->Position())
	);

}

void CAI_Biting::MoveToTarget(const Fvector &pos, u32 node_id, u32 vel_mask, u32 des_mask) 
{
	SetPathParams(
		CMovementManager::ePathTypeLevelPath, 
		node_id,
		pos
//		vel_mask,
//		des_mask
	);
}

void CAI_Biting::FaceTarget(CEntity *entity) 
{
	float yaw, pitch;
	Fvector dir;
	
	dir.sub(entity->Position(), Position());
	dir.getHP(yaw,pitch);
	yaw *= -1;
	yaw = angle_normalize(yaw);
	m_body.target.yaw = yaw;

}

void CAI_Biting::FaceTarget(const Fvector &position) 
{
	float yaw, pitch;
	Fvector dir;

	dir.sub(position, Position());
	dir.getHP(yaw,pitch);
	yaw *= -1;
	yaw = angle_normalize(yaw);
	m_body.target.yaw = yaw;
}


// возвращает true, если объект entity находится на ноде
bool CAI_Biting::IsObjectPositionValid(CEntity *entity)
{
	return ai().level_graph().inside(entity->level_vertex_id(), entity->Position());
}

bool CAI_Biting::IsMoveAlongPathFinished()
{
	return CDetailPathManager::completed(Position());
}

bool CAI_Biting::IsMovingOnPath()
{
	return (!IsMoveAlongPathFinished() && CMovementManager::enabled());
}

//-------------------------------------------------------------------------------------------

bool CAI_Biting::NeedRebuildPath(u32 n_points)
{
	if (CDetailPathManager::path().empty() || !IsMovingOnPath() || (curr_travel_point_index() + n_points >= CDetailPathManager::path().size())) {
		if (IsMovingOnPath() && (curr_travel_point_index() + n_points >= CDetailPathManager::path().size())) LOG_EX("u32(-1) Works!");		
		else LOG_EX("u32 MoveEnd Works!");
	
		return true;
	}
	return false;
}

bool CAI_Biting::NeedRebuildPath(float dist_to_end)
{
	if (CDetailPathManager::path().size() < 2) return true;

	float cur_dist_to_end = 0.f;
	for (u32 i=CDetailPathManager::curr_travel_point_index(); i<CDetailPathManager::path().size()-1; i++) {
		cur_dist_to_end += CDetailPathManager::path()[i].position.distance_to(CDetailPathManager::path()[i+1].position);
	}

	if (!IsMovingOnPath() || (cur_dist_to_end < dist_to_end)) {
		if (IsMovingOnPath() && (cur_dist_to_end < dist_to_end)) LOG_EX("float(-1) Works!");		
		else LOG_EX("float MoveEnd Works!");

		return true;
	}
	return false;
}
bool CAI_Biting::NeedRebuildPath(u32 n_points, float dist_to_end)
{
	return (NeedRebuildPath(n_points) || NeedRebuildPath(dist_to_end));
}

//-------------------------------------------------------------------------------------------


bool CAI_Biting::ObjectNotReachable(CEntity *entity) 
{
	if (!IsObjectPositionValid(entity)) return true;
	if (MotionMan.BadMotionFixed()) return true;

	return false;
}

void CAI_Biting::InitSelectorCommon(float dist_opt, float weight_opt, float dist_min, float weight_min, float dist_max, float weight_max)
{
	m_tSelectorCommon->m_fMinEnemyDistance			= dist_min;
	m_tSelectorCommon->m_fMaxEnemyDistance			= dist_max;
	m_tSelectorCommon->m_fOptEnemyDistance			= dist_opt;

	m_tSelectorCommon->m_fMinEnemyDistanceWeight	= weight_min;
	m_tSelectorCommon->m_fMaxEnemyDistanceWeight	= weight_max;
	m_tSelectorCommon->m_fOptEnemyDistanceWeight	= weight_opt;
}

void CAI_Biting::Path_CommonSelector(CEntity *pE, Fvector position)
{
	if (pE) {
		m_tEnemy.Set(pE,0); 									// forse enemy selection
		vfInitSelector(*m_tSelectorCommon, false);
	} else {
		vfInitSelector(*m_tSelectorCommon, true);
		m_tSelectorCommon->m_tEnemyPosition = position;
	}
	
	CLevelLocationSelector::set_evaluator(m_tSelectorCommon);
}

Fvector CAI_Biting::RandomPosInR(const Fvector &p, float R)
{
	Fvector v;
	v = p;
	v.x += ::Random.randF(-R,R);
	v.z += ::Random.randF(-R,R);

	return v;
}

void CAI_Biting::SetPathParams(CMovementManager::EPathType path_type, u32 dest_vertex_id, const Fvector &dest_pos)
{
	set_level_dest_vertex(dest_vertex_id);
	set_dest_position(dest_pos);
	set_path_type (path_type);
	
	SetupVelocityMasks();
}


void CAI_Biting::SetupVelocityMasks()
{
	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	switch (MotionMan.m_tAction) {
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
	case ACT_LOOK_AROUND:
	case ACT_ATTACK:
		bEnablePath = false;
		break;
	
	case ACT_WALK_FWD:
		vel_mask = eVelocityParamsWalk;
		des_mask = eVelocityParameterWalkNormal;
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		vel_mask = eVelocityParamsRun;
		des_mask = eVelocityParameterRunNormal;
		break;
	case ACT_DRAG:
		vel_mask = eVelocityParameterDrag;
		des_mask = eVelocityParameterDrag;

		MotionMan.SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = eVelocityParameterSteal;
		des_mask = eVelocityParameterSteal;
		break;
	case ACT_JUMP:
		break;
	}

	if (bEnablePath) {
		set_velocity_mask(vel_mask);	
		set_desirable_mask(des_mask);
	} else {
		enable_movement(false);
	}
}


// возвращает вершину level_graph, соответствующую случайной вершине game_graph в радиусе R
// если нет такой вершины - возвращает ближайшую
u32 CAI_Biting::GetNextGameVertex(float R)
{
	const CGameGraph::CVertex *pV;
	u32		nearest_game_vertex = u32(-1);
	float	best_dist = flt_max;

	// все вершины на этом уровне
	xr_vector<u32> level_nodes; 

	for (u32 i=0;i<ai().game_graph().header().vertex_count();i++) {
		pV = ai().game_graph().vertex(i);
		// находится ли вершина на данном уровне
		if (pV->level_id() == ai().level_graph().level_id()) {
			
			float cur_dist = pV->level_point().distance_to(Position());
			if ((best_dist > cur_dist) && (game_vertex_id() != i)) {
				best_dist = cur_dist;
				nearest_game_vertex = i;
			}
			
			if (cur_dist < R) level_nodes.push_back(i);
		}
	}	
	
	R_ASSERT(nearest_game_vertex != u32(-1));

	if (level_nodes.empty()) return ai().game_graph().vertex(nearest_game_vertex)->level_vertex_id();
	else return ai().game_graph().vertex(level_nodes[::Random.randI(level_nodes.size())])->level_vertex_id();
}

