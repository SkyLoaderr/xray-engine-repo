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

	MotionMan.accel_deactivate			();
	
	if (!MotionMan.Seq_Active()) {
		if (!UpdateStateManager()) {
			StateSelector					();
			CurrentState->Execute			(m_current_update);
		}
	} else disable_path();
	
	TranslateActionToPathParams				();

	// построить путь
	CMonsterMovement::Frame_Update			();
	
	MotionMan.Update						();

	// установить текущую скорость
	CMonsterMovement::Frame_Finalize		();

	// Debuging
#ifdef DEBUG
	HDebug->SetActive						(true);
#endif

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

