////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../phmovementcontrol.h"


void CAI_Biting::Think()
{
	if (!g_Alive()) return;
		
#ifdef DEEP_TEST_SPEED	
	if (time_next_update > Level().timeServer()) return;
	time_next_update = Level().timeServer() + 1000 / UPS;
#endif

	m_dwLastUpdateTime						= m_current_update;
	m_current_update						= Level().timeServer();
	
	MotionStats->update						();
	
	vfUpdateParameters						();

	if (m_PhysicMovementControl->JumpState()) enable_movement(false);

	CMonsterMovement::Frame_Init();

	// fix off-line displacement
	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset					();
		SetState							(stateRest);
	}
	
	// Squad calculations
	CMonsterSquad	*pSquad = Level().SquadMan.GetSquad((u8)g_Squad());
	if (pSquad && pSquad->SquadActive()) {
		pSquad->UpdateMonsterData(this,const_cast<CEntity *>(m_tEnemy.obj));
		if ((pSquad->GetLeader() == this)) {
			pSquad->UpdateDecentralized();
		} 
	}
	
	if (MotionMan.Seq_Active()) disable_path();
	else {
		// Выбор текущего состояния
		StateSelector						();
		CurrentState->Execute				(m_current_update);
	}
	// Обработать action
	MotionMan.ProcessAction					();

	UpdatePathWithAction					();

	if (CMonsterMovement::is_path_targeted())
		UpdateTargetVelocityWithPath		();
	
	// построить путь
	CMonsterMovement::Frame_Update			();

	MotionMan.FinalizeProcessing			();

	// установить текущую скорость
	CMonsterMovement::Frame_Finalize		();

	// Debuging
#ifdef DEBUG
	HDebug->SetActive						(true);
#endif

}


// В зависимости от маршрута - изменить Action
void CAI_Biting::UpdateActionWithPath()
{
	if (IsMovingOnPath()) {
		u32 cur_point_velocity_index = CDetailPathManager::path()[curr_travel_point_index()].velocity;

		if (cur_point_velocity_index == eVelocityParameterStand) 				MotionMan.m_tAction = ACT_STAND_IDLE;
		else if (cur_point_velocity_index == eVelocityParameterWalkNormal)		MotionMan.m_tAction = ACT_WALK_FWD;
		else if (cur_point_velocity_index == eVelocityParameterRunNormal)		MotionMan.m_tAction = ACT_RUN;
		else if (cur_point_velocity_index == eVelocityParameterWalkDamaged)		MotionMan.m_tAction = ACT_WALK_FWD;
		else if (cur_point_velocity_index == eVelocityParameterRunDamaged)		MotionMan.m_tAction = ACT_RUN;
		else if (cur_point_velocity_index == eVelocityParameterSteal)			MotionMan.m_tAction = ACT_STEAL;
		else if (cur_point_velocity_index == eVelocityParameterDrag)			MotionMan.m_tAction = ACT_DRAG;

		u32 next_point_velocity_index = u32(-1);
		if (CDetailPathManager::path().size() > curr_travel_point_index() + 1) 
			next_point_velocity_index = CDetailPathManager::path()[curr_travel_point_index() + 1].velocity;

		if ((cur_point_velocity_index == eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (angle_difference(m_body.current.yaw, m_body.target.yaw) < PI_DIV_6/6) {
				switch (next_point_velocity_index) {
					case eVelocityParameterWalkNormal:	MotionMan.m_tAction = ACT_WALK_FWD; break;
					case eVelocityParameterRunNormal:	MotionMan.m_tAction = ACT_RUN;		break;
					case eVelocityParameterWalkDamaged:	MotionMan.m_tAction = ACT_WALK_FWD;	break;
					case eVelocityParameterRunDamaged:	MotionMan.m_tAction = ACT_RUN;		break;
					case eVelocityParameterSteal:		MotionMan.m_tAction = ACT_STEAL;	break;
					case eVelocityParameterDrag:		MotionMan.m_tAction = ACT_DRAG;		break;
				}
			}
		}
	} 
}

// Установить линейную и угловую скорости в соответствии с построенным путем
bool CAI_Biting::UpdateVelocityWithPath()
{
//	if (MotionMan.Seq_Active()) return;
		
	if (IsMovingOnPath()) {
		u32 cur_point_velocity_index = CDetailPathManager::path()[curr_travel_point_index()].velocity;

		u32 next_point_velocity_index = u32(-1);
		if (CDetailPathManager::path().size() > curr_travel_point_index() + 1) 
			next_point_velocity_index = CDetailPathManager::path()[curr_travel_point_index() + 1].velocity;

		if ((cur_point_velocity_index == eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
			if (angle_difference(m_body.current.yaw, m_body.target.yaw) < PI_DIV_6/6) {
				cur_point_velocity_index = next_point_velocity_index;
			}
		}

		xr_map<u32,STravelParams>::const_iterator it = m_movement_params.find(cur_point_velocity_index);
		R_ASSERT(it != m_movement_params.end());

		m_velocity_linear.target	= _abs((*it).second.linear_velocity);
		m_velocity_angular.target	= (*it).second.angular_velocity;
		
		if (fis_zero((*it).second.linear_velocity)) m_velocity_linear.current = 0.f;
		
		return true;
	} 
	
	return false;
}


void CAI_Biting::UpdatePathWithAction()
{
	if (MotionMan.IsStandCurAnim()) {
		disable_path();
		m_velocity_linear.current = 0.f;
	} else enable_path();
}

void CAI_Biting::UpdateTargetVelocityWithPath()
{
	float acceleration	= GetAcceleration();
	float dist_to_end	= (m_velocity_linear.current * m_velocity_linear.current) / acceleration;
	
	if (IsPathEnd(dist_to_end) && IsMovingOnPath()) {
		m_velocity_linear.target = 0.f;
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

