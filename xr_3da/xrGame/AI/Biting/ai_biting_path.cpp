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

void CAI_Biting::MoveToTarget(CEntity *entity) 
{
	set_level_dest_vertex(entity->level_vertex_id());
	set_dest_position(GetValidPosition(entity, entity->Position()));
	set_path_type (CMovementManager::ePathTypeLevelPath);
}

void CAI_Biting::MoveToTarget(const Fvector &pos, u32 node_id) 
{
	set_level_dest_vertex(node_id);
	set_dest_position(pos);
	set_path_type (CMovementManager::ePathTypeLevelPath);
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

bool CAI_Biting::IsMovingOnPath	()
{
	return !IsMoveAlongPathFinished() && CMovementManager::enabled();
}


bool CAI_Biting::ObjectNotReachable(CEntity *entity) 
{
	if (!IsObjectPositionValid(entity)) return true;
	if (MotionMan.BadMotionFixed()) return true;

	return false;
}
