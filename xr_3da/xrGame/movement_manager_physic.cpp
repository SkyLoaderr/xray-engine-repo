////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_physic.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager : physic character movement
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "PHMovementControl.h"

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	Fvector				motion;
	dest_position		= Position();

#ifndef NO_PHYSICS_IN_AI_MOVE
	float				precision = 1.f;
#endif

	if (
			!enabled() || 
			path_completed() || 
			CDetailPathManager::path().empty() ||
			CDetailPathManager::completed(dest_position) || 
			(CDetailPathManager::curr_travel_point_index() >= CDetailPathManager::path().size() - 1) ||
			(CDetailPathManager::dest_position().similar(Position(),.1f))
		)
	{
		m_speed			= 0.f;
#ifndef NO_PHYSICS_IN_AI_MOVE
		if(movement_control->IsCharacterEnabled()) {
			motion.set	(0,0,0);
			movement_control->Calculate(CDetailPathManager::path(),0.f,CDetailPathManager::m_current_travel_point,precision);
			movement_control->GetPosition(dest_position);
		}

		if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) {
			Fvector		d;
			d.set		(0,1,0);
			Hit			(movement_control->gcontact_HealthLost,d,this,movement_control->ContactBone(),dest_position,0);
		}
#endif
		return;
	}

	if (time_delta < EPS)
		return;
#pragma todo("Dima to Kostia : Please change this piece of code to support paths with multiple desired velocities")
	float				speed		=	m_desirable_speed;//CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].m_linear_speed
	float				dist		=	speed*time_delta;
	float				dist_save	=	dist;

	// move full steps
	Fvector				mdir,target;
	target.set			(CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].m_position);
	mdir.sub			(target, dest_position);
	float				mdist =	mdir.magnitude();
	
	while (dist>mdist) {
		dest_position.set	(target);

		if (CDetailPathManager::curr_travel_point_index() + 1 >= CDetailPathManager::path().size())
			break;
		else {
			dist			-= mdist;
			++CDetailPathManager::m_current_travel_point;
			if ((CDetailPathManager::curr_travel_point_index()+1) >= CDetailPathManager::path().size())
				break;
			target.set	(CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].m_position);
			mdir.sub	(target, dest_position);
			mdist		= mdir.magnitude();
		}
	}

	if (mdist < EPS_L) {
		CDetailPathManager::m_current_travel_point = CDetailPathManager::path().size() - 1;
		m_speed			= 0.f;
		return;
	}

	// resolve stucking
	Device.Statistic.Physics.Begin	();

#ifndef NO_PHYSICS_IN_AI_MOVE
	setEnabled(false);
	Level().ObjectSpace.GetNearest		(dest_position,10.f); // JIM CHANGED IT FROM 1.0f
	xr_vector<CObject*> &tpNearestList	= Level().ObjectSpace.q_nearest; 
	setEnabled(true);
#endif

	motion.mul			(mdir,dist/mdist);
	dest_position.add	(motion);

#ifndef NO_PHYSICS_IN_AI_MOVE
	if ((tpNearestList.empty())) {
		if(!movement_control->TryPosition(dest_position)) {
//			movement_control->Calculate(CDetailPathManager::path(),CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].m_linear_speed,CDetailPathManager::curr_travel_point_index(),precision);
			movement_control->Calculate(CDetailPathManager::path(),speed,CDetailPathManager::m_current_travel_point,precision);
			movement_control->GetPosition(dest_position);
			if (!fsimilar(0.f,movement_control->gcontact_HealthLost))
				Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),dest_position,0);
		}
		else
			movement_control->b_exect_position=true;
	}
	else {
//		movement_control->Calculate(CDetailPathManager::path(),CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].m_linear_speed,CDetailPathManager::curr_travel_point_index(),precision);
		movement_control->Calculate(CDetailPathManager::path(),speed,CDetailPathManager::m_current_travel_point,precision);
		movement_control->GetPosition(dest_position);
		if (!fsimilar(0.f,movement_control->gcontact_HealthLost))
			Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),dest_position,0);
	}
#endif
	float				real_motion	= motion.magnitude() + dist_save-dist;
	float				real_speed	= real_motion/time_delta;
	m_speed				= 0.5f*speed + 0.5f*real_speed;
	Device.Statistic.Physics.End	();
}

