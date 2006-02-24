////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_movement_manager.h"
#include "stalker_movement_manager_space.h"
#include "script_entity_action.h"
#include "ai/stalker/ai_stalker.h"
#include "sight_manager.h"
#include "detail_path_manager.h"
#include "level_location_selector.h"
#include "level_path_manager.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "entitycondition.h"
#include "ai_object_location.h"
#include "stalker_velocity_holder.h"
#include "stalker_velocity_collection.h"

using namespace StalkerMovement;

IC	void CStalkerMovementManager::setup_head_speed		()
{
	if (mental_state() == eMentalStateFree) {
		if (object().sight().enabled())
			m_head.speed		= PI_DIV_2;
	}
	else
		m_head.speed			= 3*PI_DIV_2;
}

IC	void CStalkerMovementManager::add_velocity		(int mask, float linear, float compute_angular, float angular)
{
	detail().add_velocity(mask,CDetailPathManager::STravelParams(linear,compute_angular,angular));
}

IC	float CStalkerMovementManager::path_direction_angle	()
{
	if (!path().empty() && (path().size() > detail().curr_travel_point_index() + 1)) {
		Fvector					t;
		t.sub					(
			path()[detail().curr_travel_point_index() + 1].position,
			path()[detail().curr_travel_point_index()].position
		);
		float					y,p;
		t.getHP					(y,p);
		return					(angle_difference(-y,m_body.current.yaw));
	}
	return						(0.f);
}

void CStalkerMovementManager::initialize()
{
	set_path_type			(MovementManager::ePathTypeLevelPath);
	set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	set_body_state			(eBodyStateStand);
	set_movement_type		(eMovementTypeStand);
	set_mental_state		(eMentalStateDanger);
	set_desired_direction	(0);

#ifdef DEBUG
	restrictions().initialize();
#endif

	restrictions().remove_all_restrictions();
	set_nearest_accessible_position();
}

void CStalkerMovementManager::set_desired_position(const Fvector *desired_position)
{
	if (desired_position) {
		m_target.m_use_desired_position	= true;
		VERIFY2							(object().movement().accessible(*desired_position),*object().cName());
		m_target.m_desired_position		= *desired_position;
	}
	else {
		m_target.m_use_desired_position	= false;
#ifdef DEBUG
		m_target.m_desired_position		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
#endif
	}
}

IC	void CStalkerMovementManager::setup_body_orientation	()
{
	if (path().empty())
		return;
	
	if (path().size() <= detail().curr_travel_point_index() + 1)
		return;

	Fvector					temp;
	temp.sub					(
		path()[detail().curr_travel_point_index() + 1].position,
		path()[detail().curr_travel_point_index()].position
	);
	float					y,p;
	temp.getHP				(y,p);
	m_body.target.yaw		= -y;
	m_head.target.yaw		= -y;
	m_head.speed			= m_body.speed;
}

//////////////////////////////////////////////////////////////////////////

CStalkerMovementManager::CStalkerMovementManager	(CAI_Stalker *object) :
	inherited					(object)
{
	VERIFY						(object);
	m_object					= object;
	m_velocities				= 0;
}

CStalkerMovementManager::~CStalkerMovementManager	()
{
}

void CStalkerMovementManager::Load					(LPCSTR section)
{
	inherited::Load				(section);
}

void CStalkerMovementManager::reload				(LPCSTR section)
{
	inherited::reload			(section);

#ifndef OLD_VELOCITIES
	m_velocities				= &stalker_velocity_holder().collection(pSettings->r_string(section,"movement_speeds"));
#else
	m_crouch_factor				= pSettings->r_float(section,"CrouchFactor");
	m_walk_factor				= pSettings->r_float(section,"WalkFactor");
	m_walk_back_factor			= pSettings->r_float(section,"WalkBackFactor");
	m_run_factor				= pSettings->r_float(section,"RunFactor");
	m_run_back_factor			= pSettings->r_float(section,"RunBackFactor");
	m_walk_free_factor			= pSettings->r_float(section,"WalkFreeFactor");
	m_run_free_factor			= pSettings->r_float(section,"RunFreeFactor");
	m_panic_factor				= pSettings->r_float(section,"PanicFactor");
	m_damaged_walk_factor		= pSettings->r_float(section,"DamagedWalkFactor");
	m_damaged_run_factor		= pSettings->r_float(section,"DamagedRunFactor");
	m_damaged_walk_free_factor	= pSettings->r_float(section,"DamagedWalkFreeFactor");
	m_damaged_run_free_factor	= pSettings->r_float(section,"DamagedRunFreeFactor");
	m_damaged_panic_factor		= pSettings->r_float(section,"DamagedPanicFactor");
#endif

	init_velocity_masks			();
}

