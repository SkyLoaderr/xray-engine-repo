////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "PHMovementControl.h"

CMovementManager::CMovementManager	()
{
	init					();
}

CMovementManager::~CMovementManager	()
{
}

void CMovementManager::init			()
{
	CGamePathManager::init	();
	CLevelPathManager::init	();
	CDetailPathManager::init();
}

//#define NO_PHYSICS_IN_AI_MOVE

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, float time_delta)
{
	Fvector				motion;

#ifndef NO_PHYSICS_IN_AI_MOVE
	float				precision = 1.f;
#endif

	if ((m_detail_path.empty()) || (m_detail_path.size() - 1 <= m_detail_cur_point_index))	{
		m_speed			= 0;
#ifndef NO_PHYSICS_IN_AI_MOVE
		if(movement_control->IsCharacterEnabled()) {
			motion.set	(0,0,0);
			movement_control->Calculate(m_detail_path,0.f,m_detail_cur_point_index,precision);
			movement_control->GetPosition(Position());
		}

		if (movement_control->gcontact_HealthLost) {
			Fvector		d;
			d.set		(0,1,0);
			Hit			(movement_control->gcontact_HealthLost,d,this,movement_control->ContactBone(),Position(),0);
		}
#endif
		return;
	}

	if (time_delta < EPS)			
		return;
#pragma todo("Dima to Kostia : Please change this piece of code to support paths with multiple desired velocities")
	float				dist		=	m_detail_path[m_detail_cur_point_index].m_linear_speed*time_delta;
	float				dist_save	=	dist;

//	Position()			=	p_src;

	// move full steps
	Fvector				mdir,target;
	target.set			(m_detail_path[m_detail_cur_point_index + 1].m_position);
	mdir.sub			(target, Position());
	float				mdist =	mdir.magnitude();
	
	while (dist>mdist) {
		Position().set	(target);

		if (m_detail_cur_point_index + 1 >= m_detail_path.size())
			break;
		else {
			dist			-= mdist;
			m_detail_cur_point_index++;
			if ((m_detail_cur_point_index+1) >= m_detail_path.size())
				break;
			target.set	(m_detail_path[m_detail_cur_point_index+1].m_position);
			mdir.sub	(target, Position());
			mdist		= mdir.magnitude();
		}
	}

	if (mdist < EPS_L) {
		m_detail_cur_point_index = m_detail_path.size() - 1;
		m_speed			= 0;
		return;
	}

	// resolve stucking
	Device.Statistic.Physics.Begin	();

#ifndef NO_PHYSICS_IN_AI_MOVE
	setEnabled(false);
	Level().ObjectSpace.GetNearest		(Position(),1.f); 
	xr_vector<CObject*> &tpNearestList	= Level().ObjectSpace.q_nearest; 
	setEnabled(true);
#endif

	motion.mul			(mdir,dist/mdist);
	Position().add		(motion);

#ifndef NO_PHYSICS_IN_AI_MOVE
	if ((tpNearestList.empty())) {
		if(!movement_control->TryPosition(Position())) {
			movement_control->Calculate(m_detail_path,m_detail_path[m_detail_cur_point_index].m_linear_speed,m_detail_cur_point_index,precision);
			movement_control->GetPosition(Position());
			if (movement_control->gcontact_HealthLost)
				Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),Position(),0);
		}
		else
			movement_control->b_exect_position=true;
	}
	else {
		movement_control->Calculate(m_detail_path,m_detail_path[m_detail_cur_point_index].m_linear_speed,m_detail_cur_point_index,precision);
		movement_control->GetPosition(Position());
		if (movement_control->gcontact_HealthLost)
			Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),Position(),0);
	}
#endif
	float				real_motion	= motion.magnitude() + dist_save-dist;
	float				real_speed	= real_motion/time_delta;
	m_speed				= 0.5f*m_speed + 0.5f*real_speed;
	Device.Statistic.Physics.End	();
}

