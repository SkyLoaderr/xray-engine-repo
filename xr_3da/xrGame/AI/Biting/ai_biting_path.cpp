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


/////////////////////////////////////////////////////////////////////////////////////
// ‘ункци€ InitSelector
void CAI_Biting::InitSelector(PathManagers::CAbstractVertexEvaluator &S, Fvector target_pos)
{
	S.m_dwCurTime		= m_current_update;
	S.m_tMe				= this;
	S.m_tpMyNode		= level_vertex();
	S.m_tMyPosition		= Position();
	S.m_dwStartNode		= level_vertex_id();		
	S.m_tStartPosition	= Position();

	S.m_tEnemyPosition	= target_pos;
	S.m_tEnemy			= 0;
}

// high level 
void CAI_Biting::Path_GetAwayFromPoint(Fvector position, float dist)
{
//	Fvector target_pos;
//	Fvector dir;
//	dir.sub(Position(), position);
//	if (fsimilar(dir.square_magnitude(),0.f)) dir.set(0.f,0.f,1.f);
//	dir.normalize();	
//	target_pos.mad(position,dir,dist);
//	

	CLevelLocationSelector::set_evaluator(m_tSelectorGetAway);
	InitSelector(*m_tSelectorGetAway, position);

	CLevelLocationSelector::set_query_interval(3000);	
}

void CAI_Biting::Path_ApproachPoint(Fvector position)
{
	bool new_params = false;

	// перестраивать если дошЄл до конца пути
	if (NeedRebuildPath(2,0.5f)) new_params = true;
	// перестраивать если вышел временной квант
	if (m_tSelectorApproach->m_dwCurTime  + 2000 < m_dwCurrentTime) new_params = true;

	if (!new_params) return;

	// если нода в пр€мой видимости - не использовать селектор
	u32 node_id = ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),position);
	if (node_id != u32(-1)) {
		set_level_dest_vertex	(node_id);
		set_dest_position		(position);

		// хранить в данном селекторе врем€ последнего перестраивани€ пути
		m_tSelectorApproach->m_dwCurTime = m_dwCurrentTime;
	} else {
		CLevelLocationSelector::set_evaluator(m_tSelectorApproach);
		InitSelector(*m_tSelectorApproach, position);
		
		CLevelLocationSelector::set_query_interval(3000);	

	}
}


// –азвернуть объект в направление движени€ по пути
void CAI_Biting::SetDirectionLook(bool bReversed)
{
	float yaw,pitch;
	
	// get prev
	CDetailPathManager::direction().getHP(yaw,pitch);

	if (fsimilar(yaw,0.f)) return;

	m_body.target.yaw = -yaw;
	m_body.target.pitch = -pitch;

	if (bReversed) m_body.target.yaw = angle_normalize(m_body.target.yaw + PI);
	else m_body.target.yaw = angle_normalize(m_body.target.yaw);
}

// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)
void CAI_Biting::LookPosition(Fvector to_point, float angular_speed)
{
	// по-умолчанию просто изменить m_body.target.yaw
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());	
	
	// получаем вектор направлени€ к источнику звука и его мировые углы
	float		yaw,p;
	dir.getHP(yaw,p);

	// установить текущий угол
	m_body.target.yaw = angle_normalize(-yaw);
}

// проверить, находитс€ ли объект entity на ноде
// возвращает позицию объекта, если он находитс€ на ноде, или центр его ноды
Fvector CAI_Biting::GetValidPosition(const CEntity *entity, const Fvector &actual_position)
{
	if (ai().level_graph().inside(entity->level_vertex(), actual_position)) return actual_position;
	else return ai().level_graph().vertex_position(entity->level_vertex());
}

void CAI_Biting::MoveToTarget(const CEntity *entity) 
{
	SetPathParams(entity->level_vertex_id(), GetValidPosition(entity, entity->Position()));

}

void CAI_Biting::MoveToTarget(const Fvector &pos, u32 node_id) 
{
	SetPathParams(node_id,pos);
}

void CAI_Biting::FaceTarget(const CEntity *entity) 
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


// возвращает true, если объект entity находитс€ на ноде
bool CAI_Biting::IsObjectPositionValid(const CEntity *entity)
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
	if (CDetailPathManager::path().empty() || !IsMovingOnPath() || (curr_travel_point_index() + n_points >= CDetailPathManager::path().size())) return true;
	return false;
}

bool CAI_Biting::NeedRebuildPath(float dist_to_end)
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
bool CAI_Biting::NeedRebuildPath(u32 n_points, float dist_to_end)
{
	return (NeedRebuildPath(n_points) || NeedRebuildPath(dist_to_end));
}

//-------------------------------------------------------------------------------------------


bool CAI_Biting::ObjectNotReachable(const CEntity *entity) 
{
	if (!IsObjectPositionValid(entity)) return true;
	if (MotionMan.BadMotionFixed()) return true;

	return false;
}

Fvector CAI_Biting::RandomPosInR(const Fvector &p, float R)
{
	Fvector v;
	v = p;
	v.x += ::Random.randF(-R,R);
	v.z += ::Random.randF(-R,R);

	return v;
}

void CAI_Biting::SetPathParams(u32 dest_vertex_id, const Fvector &dest_pos)
{
	set_level_dest_vertex(dest_vertex_id);
	set_dest_position(dest_pos);
	set_path_type (CMovementManager::ePathTypeLevelPath);
	
	SetupVelocityMasks(false);
}

void CAI_Biting::SetSelectorPathParams()
{
	set_path_type (CMovementManager::ePathTypeLevelPath);
	SetupVelocityMasks(false);
}

void CAI_Biting::SetupVelocityMasks(bool force_real_speed)
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
	
	if (force_real_speed) vel_mask = des_mask;

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
		// находитс€ ли вершина на данном уровне
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


void CAI_Biting::WalkNextGraphPoint()
{
	set_path_type		(CMovementManager::ePathTypeGamePath);
	set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);

	SetupVelocityMasks(false);
}

