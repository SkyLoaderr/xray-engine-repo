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

	// Pre Update path parameters
	enable_movement(true);
	
	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset();
		SetState(stateRest);
	}

	StateSelector			();
	CurrentState->Execute	(m_current_update);

	// Update path
	CDetailPathManager::set_path_type(eDetailPathTypeSmooth);
	set_desirable_speed		(m_fCurSpeed);
	update_path				();

	MotionMan.ProcessAction();


	// process sound
	ControlSound(m_current_update);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Biting state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Biting::SetState(IState *pS, bool bSkipInertiaCheck)
{
	if (CurrentState != pS) {
		// �������� �������
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

