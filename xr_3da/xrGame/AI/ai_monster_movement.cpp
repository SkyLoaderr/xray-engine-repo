#include "stdafx.h"

#include "ai_monster_movement.h"
#include "ai_monster_utils.h"
#include "../path_manager_level_selector.h"
#include "biting/ai_biting.h"
#include "ai_monster_motion_stats.h"

CMonsterMovement::CMonsterMovement()
{
	m_tSelectorApproach	= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
	MotionStats			= 0;		
}

CMonsterMovement::~CMonsterMovement()
{
	xr_delete(m_tSelectorApproach);
	
	if (MotionStats) xr_delete(MotionStats);
}

void CMonsterMovement::Init()
{
	pMonster			= dynamic_cast<CAI_Biting*>(this);
	VERIFY				(pMonster);
	
	MotionStats			= xr_new<CMotionStats> (pMonster);

	b_try_min_time		= false;
	time_last_approach	= 0;

}

//////////////////////////////////////////////////////////////////////////
// Init Movement
void CMonsterMovement::Frame_Init()
{
	CLevelLocationSelector::set_evaluator	(0);
	CDetailPathManager::set_path_type		(eDetailPathTypeSmooth);
	b_try_min_time							= true;		
	b_enable_movement						= true;
}


//////////////////////////////////////////////////////////////////////////
// Update Movement
void CMonsterMovement::Frame_Update()
{
	CDetailPathManager::set_try_min_time	(b_try_min_time); 
	enable_movement							(b_enable_movement);
	update_path								();
}


//////////////////////////////////////////////////////////////////////////
// Finalize Movement
void CMonsterMovement::Frame_Finalize()
{
	set_desirable_speed						(pMonster->m_fCurSpeed);
}


/////////////////////////////////////////////////////////////////////////////////////
// ������� InitSelector
void CMonsterMovement::InitSelector(PathManagers::CAbstractVertexEvaluator &S, Fvector target_pos)
{
	S.m_dwCurTime		= pMonster->m_dwCurrentTime;
	S.m_tMe				= pMonster;
	S.m_tpMyNode		= level_vertex();
	S.m_tMyPosition		= Position();
	S.m_dwStartNode		= level_vertex_id();
	S.m_tStartPosition	= Position();

	S.m_tEnemyPosition	= target_pos;
	S.m_tEnemy			= 0;
}

//////////////////////////////////////////////////////////////////////////
// Move To Target
void CMonsterMovement::MoveToTarget(const CEntity *entity) 
{
	SetPathParams(entity->level_vertex_id(), get_valid_position(entity, entity->Position()));
}

void CMonsterMovement::MoveToTarget(const Fvector &pos, u32 node_id) 
{
	SetPathParams(node_id,pos);
}


void CMonsterMovement::MoveToTarget(const Fvector &position)
{
	bool new_params = false;

	// ������������� ���� ����� �� ����� ����
	if (IsPathEnd(2,1.5f)) new_params = true;

	// ������������� ���� ����� ��������� �����
	if (m_tSelectorApproach->m_dwCurTime  + 2000 < pMonster->m_dwCurrentTime) new_params = true;

	if (!new_params) return;

	// ���� ���� � ������ ��������� - �� ������������ ��������
	u32 node_id = ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),position);
	if (ai().level_graph().valid_vertex_id(node_id)) {
		SetPathParams (node_id, position);

		// ������� � ������ ��������� ����� ���������� �������������� ����
		m_tSelectorApproach->m_dwCurTime = pMonster->m_dwCurrentTime;
	} else {
		bool use_selector = true;

		if (ai().level_graph().valid_vertex_position(position)) {
			u32 vertex_id = ai().level_graph().vertex_id(position);
			if (ai().level_graph().valid_vertex_id(vertex_id)) {
				SetPathParams (vertex_id, position);

				// ������� � ������ ��������� ����� ���������� �������������� ����
				m_tSelectorApproach->m_dwCurTime = pMonster->m_dwCurrentTime;

				use_selector = false;
			}
		}

		if (use_selector) {
			CLevelLocationSelector::set_evaluator(m_tSelectorApproach);
			InitSelector(*m_tSelectorApproach, position);

			CLevelLocationSelector::set_query_interval(3000);	
			SetSelectorPathParams ();
		}
	}
}

void CMonsterMovement::MoveAwayFromTarget(const Fvector &position, float dist)
{
	Fvector target_pos;
	Fvector dir;
	dir.sub(Position(), position);
#pragma todo("Dima to Jim : verify why direction becomes incorrect, pssibly XFORM() and/or position is not valid")
	if (!_valid(dir))
		dir.set(0.f,0.f,1.f);
	if (fis_zero(dir.square_magnitude(),EPS_L)) dir.set(0.f,0.f,1.f);
	dir.normalize();
	target_pos.mad(Position(),dir,dist);

	MoveToTarget(target_pos);
}


bool CMonsterMovement::IsMoveAlongPathFinished()
{
	return CDetailPathManager::completed(Position());
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
	if (pMonster->MotionMan.BadMotionFixed()) return true;

	return false;
}


void CMonsterMovement::SetPathParams(u32 dest_vertex_id, const Fvector &dest_pos)
{
	set_level_dest_vertex(dest_vertex_id);
	set_dest_position(dest_pos);
	set_path_type (CMovementManager::ePathTypeLevelPath);

	pMonster->SetupVelocityMasks(false);
}

void CMonsterMovement::SetSelectorPathParams()
{

	set_path_type (CMovementManager::ePathTypeLevelPath);
	
	// ������������ ��� ��������� ���������: true - ������������ ���� ��������� ����������, false - �������� ������� ������� ����, ���� ������ BuildLevelPath
	use_selector_path(true);

	pMonster->SetupVelocityMasks(false);
}


void CMonsterMovement::WalkNextGraphPoint()
{
	set_path_type		(CMovementManager::ePathTypeGamePath);
	set_selection_type	(CMovementManager::eSelectionTypeRandomBranching);

	pMonster->SetupVelocityMasks(false);
}

