////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../ai_monster_debug.h"
#include "../ai_monster_group.h"


void CAI_Biting::Think()
{
	if (!g_Alive())		return;
	if (getDestroy())	return;

#ifdef DEEP_TEST_SPEED	
	if (time_next_update > Level().timeServer()) return;
	time_next_update = Level().timeServer() + 1000 / UPS;
#endif

	m_dwLastUpdateTime						= m_current_update;
	m_current_update						= Level().timeServer();

	MotionStats->update						();
	
	vfUpdateParameters						();

	CMonsterMovement::Frame_Init();

	// Squad calculations
	CMonsterSquad	*pSquad = Level().SquadMan->GetSquad((u8)g_Squad());
	if (pSquad && pSquad->SquadActive()) {
		pSquad->UpdateMonsterData(this,const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
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

	CMonsterMovement::Frame_Update			();
	
	MotionMan.Update						();

	// установить текущую скорость
	CMonsterMovement::Frame_Finalize		();
	
	// Debuging
#ifdef DEBUG
	HDebug->SetActive						(true);
#endif

	//static u32 next_bridge	= 10000;

	//if (next_bridge < m_current_update) {
	//	next_bridge = m_current_update + 5000;
	//	
	//	// remove from prev team
	//	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	//	Group.Member_Remove(this);

	//	// set new team
	//	if (g_Team() == 0){
	//		id_Team		= 1;
	//		id_Squad	= 1;
	//		id_Group	= 0;
	//	} else {
	//		id_Team		= 0;
	//		id_Squad	= 0;
	//		id_Group	= 0;
	//	}
	//	
	//	// add to new team
	//	Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	//	Group.Member_Add(this);
	//} 

	//Msg("Time[%u]  ::  Team[%u] Squad[%u] Group[u%]", m_current_update, g_Team(),g_Squad(),g_Group());
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


void CAI_Biting::State_PlaySound(u32 internal_type, u32 max_stop_time)
{
	if (m_bAngry && ((internal_type == eMonsterSoundIdle) ||  (internal_type == eMonsterSoundEat)))
		CSoundPlayer::play(MonsterSpace::eMonsterSoundGrowling, 0, 0, get_sd()->m_dwAttackSndDelay);
	else 
		CSoundPlayer::play(internal_type, 0, 0, max_stop_time);
}