void CMovementManager::init_selector(PathManagers::CAbstractNodeEvaluator &S, CSquad &Squad, CEntity* &Leader)
{
	CSquad						&Squad = Level().Teams[g_Team()].Squads[g_Squad()];
	CEntity						*Leader = Squad.Leader;
	if (Leader->g_Health() <= 0)
		Leader					= this;
	R_ASSERT					(Leader);

	S.m_tHitDir			= m_tHitDir;
	S.m_dwHitTime		= m_dwHitTime;
	S.m_dwCurTime		= m_dwCurrentUpdate;
	S.m_tMe				= this;
	S.m_tpMyNode		= AI_Node;
	S.m_tMyPosition		= Position();
	
	if (m_tEnemy.Enemy) {
		vfSaveEnemy();
		S.m_tEnemy			= m_tEnemy.Enemy;
		S.m_tEnemyPosition	= m_tEnemy.Enemy->Position();
		S.m_dwEnemyNode		= m_tEnemy.Enemy->AI_NodeID;
		S.m_tpEnemyNode		= m_tEnemy.Enemy->AI_Node;
	}
	else {
		S.m_tEnemy			= m_tSavedEnemy;
		S.m_tEnemyPosition	= m_tSavedEnemyPosition;
		S.m_dwEnemyNode		= m_dwSavedEnemyNodeID;
		R_ASSERT2			(int(m_dwSavedEnemyNodeID) > 0, "Invalid enemy vertex");
		S.m_tpEnemyNode		= m_tpSavedEnemyNode;
	}
	
	S.m_taMembers		= &(Squad.Groups[g_Group()].Members);
	S.m_dwStartNode		= AI_NodeID;
	S.m_tStartPosition	= Position();
}

void CMovementManager::find_position(PathManagers::CAbstractNodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader)
{
	CSquad						&Squad = Level().Teams[g_Team()].Squads[g_Squad()];
	CEntity						*Leader = Squad.Leader;
	if (Leader->g_Health() <= 0)
		Leader					= this;
	R_ASSERT					(Leader);

//	Device.Statistic.AI_Range.Begin();	// определение времени вып. функции
//	
//	if ((!m_dwLastRangeSearch) || (m_detail_path.empty()) || (int(m_detail_cur_point_index) > int(m_detail_path.size()) - 4) || (speed() < EPS_L) || ((tNodeEvaluator.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL))) {
//		
//		m_dwLastRangeSearch = tNodeEvaluator.m_dwCurTime;
//		
//		float fOldCost = MAX_NODE_ESTIMATION_COST;
//		
//		if (m_level_dest_node != u32(-1)) {
//			tNodeEvaluator.m_tpCurrentNode	= ai().level_graph().vertex(m_level_dest_node);
//			tNodeEvaluator.m_fDistance		= Position().distance_to(ai().level_graph().vertex_position(m_level_dest_node));
//			fOldCost						= tNodeEvaluator.ffEvaluateNode();
////			Msg								("Old  : [%d][%f]",AI_NodeID,fOldCost);
//		}
//
//		Squad.Groups[g_Group()].GetAliveMemberInfo(tNodeEvaluator.m_taMemberPositions, tNodeEvaluator.m_taMemberNodes, tNodeEvaluator.m_taDestMemberPositions, tNodeEvaluator.m_taDestMemberNodes, this);
//		
//		Device.Statistic.AI_Range.End();
//		
//		tNodeEvaluator.vfShallowGraphSearch(ai().q_mark_bit);
//		
//		Device.Statistic.AI_Range.Begin();
//
////		Msg									("Best : [%d][%f]",tNodeEvaluator.m_dwBestNode,tNodeEvaluator.m_fBestCost);
////		Msg									("Params : %f - [%f][%f][%f][%f][%f][%f]",m_tEnemy.Enemy->Position().distance_to(Position()),tNodeEvaluator.m_fMaxEnemyDistance,tNodeEvaluator.m_fOptEnemyDistance,tNodeEvaluator.m_fMinEnemyDistance,tNodeEvaluator.m_fMaxEnemyDistanceWeight,tNodeEvaluator.m_fOptEnemyDistanceWeight,tNodeEvaluator.m_fMinEnemyDistanceWeight);
////		Msg									("Evaluator : [%f][%f][%f]",tNodeEvaluator.m_fMaxEnemyDistance,tNodeEvaluator.m_fOptEnemyDistance,tNodeEvaluator.m_fMinEnemyDistance);
//		if ((m_level_dest_node != tNodeEvaluator.m_dwBestNode) && (tNodeEvaluator.m_fBestCost < fOldCost - 0.f)){
//			m_level_dest_node		= tNodeEvaluator.m_dwBestNode;
//			if (!m_level_dest_node) {
//				Msg("! Invalid vertex Evaluator vertex");
//			}
//			m_level_path.clear		();
//			m_tPathState			= ePathStateBuildNodePath;
//			m_bIfSearchFailed		= false;
//		} 
//		else
//			m_bIfSearchFailed		= true;
//	}
//
//	Device.Statistic.AI_Range.End();
}

