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

	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	vfUpdateParameters		();

//#ifndef SILENCE
//	if (g_Alive())
//		Msg("%s : [A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][H=%d][I=%d][J=%d][K=%d]",cName(),A,B,C,D,E,F,H,I,J,K);
//#endif
	
	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset();
		SetState(stateRest);
	}
	
	StateSelector			();
	CurrentState->Execute	(m_dwCurrentUpdate);

	MotionMan.ProcessAction();
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

