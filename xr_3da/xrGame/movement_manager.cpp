////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

CMovementManager::CMovementManager	()
{
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
//				if (!bSearchForNode || !tpDestinationPosition || !m_detail_path.size() || (m_detail_path[m_detail_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
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

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, float time_delta)
{
//	Fvector				motion;
//#ifndef NO_PHYSICS_IN_AI_MOVE
//float precesition=1.f;
//#endif
//	if ((TravelPath.empty()) || (TravelPath.size() - 1 <= TravelStart))	{
//		fSpeed = 0;
//#ifndef NO_PHYSICS_IN_AI_MOVE
//		if(Me->Movement.IsCharacterEnabled()) {
//			motion.set(0,0,0);
//
//			Me->Movement.Calculate(TravelPath,0.f,TravelStart,precesition);
//			Me->Movement.GetPosition(p_dest);
//		}
//
//		if (Me->Movement.gcontact_HealthLost) {
//			Fvector d;
//			d.set(0,1,0);
//			Me->Hit	(Me->Movement.gcontact_HealthLost,d,Me,Me->Movement.ContactBone(),p_dest,0);
//		}
//#endif
//		return;
//	}
//
//	if (dt<EPS)			
//		return;
//	float	dist		=	speed*dt;
//	float	dist_save	=	dist;
//
//	p_dest				=	p_src;
//
//	// move full steps
//	Fvector	mdir,target;
//	target.set		(TravelPath[TravelStart+1].P);
//	mdir.sub		(target, p_dest);
//	float	mdist	=	mdir.magnitude();
//	
//	while (dist>mdist) {
//		p_dest.set	(target);
//
//		if ((TravelStart+1) >= TravelPath.size())
//			break;
//		else {
//			dist			-= mdist;
//			TravelStart		++;
//			if ((TravelStart+1) >= TravelPath.size())
//				break;
//			target.set		(TravelPath[TravelStart+1].P);
//			mdir.sub		(target, p_dest);
//			mdist			= mdir.magnitude();
//		}
//	}
//
//	if (mdist < EPS_L) {
//		TravelStart = TravelPath.size() - 1;
//		fSpeed = 0;
//		return;
//	}
//
//	// resolve stucking
//	Device.Statistic.Physics.Begin	();
//
//#ifndef NO_PHYSICS_IN_AI_MOVE
//	Me->setEnabled(false);
//	Level().ObjectSpace.GetNearest		(p_dest,1.f); 
//	xr_vector<CObject*> &tpNearestList	= Level().ObjectSpace.q_nearest; 
//	Me->setEnabled(true);
//#endif
//
//	motion.mul			(mdir,dist/mdist);
//	p_dest.add			(motion);
//
//#ifndef NO_PHYSICS_IN_AI_MOVE
//	if ((tpNearestList.empty())) 
//	{
//		if(!Me->Movement.TryPosition(p_dest))//!Me->Movement.TryPosition(p_dest)
//		{
//			//motion.mul			(mdir,speed*10.f/mdir.magnitude());
//			//Me->Movement.Calculate(motion,0,0,0,0);
//			Me->Movement.Calculate(TravelPath,speed,TravelStart,precesition);
//			Me->Movement.GetPosition(p_dest);
//
//			if (Me->Movement.gcontact_HealthLost)	
//			{
//				Me->Hit	(Me->Movement.gcontact_HealthLost,mdir,Me,Me->Movement.ContactBone(),p_dest,0);
//			}
//		}
//		else
//		{
//			Me->Movement.b_exect_position=true;
//		}
//
//	}
//	else
//	{
//		//motion.mul			(mdir,speed*10.f/mdir.magnitude());
//		//Me->Movement.Calculate(motion,0,0,0,0);
//
//			Me->Movement.Calculate(TravelPath,speed,TravelStart,precesition);
//			Me->Movement.GetPosition(p_dest);
//		if (Me->Movement.gcontact_HealthLost)	
//		{
//			Me->Hit	(Me->Movement.gcontact_HealthLost,mdir,Me,Me->Movement.ContactBone(),p_dest,0);
//		}
//	}
//#endif
//	float	real_motion	= motion.magnitude() + dist_save-dist;
//	float	real_speed	= real_motion/dt;
//	fSpeed				= 0.5f*fSpeed + 0.5f*real_speed;
//	Device.Statistic.Physics.End	();
}
