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

void CMovementManager::build_path	()
{
//	INIT_SQUAD_AND_LEADER;
//
//	if (m_tPrevPathType != m_tPathType) {
//		m_tPrevPathType		= m_tPathType;
//		m_tPathState		= ePathStateSearchNode;
//		m_level_path.clear	();
//	}
//	if (tpNodeEvaluator)
//		vfInitSelector			(*tpNodeEvaluator,Squad,Leader);
//
//	EPathType tPathType = m_tPathType;
//	switch (m_tPathType) {
//		case ePathTypeStraight : {
//			m_tPathType = ePathTypeStraight;
//			break;
//								 }
//		case ePathTypeDodge : {
//			m_tPathType = ePathTypeDodge;
//			break;
//							  }
//		case ePathTypeCriteria : {
//			m_tPathType = ePathTypeCriteria;
//			break;
//								 }
//		case ePathTypeStraightDodge : {
//			if (::Random.randI(0,100) < (int)m_dwRandomFactor)
//				m_tPathType = ePathTypeStraight;
//			else
//				m_tPathType = ePathTypeDodge;
//			break;
//									  }
//		case ePathTypeDodgeCriteria : {
//			if (::Random.randI(0,100) < (int)m_dwRandomFactor)
//				m_tPathType = ePathTypeDodge;
//			else
//				m_tPathType = ePathTypeCriteria;
//			break;
//									  }
//	}
//	switch (m_tPathState) {
//		case ePathStateSearchNode : {
//			if (tpNodeEvaluator && bSearchForNode)
//				vfSearchForBetterPosition(*tpNodeEvaluator,Squad,Leader);
//			else
//				if (!bSearchForNode || !tpDestinationPosition || !m_detail_path.size() || (m_detail_path[m_detail_path.size() - 1].m_position.distance_to(*tpDestinationPosition) > EPS_L))
//					m_tPathState = ePathStateBuildNodePath;
//			break;
//									}
//		case ePathStateBuildNodePath : {
//			if ((m_level_dest_node != AI_NodeID) && (m_level_path.empty() || (m_level_path[m_level_path.size() - 1] != m_level_dest_node) || m_detail_path.empty() || ((m_detail_path.size() - 1) <= m_detail_cur_point_index)))
//				vfBuildPathToDestinationPoint(tpNodeEvaluator);
//			else
//				if ((m_level_dest_node == AI_NodeID) && tpDestinationPosition) {
//					m_level_path.clear();
//					m_level_path.push_back(AI_NodeID);
//					m_tPathState = ePathStateBuildTravelLine;
//				}
//				else
//					if (bSearchForNode && tpNodeEvaluator)
//						m_tPathState = ePathStateSearchNode;
//
//			break;
//									   }
//		case ePathStateBuildTravelLine : {
//			vfBuildTravelLine(tpDestinationPosition);
//			break;
//										 }
//		case ePathStateDodgeTravelLine : {
//			vfDodgeTravelLine();
//			break;
//										 }
//	}
//	m_tPathType = tPathType;
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