void CMovementManager::find_path(PathManagers::CAbstractNodeEvaluator *tpNodeEvaluator)
{
//	Device.Statistic.AI_Path.Begin();
//	
//	if (m_level_dest_node == AI_NodeID) {
//		m_level_path.clear		();
//		m_detail_path.clear();
//		m_detail_cur_point_index		= 0;
//		m_fCurSpeed				= 0;
//		m_tPathState			= ePathStateSearchNode;
//		Device.Statistic.AI_Path.End();
//		return;
//	}
//	
//	if (tpNodeEvaluator)
//		ai().m_tpAStar->ffFindOptimalPath(AI_NodeID,m_level_dest_node,AI_Path,tpNodeEvaluator->m_dwEnemyNode,tpNodeEvaluator->m_fOptEnemyDistance);
//	else
//		ai().m_tpAStar->ffFindMinimalPath(AI_NodeID,m_level_dest_node,AI_Path);
//	
//	if (m_level_path.empty()) {
//#ifdef DEBUG
//		Msg("! !!!! node_start %d, node_finish %d",AI_NodeID,m_level_dest_node);
//#endif
////		if (tpNodeEvaluator) {
//			ai().m_tpAStar->ffFindMinimalPath(AI_NodeID,m_level_dest_node,AI_Path);
//			if (m_level_path.empty())
//				m_tPathState = ePathStateSearchNode;
//			else
//				m_tPathState = ePathStateBuildTravelLine;
////		}
////		else
////			m_tPathState = ePathStateSearchNode;
//	}
//	else
//		m_tPathState = ePathStateBuildTravelLine;
//	
//	Device.Statistic.AI_Path.End();
}

void CMovementManager::build_path(PathManagers::CAbstractNodeEvaluator *node_evluator, Fvector *tpDestinationPosition, bool bSearchForNode)
{
	if (node_evluator)
		init_evaluator			(*node_evluator);

	if (m_path_state_previous != m_path_state) {
		m_path_state_previous	= m_path_state;
		m_path_state			= ePathStateSearchNode;
		m_level_path.clear		();
	}

	EPathType					tPathType = m_tPathType;
	switch (m_tPathType) {
		case ePathTypeStraight : {
			m_tPathType			= ePathTypeStraight;
			break;
		}
		case ePathTypeDodge : {
			m_tPathType			= ePathTypeDodge;
			break;
		}
		case ePathTypeCriteria : {
			m_tPathType			= ePathTypeCriteria;
			break;
		}
		case ePathTypeStraightDodge : {
			if (::Random.randI(0,100) < (int)m_dwRandomFactor)
				m_tPathType		= ePathTypeStraight;
			else
				m_tPathType		= ePathTypeDodge;
			break;
		}
		case ePathTypeDodgeCriteria : {
			if (::Random.randI(0,100) < (int)m_dwRandomFactor)
				m_tPathType		= ePathTypeDodge;
			else
				m_tPathType		= ePathTypeCriteria;
			break;
		}
	}
	switch (m_tPathState) {
		case ePathStateSearchNode : {
			if (tpNodeEvaluator && bSearchForNode)
				find_position	(*tpNodeEvaluator,Squad,Leader);
			else
				if (!bSearchForNode || !tpDestinationPosition || !m_detail_path.size() || (m_detail_path[m_detail_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
					m_tPathState = ePathStateBuildNodePath;
			break;
		}
		case ePathStateBuildNodePath : {
			if ((m_level_dest_node != AI_NodeID) && (m_level_path.empty() || (m_level_path[m_level_path.size() - 1] != m_level_dest_node) || m_detail_path.empty() || ((m_detail_path.size() - 1) <= m_detail_cur_point_index)))
				vfBuildPathToDestinationPoint(tpNodeEvaluator);
			else
				if ((m_level_dest_node == AI_NodeID) && tpDestinationPosition) {
					m_level_path.clear();
					m_level_path.push_back(AI_NodeID);
					m_tPathState = ePathStateBuildTravelLine;
				}
				else
					if (bSearchForNode && tpNodeEvaluator)
						m_tPathState = ePathStateSearchNode;

			break;
		}
		case ePathStateBuildTravelLine : {
			vfBuildTravelLine(tpDestinationPosition);
			break;
		}
		case ePathStateDodgeTravelLine : {
			vfDodgeTravelLine();
			break;
		}
	}
	m_tPathType = tPathType;
}

