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


	//	if (CDetailPathManager::completed(Position())) {
//		set_level_dest_vertex(dynamic_cast<CAI_ObjectLocation*>(Level().CurrentEntity())->level_vertex_id());
//		set_dest_position(Level().CurrentEntity()->Position());
//		set_desirable_speed(1.f);
//		set_path_type(ePathTypeLevelPath);
//		enable_movement(true);
//		update_path();
//	}
//
//	return;
//
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
	update_path				();

	MotionMan.ProcessAction();

#pragma todo("Dima to Jim : This method will be automatically removed after 22.12.2003 00:00")
	set_desirable_speed		(m_fCurSpeed);

	// process sound
	ControlSound(m_current_update);
	
	m_head = m_body;
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

