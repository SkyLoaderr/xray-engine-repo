////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

void CAI_Biting::Think()
{
	if (!g_Alive()) return;


	m_dwLastUpdateTime		= m_current_update;
	m_current_update		= Level().timeServer();

	vfUpdateParameters		();

	// pre-update path parameters
	enable_movement(true);
	CLevelLocationSelector::set_evaluator(0);

	// fix off-line displacement
	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset();
		SetState(stateRest);
	}

	StateSelector			();
	CurrentState->Execute	(m_current_update);

	// update path
	CDetailPathManager::set_path_type(eDetailPathTypeSmooth);
	CDetailPathManager::set_try_min_time(true);

//	CTimer T;
//	T.Start();
	
	update_path				();

//	if (T.GetElapsed_sec() > 0.1f)  {
//		// Time checker	
//		LOG_EX("------------- PATH BUILDER OVERTIME -------------");
//		LOG_EX2("Time: %f ", *"*/ T.GetElapsed_sec() /*"*);
//		LOG_EX2("Start Node ID = [%u] Start Position = [%f,%f] ", *"*/ CDetailPathManager::path()[0].vertex_id,  CDetailPathManager::path()[0].position.x, CDetailPathManager::path()[0].position.y /*"*);
//		
//		u32 last_index = CDetailPathManager::path().size() - 1;
//		LOG_EX2("Finish Node ID = [%u] Finish Position = [%f,%f] ", *"*/ CDetailPathManager::path()[last_index].vertex_id,  CDetailPathManager::path()[last_index].position.x, CDetailPathManager::path()[last_index].position.y /*"*);	
//		
//		LOG_EX("Velocity Mask: ");
//
//		xr_map<u32,STravelParams>::iterator it;
//		for (it = m_movement_params.begin(); it != m_movement_params.end(); it++) {
//			LOG_EX2("Mask: [linear = %f, angular = %f]", *"*/ it->second.linear_velocity, it->second.angular_velocity /*"*);
//		}
//		
//		LOG_EX("Desirable Mask: ");
//		u32 m = desirable_mask();
//		
////		if ((m & eMovementParameterStand) == eMovementParameterStand) {
////			xr_map<u32,STravelParams>::const_iterator it = m_movement_params.find(eMovementParameterStand);
////			LOG_EX2("Mask: [linear = %f, angular = %f]", *"*/ it->second.linear_velocity, it->second.angular_velocity /*"*);
////		} 
////		if ((m & eMovementParameterWalkFree) == eMovementParameterWalkFree) {
////			xr_map<u32,STravelParams>::const_iterator it = m_movement_params.find(eMovementParameterWalkFree);
////			LOG_EX2("Mask: [linear = %f, angular = %f]", *"*/ it->second.linear_velocity, it->second.angular_velocity /*"*);
////		}
////		
////		if ((m & eMovementParameterRunFree) == eMovementParameterRunFree) {
////			xr_map<u32,STravelParams>::const_iterator it = m_movement_params.find(eMovementParameterRunFree);
////			LOG_EX2("Mask: [linear = %f, angular = %f]", *"*/ it->second.linear_velocity, it->second.angular_velocity /*"*);
////		}
//		
//		LOG_EX2("Start dir: [yaw = %f]", *"*/m_body.current.yaw /*"*);
//
//		LOG_EX("-------------------------------------------------");
//	}

	PreprocessAction();
	MotionMan.ProcessAction();
	
	SetVelocity();

#pragma todo("Dima to Jim : This method will be automatically removed after 22.12.2003 00:00")
	set_desirable_speed		(m_fCurSpeed);
	
	// process sound
	ControlSound(m_current_update);
	
	m_head = m_body;
}

// Установить линейную и угловую скорости в соответствии с построенным путем
void CAI_Biting::SetVelocity()
{
	if (IsMovingOnPath()) {
		u32 velocity_index = CDetailPathManager::path()[curr_travel_point_index()].velocity;

		u32 next_point_velocity = u32(-1);
		if (CDetailPathManager::path().size() > curr_travel_point_index() + 1) 
			next_point_velocity = CDetailPathManager::path()[curr_travel_point_index() + 1].velocity;

		if ((velocity_index == eVelocityParameterStand) && (next_point_velocity != u32(-1))) {
			if (angle_difference(m_body.current.yaw, m_body.target.yaw) < PI_DIV_6/6) {
				velocity_index = next_point_velocity;
			}
		}

		xr_map<u32,STravelParams>::const_iterator it = m_movement_params.find(velocity_index);
		R_ASSERT(it != m_movement_params.end());

		m_fCurSpeed		= (*it).second.linear_velocity;
		m_body.speed	= 2*(*it).second.angular_velocity;
	} else m_fCurSpeed	= 0;	
}

// В зависимости от маршрута - изменить Action
void CAI_Biting::PreprocessAction()
{
	if (IsMovingOnPath()) {
		u32 velocity_index = CDetailPathManager::path()[curr_travel_point_index()].velocity;

		if (velocity_index == eVelocityParameterStand) MotionMan.m_tAction = ACT_STAND_IDLE;
		else if (velocity_index == eVelocityParameterWalkNormal) MotionMan.m_tAction = ACT_WALK_FWD;
		else if (velocity_index == eVelocityParameterRunNormal) MotionMan.m_tAction = ACT_RUN;

		else if (velocity_index == eVelocityParameterWalkDamaged) {
			MotionMan.m_tAction = ACT_WALK_FWD;
		} else if (velocity_index == eVelocityParameterRunDamaged) {
			MotionMan.m_tAction = ACT_RUN;
		} else if (velocity_index == eVelocityParameterSteal) {
			MotionMan.m_tAction = ACT_STEAL;
		} else if (velocity_index == eVelocityParameterDrag) {
			MotionMan.m_tAction = ACT_DRAG;
		}

		u32 next_point_velocity = u32(-1);
		if (CDetailPathManager::path().size() > curr_travel_point_index() + 1) 
			next_point_velocity = CDetailPathManager::path()[curr_travel_point_index() + 1].velocity;

		if ((velocity_index == eVelocityParameterStand) && (next_point_velocity != u32(-1))) {
			if (angle_difference(m_body.current.yaw, m_body.target.yaw) < PI_DIV_6/6) {
				MotionMan.m_tAction = ACT_RUN;
			}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Biting state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Biting::SetState(IState *pS, bool bSkipInertiaCheck)
{
	if (CurrentState != pS) {
		// проверка инерций
		if (!bSkipInertiaCheck)
			if (CurrentState->IsInertia()) {
				if (CurrentState->GetPriority() >= pS->GetPriority()) return;
			}

			CurrentState->Done();
			CurrentState->Reset();
			CurrentState = pS;
			CurrentState->Activate();
	}
}
