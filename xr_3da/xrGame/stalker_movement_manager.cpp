////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_movement_manager.h"
#include "ai_script_actions.h"
#include "custommonster.h"

CStalkerMovementManager::CStalkerMovementManager	()
{
	Init							();
}

CStalkerMovementManager::~CStalkerMovementManager	()
{
}

void CStalkerMovementManager::Init					()
{
}

void CStalkerMovementManager::Load					(LPCSTR section)
{
	inherited::Load					(section);
	m_fCrouchFactor					= pSettings->r_float(section,"CrouchFactor");
	m_fWalkFactor					= pSettings->r_float(section,"WalkFactor");
	m_fRunFactor					= pSettings->r_float(section,"RunFactor");
	m_fWalkFreeFactor				= pSettings->r_float(section,"WalkFreeFactor");
	m_fRunFreeFactor				= pSettings->r_float(section,"RunFreeFactor");
	m_fPanicFactor					= pSettings->r_float(section,"PanicFactor");
	m_fDamagedWalkFactor			= pSettings->r_float(section,"DamagedWalkFactor");
	m_fDamagedRunFactor				= pSettings->r_float(section,"DamagedRunFactor");
	m_fDamagedWalkFreeFactor		= pSettings->r_float(section,"DamagedWalkFreeFactor");
	m_fDamagedRunFreeFactor			= pSettings->r_float(section,"DamagedRunFreeFactor");
	m_fDamagedPanicFactor			= pSettings->r_float(section,"DamagedPanicFactor");
	float							cf = 2.f;

	m_movement_params.insert		(std::make_pair(eMovementParameterStandingFreeStand						,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingPanicStand					,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingDangerStand					,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingFreeStandDamaged				,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingPanicStandDamaged				,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingDangerStandDamaged			,STravelParams(0.f									,PI_MUL_2	)));

	m_movement_params.insert		(std::make_pair(eMovementParameterStandingFreeCrouch					,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingPanicCrouch					,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingDangerCrouch					,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingFreeCrouchDamaged				,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingPanicCrouchDamaged			,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterStandingDangerCrouchDamaged			,STravelParams(0.f									,PI_MUL_2	)));

	m_movement_params.insert		(std::make_pair(eMovementParameterWalkFreePositive						,STravelParams(m_fWalkFreeFactor					,PI_DIV_8/1	,cf*PI_DIV_8/1	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunFreePositive						,STravelParams(m_fRunFreeFactor						,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerStandPositive				,STravelParams(m_fWalkFactor						,100*PI		,cf*PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerCrouchPositive				,STravelParams(m_fWalkFactor*m_fCrouchFactor		,3*PI_DIV_2	,cf*3*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerStandPositive				,STravelParams(m_fRunFactor							,100*PI		,cf*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerCrouchPositive				,STravelParams(m_fRunFactor*m_fCrouchFactor			,PI			,cf*PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunPanicStandPositive					,STravelParams(m_fPanicFactor						,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
																																											
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkFreeDamagedPositive				,STravelParams(m_fDamagedWalkFreeFactor				,PI_DIV_8	,cf*PI_DIV_8	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunFreeDamagedPositive				,STravelParams(m_fDamagedRunFreeFactor				,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerStandDamagedPositive		,STravelParams(m_fDamagedWalkFactor					,PI			,cf*PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerCrouchDamagedPositive		,STravelParams(m_fWalkFactor*m_fCrouchFactor		,3*PI_DIV_2	,cf*3*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerStandDamagedPositive			,STravelParams(m_fDamagedRunFactor					,PI_DIV_2	,cf*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerCrouchDamagedPositive		,STravelParams(m_fRunFactor*m_fCrouchFactor			,PI			,cf*PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunPanicDamagedStandPositive			,STravelParams(m_fDamagedPanicFactor				,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
																																											
//	m_movement_params.insert		(std::make_pair(eMovementParameterWalkFreeNegative						,STravelParams(-m_fWalkFreeFactor					,PI_DIV_8/1	,cf*PI_DIV_8/1	)));
//	m_movement_params.insert		(std::make_pair(eMovementParameterRunFreeNegative						,STravelParams(-m_fRunFreeFactor					,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerStandNegative				,STravelParams(-m_fWalkFactor						,PI			,cf*PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerCrouchNegative				,STravelParams(-m_fWalkFactor*m_fCrouchFactor		,3*PI_DIV_2	,cf*3*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerStandNegative				,STravelParams(-m_fRunFactor						,PI_DIV_2	,cf*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerCrouchNegative				,STravelParams(-m_fRunFactor*m_fCrouchFactor		,PI			,cf*PI			)));
//	m_movement_params.insert		(std::make_pair(eMovementParameterRunPanicStandNegative					,STravelParams(-m_fPanicFactor						,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
																																											
//	m_movement_params.insert		(std::make_pair(eMovementParameterWalkFreeDamagedNegative				,STravelParams(-m_fDamagedWalkFreeFactor			,PI_DIV_8	,cf*PI_DIV_8	)));
//	m_movement_params.insert		(std::make_pair(eMovementParameterRunFreeDamagedNegative				,STravelParams(-m_fDamagedRunFreeFactor				,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerStandDamagedNegative		,STravelParams(-m_fDamagedWalkFactor				,PI			,cf*PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerCrouchDamagedNegative		,STravelParams(-m_fWalkFactor*m_fCrouchFactor		,3*PI_DIV_2	,cf*3*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerStandDamagedNegative			,STravelParams(-m_fDamagedRunFactor					,PI_DIV_2	,cf*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerCrouchDamagedNegative		,STravelParams(-m_fRunFactor*m_fCrouchFactor		,PI			,cf*PI			)));
//	m_movement_params.insert		(std::make_pair(eMovementParameterRunPanicDamagedStandNegative			,STravelParams(-m_fDamagedPanicFactor				,PI_DIV_8/2	,cf*PI_DIV_8/2	)));
}

void CStalkerMovementManager::reinit				()
{
	inherited::reinit				();
	m_body_state					= eBodyStateStand;
	m_movement_type					= eMovementTypeStand;
	m_mental_state					= eMentalStateFree;
	CCustomMonster					*custom_monster = dynamic_cast<CCustomMonster*>(this);
	VERIFY							(custom_monster);
	custom_monster->m_body.speed	= PI_MUL_2;
	m_head.speed					= 3*PI_DIV_2;

	m_use_desired_position			= false;
	m_use_desired_direction			= false;
	m_desired_position				= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
	m_desired_direction				= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));

	m_path_evaluator				= 0;
	m_node_evaluator				= 0;
	m_body_state					= eBodyStateStand;
	m_movement_type					= eMovementTypeStand;
	m_mental_state					= eMentalStateDanger;
	m_path_type						= ePathTypeNoPath;
	m_detail_path_type				= eDetailPathTypeSmooth;
}

void CStalkerMovementManager::reload				(LPCSTR section)
{
	inherited::reload				(section);
}

void CStalkerMovementManager::update(u32 time_delta)
{
	CMovementManager::set_path_type						(m_path_type);
	CDetailPathManager::set_path_type					(m_detail_path_type);
	CLevelLocationSelector::set_evaluator				(m_node_evaluator);
	CMovementManager::CLevelPathManager::set_evaluator	(m_path_evaluator ? m_path_evaluator : base_level_selector());
	
	if (m_use_desired_position) {
		VERIFY											(valid(m_desired_position));
		CDetailPathManager::set_dest_position			(m_desired_position);
	}
	else
		if ((m_path_type != ePathTypePatrolPath) && (m_path_type != ePathTypeGamePath))
			CDetailPathManager::set_dest_position		(ai().level_graph().vertex_position(CLevelPathManager::dest_vertex_id()));

	if (m_use_desired_direction) {
		VERIFY											(valid(m_desired_direction));
		CDetailPathManager::set_dest_direction			(m_desired_direction);
		CDetailPathManager::set_use_dest_orientation	(true);
	}
	else
		CDetailPathManager::set_use_dest_orientation	(false);

	CScriptMonster			*script_monster = dynamic_cast<CScriptMonster*>(this);
	VERIFY					(script_monster);

	CCustomMonster			*custom_monster = dynamic_cast<CCustomMonster*>(this);
	VERIFY					(custom_monster);

	if (script_monster->GetScriptControl() && script_monster->GetCurrentAction() && (_abs(script_monster->GetCurrentAction()->m_tMovementAction.m_fSpeed) > EPS_L)) {
		custom_monster->m_fCurSpeed	= script_monster->GetCurrentAction()->m_tMovementAction.m_fSpeed;
		set_desirable_speed	(custom_monster->m_fCurSpeed);
		return;
	}

	const CEntityCondition	*entity_condition = dynamic_cast<const CEntityCondition*>(this);
	VERIFY					(entity_condition);
	u32						velocity_mask = eMovementParameterPositiveVelocity | (entity_condition->IsLimping() ? eMovementParameterDamaged : 0);

	switch (m_body_state) {
		case eBodyStateCrouch : {
			velocity_mask	|= eMovementParameterCrouch;
			break;
		}
		case eBodyStateStand : {
			velocity_mask	|= eMovementParameterStand;
			break;
		}
		case eBodyStateStandDamaged : {
			velocity_mask	|= eMovementParameterStand | eMovementParameterDamaged;
			break;
		}
		default : NODEFAULT;
	}

	switch (m_mental_state) {
		case eMentalStateDanger : {
			velocity_mask	|= eMovementParameterDanger;
			break;
		}
		case eMentalStateFree : {
			velocity_mask	|= eMovementParameterFree;
			break;
		}
		case eMentalStatePanic : {
			velocity_mask	|= eMovementParameterPanic;
			break;
		}
	}

//	if (!CDetailPathManager::path().empty() && ((CDetailPathManager::path().size() - 1) > CDetailPathManager::curr_travel_point_index())) {
		switch (m_movement_type) {
			case eMovementTypeWalk : {
				velocity_mask	|= eMovementParameterWalk;
				break;
			}
			case eMovementTypeRun : {
				velocity_mask	|= eMovementParameterRun;
				break;
			}
			default : {
				velocity_mask	|= eMovementParameterStanding;
				velocity_mask	&= u32(-1) ^ (eMovementParameterNegativeVelocity | eMovementParameterPositiveVelocity);
			}
		}
//	}
//	else
//		velocity_mask	|= eMovementParameterStanding;

	xr_map<u32,STravelParams>::const_iterator	I = m_movement_params.find(velocity_mask);
	VERIFY							(I != m_movement_params.end());

	if (CDetailPathManager::path().size()) {
//		if (angle_difference(m_body.current.yaw,m_body.target.yaw) > PI_DIV_8)

//		u32							path_velocity = CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].velocity;
//		xr_map<u32,STravelParams>::const_iterator	J = m_movement_params.find(path_velocity);
//		VERIFY						(J != m_movement_params.end());
//		if ((fis_zero((*J).second.angular_velocity) || ((*J).second.linear_velocity/(*J).second.angular_velocity) < ((*I).second.linear_velocity/(*I).second.angular_velocity + EPS_L)) &&
//			(CDetailPathManager::path().size() > CDetailPathManager::curr_travel_point_index() + 1)) {
//			Fvector					t;
//			t.sub					(
//				CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].position,
//				CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].position
//			);
//			float					y,p;
//			t.getHP					(y,p);
//			if (angle_difference(-y,m_body.current.yaw) > PI_DIV_8)
//				I					= J;
//		}
//		if ((fis_zero((*J).second.angular_velocity) || ((*J).second.linear_velocity/(*J).second.angular_velocity) < ((*I).second.linear_velocity/(*I).second.angular_velocity - EPS_L)) &&
//			(angle_difference(m_body.current.yaw,m_body.target.yaw) > PI_DIV_8))
//			I					= J;
	}
	custom_monster->m_fCurSpeed		= (*I).second.linear_velocity;
	custom_monster->m_body.speed	= (*I).second.real_angular_velocity;
	m_head.speed					= 3*PI_DIV_2;

	if (m_movement_type != eMovementTypeStand)
		if ((velocity_mask & eMovementParameterDanger))// && (velocity_mask & eMovementParameterWalk))
			set_velocity_mask		(velocity_mask | eMovementParameterStanding);
		else
			set_velocity_mask		(velocity_mask | eMovementParameterWalk | eMovementParameterRun | eMovementParameterStanding | eMovementParameterNegativeVelocity);
	if ((velocity_mask & eMovementParameterStanding) != eMovementParameterStanding)
		set_desirable_mask			(velocity_mask);

	update_path						();

#pragma todo("Dima to Dima : This method will be automatically removed after 22.12.2003 00:00")
	set_desirable_speed				(custom_monster->m_fCurSpeed);
}