////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_movement_manager.h"
#include "stalker_movement_manager_space.h"
#include "script_entity_action.h"
#include "ai/stalker/ai_stalker.h"
#include "sight_manager.h"

using namespace StalkerMovement;

IC	void CStalkerMovementManager::setup_head_speed		()
{
	if (mental_state() == eMentalStateFree) {
		if (m_stalker->CSightManager::enabled())
			m_head.speed		= PI_DIV_2;
	}
	else
		m_head.speed			= 3*PI_DIV_2;
}

IC	void CStalkerMovementManager::setup_body_orientation	()
{
	if (!path().empty() && (path().size() > curr_travel_point_index() + 1)) {
		Fvector					t;
		t.sub					(
			path()[curr_travel_point_index() + 1].position,
			path()[curr_travel_point_index()].position
		);
		float					y,p;
		t.getHP					(y,p);
		m_body.target.yaw		= -y;
		m_head.target.yaw		= -y;
		m_head.speed			= m_body.speed;
	}
}

//////////////////////////////////////////////////////////////////////////

CStalkerMovementManager::CStalkerMovementManager	()
{
	init							();
}

CStalkerMovementManager::~CStalkerMovementManager	()
{
}

void CStalkerMovementManager::init					()
{
}

void CStalkerMovementManager::Load					(LPCSTR section)
{
	inherited::Load					(section);
	m_stalker						= smart_cast<CAI_Stalker*>(this);
	VERIFY							(m_stalker);
}

void CStalkerMovementManager::reload				(LPCSTR section)
{
	inherited::reload				(section);
	
	m_fCrouchFactor					= pSettings->r_float(section,"CrouchFactor");
	m_fWalkFactor					= pSettings->r_float(section,"WalkFactor");
	m_fWalkBackFactor				= pSettings->r_float(section,"WalkBackFactor");
	m_fRunFactor					= pSettings->r_float(section,"RunFactor");
	m_fRunBackFactor				= pSettings->r_float(section,"RunBackFactor");
	m_fWalkFreeFactor				= pSettings->r_float(section,"WalkFreeFactor");
	m_fRunFreeFactor				= pSettings->r_float(section,"RunFreeFactor");
	m_fPanicFactor					= pSettings->r_float(section,"PanicFactor");
	m_fDamagedWalkFactor			= pSettings->r_float(section,"DamagedWalkFactor");
	m_fDamagedRunFactor				= pSettings->r_float(section,"DamagedRunFactor");
	m_fDamagedWalkFreeFactor		= pSettings->r_float(section,"DamagedWalkFreeFactor");
	m_fDamagedRunFreeFactor			= pSettings->r_float(section,"DamagedRunFreeFactor");
	m_fDamagedPanicFactor			= pSettings->r_float(section,"DamagedPanicFactor");

	init_velocity_masks				();
}

