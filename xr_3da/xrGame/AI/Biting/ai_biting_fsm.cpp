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
	
	if (Motion.m_tSeq.isActive())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		StateSelector			();

		CurrentState->Execute(m_dwCurrentUpdate);

		// провер€ем на завершЄнность
		if (CurrentState->CheckCompletion()) SetState(stateRest, true);
	}

	//ProcessAction();
//	Motion.SetFrameParams(this);
//	ControlAnimation();		
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

void CAI_Biting::OnMotionSequenceStart()
{
	// блокировка состо€ни€
	CurrentState->LockState();
	FORCE_ANIMATION_SELECT();
}

void CAI_Biting::OnMotionSequenceEnd()
{
	// ѕосле проигрывани€ Seq, m_tParams содержит необходимые данные дл€ установки анимации
	Motion.m_tParams.ApplyData(this);
	// восстановление текущего состо€ни€
	CurrentState->UnlockState(m_dwCurrentUpdate);
}

void CAI_Biting::CheckTransitionAnims()
{
	if ((m_tAnimPrevFrame == eAnimLieIdle) && (m_tAnim != eAnimLieIdle)){
		Motion.m_tSeq.Add(eAnimLieStandUp,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED,STOP_ANIM_END);
		Motion.m_tSeq.Switch();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBitingMotion::Init(CAI_Biting *pData)
{
	m_tSeq.Setup(pData);
	m_tSeq.Init();
	m_tTurn.SetMoveBkwd(false);
}

void CBitingMotion::SetFrameParams(CAI_Biting *pData) 
{
	if (!m_tSeq.isActive()) {
		// Process action here
//		EAction cur_act;
//		pData->ProcessAction(cur_act);

//		m_tParams.ApplyData(pData);
//		m_tTurn.CheckTurning(pData);
//
//		//!- проверить необходимо ли устанавливать специфич. параметры (kinda StandUp)
//		pData->CheckTransitionAnims();
//		
//		//!---

	} else {
		m_tSeq.ApplyData();
	}
}

//---------------------------------------------------------------------------------------------------------
