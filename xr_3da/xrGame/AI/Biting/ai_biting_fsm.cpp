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

	//if (IsMovingOnPath()) {
	//	HDebug->L_Clear();
	//	for (u32 i=0; i<CDetailPathManager::path().size();i++) {
	//		xr_map<u32,CDetailPathManager::STravelParams>::const_iterator it = m_movement_params.find(CDetailPathManager::path()[i].velocity);
	//		VERIFY(it != m_movement_params.end());

	//		if (fis_zero((*it).second.linear_velocity))
	//			HDebug->L_AddPoint(CDetailPathManager::path()[i].position,0.15f,D3DCOLOR_XRGB(255,0,100));
	//		else 
	//			HDebug->L_AddPoint(CDetailPathManager::path()[i].position,0.15f,D3DCOLOR_XRGB(0,255,100));
	//	}
	//}


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