void CStalkerMovementManager::init_velocity_masks	()
{
	float			cf = 2.f;

#ifndef OLD_VELOCITIES
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

	add_velocity	(eVelocityWalkFreePositive						,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,PI_DIV_8/1	,cf*PI_DIV_8/1	);
	add_velocity	(eVelocityRunFreePositive						,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandPositive				,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchPositive				,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeWalk,eMovementDirectionForward),100*PI		,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandPositive				,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityRunDangerCrouchPositive				,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityRunPanicStandPositive					,m_velocities->velocity(eMentalStatePanic,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																	 																						
	add_velocity	(eVelocityWalkFreeDamagedPositive				,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,PI_DIV_8/1	,cf*PI_DIV_8/1	);
	add_velocity	(eVelocityRunFreeDamagedPositive				,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandDamagedPositive		,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchDamagedPositive		,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeWalk,eMovementDirectionForward),100*PI		,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandDamagedPositive			,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityRunDangerCrouchDamagedPositive		,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityRunPanicDamagedStandPositive			,m_velocities->velocity(eMentalStatePanic,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
//	add_velocity	(eVelocityWalkFreeNegative						,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,PI_DIV_8/1	,cf*PI_DIV_8/1	);
//	add_velocity	(eVelocityRunFreeNegative						,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandNegative				,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchNegative				,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeWalk,eMovementDirectionForward),100*PI		,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandNegative				,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityRunDangerCrouchNegative				,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
//	add_velocity	(eVelocityRunPanicStandNegative					,m_velocities->velocity(eMentalStatePanic,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
//	add_velocity	(eVelocityWalkFreeDamagedNegative				,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,PI_DIV_8/1	,cf*PI_DIV_8/1	);
//	add_velocity	(eVelocityRunFreeDamagedNegative				,m_velocities->velocity(eMentalStateFree,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandDamagedNegative		,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeWalk,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchDamagedNegative		,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeWalk,eMovementDirectionForward),100*PI		,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandDamagedNegative			,m_velocities->velocity(eMentalStateDanger,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
	add_velocity	(eVelocityRunDangerCrouchDamagedNegative		,m_velocities->velocity(eMentalStateDanger,eBodyStateCrouch,eMovementTypeRun,eMovementDirectionForward)	,100*PI		,cf*PI			);
//	add_velocity	(eVelocityRunPanicDamagedStandNegative			,m_velocities->velocity(eMentalStatePanic,eBodyStateStand,eMovementTypeRun,eMovementDirectionForward)	,PI_DIV_8/2	,cf*PI_DIV_8/2	);
#else
	add_velocity	(eVelocityStandingFreeStand						,0.f							,PI_DIV_4	,PI_MUL_2		);
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

	add_velocity	(eVelocityWalkFreePositive						,walk_free_factor()				,PI_DIV_8/1	,cf*PI_DIV_8/1	);
	add_velocity	(eVelocityRunFreePositive						,run_free_factor()				,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandPositive				,walk_factor()					,100*PI		,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchPositive				,walk_factor()*crouch_factor()	,100*PI_DIV_2,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandPositive				,run_factor()					,100*PI		,2*cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchPositive				,run_factor()*crouch_factor()	,100*PI		,cf*PI			);
	add_velocity	(eVelocityRunPanicStandPositive					,panic_factor()					,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
	add_velocity	(eVelocityWalkFreeDamagedPositive				,damaged_walk_free_factor()		,PI_DIV_8	,cf*PI_DIV_8	);
	add_velocity	(eVelocityRunFreeDamagedPositive				,damaged_run_free_factor()		,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandDamagedPositive		,damaged_walk_factor()			,PI			,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchDamagedPositive		,walk_factor()*crouch_factor()	,3*PI_DIV_2	,cf*3*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandDamagedPositive			,damaged_run_factor()			,PI_DIV_2	,2*cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchDamagedPositive		,run_factor()*crouch_factor()	,PI			,cf*PI			);
	add_velocity	(eVelocityRunPanicDamagedStandPositive			,damaged_panic_factor()			,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
//	add_velocity	(eVelocityWalkFreeNegative						,-walk_free_factor()				,PI_DIV_8/1	,cf*PI_DIV_8/1	);
//	add_velocity	(eVelocityRunFreeNegative						,-run_free_factor()				,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandNegative				,-walk_factor()					,PI			,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchNegative				,-walk_factor()*crouch_factor()	,3*PI_DIV_2	,cf*3*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandNegative				,-run_factor()					,PI_DIV_2	,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchNegative				,-run_factor()*crouch_factor()	,PI			,cf*PI			);
//	add_velocity	(eVelocityRunPanicStandNegative					,-panic_factor()				,PI_DIV_8/2	,cf*PI_DIV_8/2	);
																																							
//	add_velocity	(eVelocityWalkFreeDamagedNegative				,-damaged_walk_free_factor()		,PI_DIV_8	,cf*PI_DIV_8	);
//	add_velocity	(eVelocityRunFreeDamagedNegative				,-damaged_run_free_factor()		,PI_DIV_8/2	,cf*PI_DIV_8/2	);
	add_velocity	(eVelocityWalkDangerStandDamagedNegative		,-damaged_walk_factor()			,PI			,cf*PI			);
	add_velocity	(eVelocityWalkDangerCrouchDamagedNegative		,-walk_factor()*crouch_factor()	,3*PI_DIV_2	,cf*3*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerStandDamagedNegative			,-damaged_run_factor()			,PI_DIV_2	,cf*PI_DIV_2	);
	add_velocity	(eVelocityRunDangerCrouchDamagedNegative		,-run_factor()*crouch_factor()	,PI			,cf*PI			);
//	add_velocity	(eVelocityRunPanicDamagedStandNegative			,-damaged_panic_factor()			,PI_DIV_8/2	,cf*PI_DIV_8/2	);
#endif
}

void CStalkerMovementManager::reinit				()
{
	inherited::reinit					();
	m_body.speed						= PI_MUL_2;
	m_head.speed						= 3*PI_DIV_2;

	m_current.m_use_desired_position	= false;
	m_current.m_use_desired_direction	= false;
	m_current.m_desired_position		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
	m_current.m_desired_direction		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
	m_current.m_body_state				= eBodyStateStand;
	m_current.m_movement_type			= eMovementTypeStand;
	m_current.m_mental_state			= eMentalStateDanger;
	m_current.m_path_type				= MovementManager::ePathTypeNoPath;
	m_current.m_detail_path_type		= DetailPathManager::eDetailPathTypeSmooth;

	m_target							= m_current;

	m_last_turn_index					= u32(-1);
}

bool CStalkerMovementManager::script_control		()
{
	if (!object().GetScriptControl())
		return						(false);

	if (!object().GetCurrentAction())
		return						(false);

	if (fis_zero(object().GetCurrentAction()->m_tMovementAction.m_fSpeed))
		return						(false);

	object().m_fCurSpeed			= object().GetCurrentAction()->m_tMovementAction.m_fSpeed;
	set_desirable_speed				(object().m_fCurSpeed);
	return							(true);
}

void CStalkerMovementManager::setup_movement_params	()
{
	inherited::set_path_type				(path_type());
	detail().set_path_type					(detail_path_type());
	level_path().set_evaluator				(base_level_params());

	if (use_desired_position()) {
		VERIFY								(_valid(desired_position()));
		if (!restrictions().actual() && !restrictions().accessible(desired_position())) {
			Fvector							temp;
			level_path().set_dest_vertex	(restrictions().accessible_nearest(desired_position(),temp));
			detail().set_dest_position		(temp);
		}
		else
			detail().set_dest_position		(desired_position());
	}
	else
		if ((path_type() != MovementManager::ePathTypePatrolPath) && (path_type() != MovementManager::ePathTypeGamePath)  && (path_type() != MovementManager::ePathTypeNoPath)) {
			if (!restrictions().actual() && !restrictions().accessible(level_path().dest_vertex_id())) {
				Fvector							temp;
				level_path().set_dest_vertex	(restrictions().accessible_nearest(ai().level_graph().vertex_position(level_path().dest_vertex_id()),temp));
				detail().set_dest_position		(temp);
			}
			else
				detail().set_dest_position		(ai().level_graph().vertex_position(level_path().dest_vertex_id()));
		}

	if (use_desired_direction()) {
		VERIFY											(_valid(desired_direction()));
		detail().set_dest_direction		(desired_direction());
		detail().set_use_dest_orientation	(true);
	}
	else
		detail().set_use_dest_orientation	(false);
}

void CStalkerMovementManager::setup_velocities		()
{
	// setup desirable velocities mask
	// if we want to stand, do not setup velocity to prevent path rebuilding

	int						velocity_mask = eVelocityPositiveVelocity;

	// setup health state
	if (object().conditions().IsLimping())
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
		detail().set_desirable_mask		(velocity_mask);
		detail().set_velocity_mask	(
			velocity_mask | 
			eVelocityStanding
		);
	}
	else {
		detail().set_try_min_time		(true);
		detail().set_desirable_mask		(velocity_mask | eVelocityStanding);
		detail().set_velocity_mask		(
			velocity_mask | 
			eVelocityWalk | 
			eVelocityStanding
		);
	}
}

void CStalkerMovementManager::parse_velocity_mask	()
{
	if (path().empty() || (detail().curr_travel_point_index() != m_last_turn_index))
		m_last_turn_index		= u32(-1);

	object().sight().enable		(true);

	if ((movement_type() == eMovementTypeStand) || path().empty() || (path().size() <= detail().curr_travel_point_index()) || path_completed()) {
		object().m_fCurSpeed	= 0;
		if (mental_state() != eMentalStateDanger)
			m_body.speed		= 1*PI_DIV_2;
		else
			m_body.speed		= PI_MUL_2;
		set_desirable_speed		(object().m_fCurSpeed);
		setup_head_speed		();
		m_current.m_movement_type	= eMovementTypeStand;
		return;
	}

	DetailPathManager::STravelPathPoint	point = path()[detail().curr_travel_point_index()];
	CDetailPathManager::STravelParams	current_velocity = detail().velocity(point.velocity);

	if (fis_zero(current_velocity.linear_velocity)) {
		if (mental_state() == eMentalStateFree) {
			setup_body_orientation	();
			object().sight().enable	(false);
//			Msg						("%d FALSE",Device.dwTimeGlobal);
		}
		if ((mental_state() != eMentalStateFree) || fis_zero(path_direction_angle(),EPS_L) || (m_last_turn_index == detail().curr_travel_point_index())) {
			m_last_turn_index			= detail().curr_travel_point_index();
			object().sight().enable(true);
//			Msg						("%d TRUE",Device.dwTimeGlobal);
			if (detail().curr_travel_point_index() + 1 < path().size()) {
				point				= path()[detail().curr_travel_point_index() + 1];
				current_velocity	= detail().velocity(point.velocity);
			}
		}
	}
	else {
		if (mental_state() != eMentalStateDanger) {
			if (mental_state() == eMentalStatePanic) {
				if (!fis_zero(path_direction_angle(),PI_DIV_8*.5f)) {
					u32					temp = u32(-1);
					temp				^= eVelocityFree;
					temp				^= eVelocityDanger;
					temp				^= eVelocityPanic;
					point.velocity		&= temp;
					point.velocity		|= eVelocityDanger;
					current_velocity	= detail().velocity(point.velocity);
				}
			}
			else {
				if (!fis_zero(path_direction_angle(),PI_DIV_8*.5f)) {
					setup_body_orientation	();
					object().sight().enable	(false);
					current_velocity		= detail().velocity(path()[detail().curr_travel_point_index()].velocity);
					current_velocity.linear_velocity	= 0.f;
					current_velocity.real_angular_velocity	= PI;
				}
				else
					object().sight().enable	(true);
			}
		}
	}
	
	object().m_fCurSpeed	= current_velocity.linear_velocity;
	m_body.speed			= current_velocity.real_angular_velocity;
	set_desirable_speed		(object().m_fCurSpeed);

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

	VERIFY2	((m_current.m_mental_state != eMentalStateFree) || m_current.m_body_state != eBodyStateCrouch,*object().cName());

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

void CStalkerMovementManager::set_nearest_accessible_position()
{
	set_nearest_accessible_position(object().Position(),object().ai_location().level_vertex_id());
}

void CStalkerMovementManager::set_nearest_accessible_position(Fvector desired_position, u32 level_vertex_id)
{
	if (!ai().level_graph().inside(level_vertex_id,desired_position))
		desired_position		= ai().level_graph().vertex_position(level_vertex_id);
	else
		desired_position.y		= ai().level_graph().vertex_plane_y(level_vertex_id,desired_position.x,desired_position.z);

	if (!restrictions().accessible(desired_position))
		level_vertex_id			= restrictions().accessible_nearest(Fvector().set(desired_position),desired_position);

	VERIFY						(ai().level_graph().inside(level_vertex_id,desired_position));

	VERIFY						(restrictions().accessible(level_vertex_id));
	set_level_dest_vertex		(level_vertex_id);
	
	VERIFY						(restrictions().accessible(desired_position));
	set_desired_position		(&desired_position);
}

void CStalkerMovementManager::update(u32 time_delta)
{
	VERIFY						((m_target.m_mental_state != eMentalStateFree) || (m_target.m_body_state != eBodyStateCrouch));
	m_current					= m_target;

	if (movement_type() != eMovementTypeStand)
		setup_movement_params	();

	if (script_control())
		return;

	if (movement_type() != eMovementTypeStand)
		setup_velocities		();

	if (movement_type() != eMovementTypeStand)
		update_path				();

	parse_velocity_mask			();
}

void CStalkerMovementManager::on_travel_point_change	(const u32 &previous_travel_point_index)
{
	inherited::on_travel_point_change	(previous_travel_point_index);
}

void CStalkerMovementManager::on_restrictions_change	()
{
	inherited::on_restrictions_change	();
	if (use_desired_position() && !restrictions().accessible(desired_position()))
		set_nearest_accessible_position	();
}

void CStalkerMovementManager::adjust_speed_to_animation	(const EMovementDirection &movement_direction)
{
#ifndef OLD_VELOCITIES
	VERIFY								(movement_type() != eMovementTypeStand);

	object().m_fCurSpeed				=
		m_velocities->velocity	(
			mental_state(),
			body_state(),
			movement_type(),
			movement_direction
		);

	set_desirable_speed					(object().m_fCurSpeed);
#else
	switch (body_state()) {
		case MonsterSpace::eBodyStateStand : {
			if (movement_direction != MonsterSpace::eMovementDirectionBackward) {
				if (movement_type() == MonsterSpace::eMovementTypeWalk)
					set_desirable_speed(object().m_fCurSpeed = walk_factor());
				else
					if (movement_type() == MonsterSpace::eMovementTypeRun)
						set_desirable_speed(object().m_fCurSpeed = run_factor());
			}
			else {
				if (movement_type() == MonsterSpace::eMovementTypeWalk)
					set_desirable_speed(object().m_fCurSpeed = walk_back_factor());
				else
					if (movement_type() == MonsterSpace::eMovementTypeRun)
						set_desirable_speed(object().m_fCurSpeed = run_back_factor());
			}

			break;
		};
		case eBodyStateCrouch : {
			if (movement_direction != MonsterSpace::eMovementDirectionBackward) {
				if (movement_type() == MonsterSpace::eMovementTypeWalk)
					set_desirable_speed(object().m_fCurSpeed = crouch_factor()*walk_factor());
				else
					if (movement_type() == MonsterSpace::eMovementTypeRun)
						set_desirable_speed(object().m_fCurSpeed = crouch_factor()*run_factor());
			}
			else {
				if (movement_type() == MonsterSpace::eMovementTypeWalk)
					set_desirable_speed(object().m_fCurSpeed = crouch_factor()*walk_back_factor());
				else
					if (movement_type() == MonsterSpace::eMovementTypeRun)
						set_desirable_speed(object().m_fCurSpeed = crouch_factor()*run_back_factor());
			}

			break;
		};
		default						: NODEFAULT;
	}
#endif
}