void CStalkerMovementManager::init_velocity_masks	()
{
	float			cf = 2.f;

	add_velocity	(eVelocityStandingFreeStand						,0.f							,PI_DIV_4	,PI_MUL_2	);
	add_velocity	(eVelocityStandingPanicStand					,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingDangerStand					,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingFreeStandDamaged				,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingPanicStandDamaged				,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingDangerStandDamaged			,0.f							,PI_MUL_2	);

	add_velocity	(eVelocityStandingFreeCrouch					,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingPanicCrouch					,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingDangerCrouch					,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingFreeCrouchDamaged				,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingPanicCrouchDamaged			,0.f							,PI_MUL_2	);
	add_velocity	(eVelocityStandingDangerCrouchDamaged			,0.f							,PI_MUL_2	);

	add_velocity	(eVelocityWalkFreePositive						,m_fWalkFreeFactor				,PI_DIV_8/1	,cf*PI_DIV_8/1	);
	add_velocity	(eVelocityRunFreePositive						,m_fRunFreeFactor				,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandPositive				,m_fWalkFactor					,100*PI		,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchPositive				,m_fWalkFactor*m_fCrouchFactor	,100*PI_DIV_2,cf*100*PI_DIV_2);
	add_velocity	(eVelocityRunDangerStandPositive				,m_fRunFactor					,100*PI		,2*cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchPositive				,m_fRunFactor*m_fCrouchFactor	,100*PI		,cf*100*PI		);
	add_velocity	(eVelocityRunPanicStandPositive					,m_fPanicFactor					,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
	add_velocity	(eVelocityWalkFreeDamagedPositive				,m_fDamagedWalkFreeFactor		,PI_DIV_8	,cf*PI_DIV_8	);
	add_velocity	(eVelocityRunFreeDamagedPositive				,m_fDamagedRunFreeFactor		,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandDamagedPositive		,m_fDamagedWalkFactor			,PI			,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchDamagedPositive		,m_fWalkFactor*m_fCrouchFactor	,3*PI_DIV_2	,cf*3*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandDamagedPositive			,m_fDamagedRunFactor			,PI_DIV_2	,2*cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchDamagedPositive		,m_fRunFactor*m_fCrouchFactor	,PI			,cf*PI			);
	add_velocity	(eVelocityRunPanicDamagedStandPositive			,m_fDamagedPanicFactor			,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
//	add_velocity	(eVelocityWalkFreeNegative						,-m_fWalkFreeFactor				,PI_DIV_8/1	,cf*PI_DIV_8/1	);
//	add_velocity	(eVelocityRunFreeNegative						,-m_fRunFreeFactor				,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandNegative				,-m_fWalkFactor					,PI			,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchNegative				,-m_fWalkFactor*m_fCrouchFactor	,3*PI_DIV_2	,cf*3*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandNegative				,-m_fRunFactor					,PI_DIV_2	,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchNegative				,-m_fRunFactor*m_fCrouchFactor	,PI			,cf*PI			);
//	add_velocity	(eVelocityRunPanicStandNegative					,-m_fPanicFactor				,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
//	add_velocity	(eVelocityWalkFreeDamagedNegative				,-m_fDamagedWalkFreeFactor		,PI_DIV_8	,cf*PI_DIV_8	);
//	add_velocity	(eVelocityRunFreeDamagedNegative				,-m_fDamagedRunFreeFactor		,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandDamagedNegative		,-m_fDamagedWalkFactor			,PI			,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchDamagedNegative		,-m_fWalkFactor*m_fCrouchFactor	,3*PI_DIV_2	,cf*3*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandDamagedNegative			,-m_fDamagedRunFactor			,PI_DIV_2	,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchDamagedNegative		,-m_fRunFactor*m_fCrouchFactor	,PI			,cf*PI			);
//	add_velocity	(eVelocityRunPanicDamagedStandNegative			,-m_fDamagedPanicFactor			,PI_DIV_8/2	,cf*PI_DIV_8/2	);
}

void CStalkerMovementManager::reinit				()
{
	inherited::reinit					();
	m_stalker->m_body.speed				= PI_MUL_2;
	m_head.speed						= 3*PI_DIV_2;

	m_current.m_use_desired_position	= false;
	m_current.m_use_desired_direction	= false;
	m_current.m_desired_position		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
	m_current.m_desired_direction		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
	m_current.m_path_evaluator			= 0;
	m_current.m_node_evaluator			= 0;
	m_current.m_body_state				= eBodyStateStand;
	m_current.m_movement_type			= eMovementTypeStand;
	m_current.m_mental_state			= eMentalStateDanger;
	m_current.m_path_type				= ePathTypeNoPath;
	m_current.m_detail_path_type		= eDetailPathTypeSmooth;

	m_target							= m_current;

	m_last_turn_index					= u32(-1);
}

bool CStalkerMovementManager::script_control		()
{
	if (!m_stalker->GetScriptControl())
		return						(false);

	if (!m_stalker->GetCurrentAction())
		return						(false);

	if (fis_zero(m_stalker->GetCurrentAction()->m_tMovementAction.m_fSpeed))
		return						(false);

	m_stalker->m_fCurSpeed			= m_stalker->GetCurrentAction()->m_tMovementAction.m_fSpeed;
	set_desirable_speed				(m_stalker->m_fCurSpeed);
	return							(true);
}

void CStalkerMovementManager::setup_movement_params	()
{
	m_current											= m_target;
	CMovementManager::set_path_type						(path_type());
	CDetailPathManager::set_path_type					(detail_path_type());
	CLevelLocationSelector::set_evaluator				(node_evaluator());
	CMovementManager::CLevelPathManager::set_evaluator	(path_evaluator() ? path_evaluator() : base_level_selector());

	if (use_desired_position()) {
		VERIFY											(valid(desired_position()));
		CDetailPathManager::set_dest_position			(desired_position());
	}
	else
		if ((path_type() != ePathTypePatrolPath) && (path_type() != ePathTypeGamePath))
			CDetailPathManager::set_dest_position		(ai().level_graph().vertex_position(CLevelPathManager::dest_vertex_id()));

	if (use_desired_direction()) {
		VERIFY											(valid(desired_direction()));
		CDetailPathManager::set_dest_direction			(desired_direction());
		CDetailPathManager::set_use_dest_orientation	(true);
	}
	else
		CDetailPathManager::set_use_dest_orientation	(false);
}

void CStalkerMovementManager::setup_velocities		()
{
	// setup desirable velocities mask
	// if we want to stand, do not setup velocity to prevent path rebuilding

	if (movement_type() == eMovementTypeStand)
		return;

	int						velocity_mask = eVelocityPositiveVelocity;

	// setup health state
	if (m_stalker->IsLimping())
		velocity_mask		|= eVelocityDamaged;

	// setup body state
	switch (body_state()) {
		case eBodyStateCrouch : {
			velocity_mask	|= eVelocityCrouch;
			break;
		}
		case eBodyStateStand : {
			velocity_mask	|= eVelocityStand;
			break;
		}
		case eBodyStateStandDamaged : {
			velocity_mask	|= eVelocityStand | eVelocityDamaged;
			break;
		}
		default : NODEFAULT;
	}

	// setup mental state
	switch (mental_state()) {
		case eMentalStateDanger : {
			velocity_mask	|= eVelocityDanger;
			break;
		}
		case eMentalStateFree : {
			velocity_mask	|= eVelocityFree;
			break;
		}
		case eMentalStatePanic : {
			velocity_mask	|= eVelocityPanic;
			break;
		}
	}

	// setup_movement_type
	switch (movement_type()) {
		case eMovementTypeWalk : {
			velocity_mask	|= eVelocityWalk;
			break;
		}
		case eMovementTypeRun : {
			velocity_mask	|= eVelocityRun;
			break;
		}
		default : {
			velocity_mask	|= eVelocityStanding;
			velocity_mask	&= u32(-1) ^ (eVelocityNegativeVelocity | eVelocityPositiveVelocity);
		}
	}

	// setup all the possible velocities
	if (velocity_mask & eVelocityDanger) {
		set_desirable_mask		(velocity_mask);
		set_velocity_mask	(
			velocity_mask | 
			eVelocityStanding
		);
	}
	else {
		set_try_min_time		(true);
		set_desirable_mask		(velocity_mask | eVelocityStanding);
		set_velocity_mask	(
			velocity_mask | 
			eVelocityWalk | 
			eVelocityStanding
		);
	}
}

void CStalkerMovementManager::parse_velocity_mask	()
{
	if (path().empty() || (curr_travel_point_index() != m_last_turn_index))
		m_last_turn_index				= u32(-1);

	m_stalker->CSightManager::enable	(true);

	if ((movement_type() == eMovementTypeStand) || path().empty() || (path().size() <= curr_travel_point_index())) {
		m_stalker->m_fCurSpeed			= 0;
		if (mental_state() != eMentalStateDanger)
			m_stalker->m_body.speed		= 1*PI_DIV_2;
		else
			m_stalker->m_body.speed		= PI_MUL_2;
		set_desirable_speed				(m_stalker->m_fCurSpeed);
		setup_head_speed				();
		if (angle_difference(m_body.current.yaw,m_head.current.yaw) > (left_angle(-m_head.current.yaw,-m_body.current.yaw) ? PI_DIV_6 : PI_DIV_3))
			m_body.target.yaw			= m_head.current.yaw;
		return;
	}

	DetailPathManager::STravelPathPoint	point = path()[curr_travel_point_index()];
	CDetailPathManager::STravelParams	current_velocity = velocity(point.velocity);

	if (fis_zero(current_velocity.linear_velocity)) {
		if (mental_state() != eMentalStateDanger) {
			setup_body_orientation			();
			m_stalker->CSightManager::enable(false);
		}
		if ((mental_state() == eMentalStateDanger) || fis_zero(path_direction_angle(),EPS_L) || (m_last_turn_index == curr_travel_point_index())) {
			m_last_turn_index			= curr_travel_point_index();
			m_stalker->CSightManager::enable(true);
			if (curr_travel_point_index() + 1 < path().size()) {
				point				= path()[curr_travel_point_index() + 1];
				current_velocity	= velocity(point.velocity);
			}
		}
	}
	
	m_stalker->m_fCurSpeed	= current_velocity.linear_velocity;
	m_stalker->m_body.speed	= current_velocity.real_angular_velocity;
	set_desirable_speed		(m_stalker->m_fCurSpeed);

	switch (point.velocity & eVelocityBodyState) {
		case eVelocityStand : {
			m_current.m_body_state	= eBodyStateStand;
			break;
		}
		case eVelocityCrouch : {
			m_current.m_body_state	= eBodyStateCrouch;
			break;
		}
		default : NODEFAULT;
	}

	switch (point.velocity & eVelocityMentalState) {
		case eVelocityFree : {
			m_current.m_mental_state	= eMentalStateFree;
			break;
		}
		case eVelocityDanger : {
			m_current.m_mental_state	= eMentalStateDanger;
			break;
		}
		case eVelocityPanic : {
			m_current.m_mental_state	= eMentalStatePanic;
			break;
		}
		default : NODEFAULT;
	}

	switch (point.velocity & eVelocityMovementType) {
		case eVelocityStanding : {
			m_current.m_movement_type	= eMovementTypeStand;
			break;
		}
		case eVelocityWalk : {
			m_current.m_movement_type	= eMovementTypeWalk;
			break;
		}
		case eVelocityRun : {
			m_current.m_movement_type	= eMovementTypeRun;
			break;
		}
		default : NODEFAULT;
	}

	setup_head_speed		();
}

void CStalkerMovementManager::update(u32 time_delta)
{
	setup_movement_params	();

	if (script_control())
		return;

	setup_velocities		();

	if (movement_type() != eMovementTypeStand)
		update_path			();

	parse_velocity_mask		();

//	set_desirable_speed		(0.f);
}
