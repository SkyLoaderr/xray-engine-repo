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

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBitingMotion::Init()
{
	m_tSeq.Init();
}

void CBitingMotion::SetFrameParams(CAI_Biting *pData) 
{
	if (!m_tSeq.Active()) {

		m_tParams.ApplyData(pData);
		m_tTurn.CheckTurning(pData);

		//!- проверить необходимо ли устанавливать специфич. параметры (kinda StandUp)
		if ((pData->m_tAnimPrevFrame == eMotionLieIdle) && (pData->m_tAnim != eMotionLieIdle)){
			m_tSeq.Add(eMotionStandUp,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			m_tSeq.Switch();
			m_tSeq.ApplyData(pData);
		}

		//!---
	} else {
		m_tSeq.ApplyData(pData);
	}
}

//---------------------------------------------------------------------------------------------------------

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

			CurrentState->Reset();
			CurrentState = pS;
			CurrentState->Activate();
	}
}

void CAI_Biting::ControlAnimation()
{
	//-- проверка специфических анимаций --
	if (Motion.m_tSeq.Started) {
		Motion.m_tSeq.Playing = true;
		Motion.m_tSeq.Started = false;
		Motion.m_tSeq.Finished = false;
		// блокировка состо€ни€
		CurrentState->LockState();
		FORCE_ANIMATION_SELECT();
	} 

	if (!Motion.m_tSeq.Playing) {

		// ≈сли нет пути и есть анимаци€ движени€, то играть анимацию отдыха
		if (AI_Path.TravelPath.empty() || ((AI_Path.TravelPath.size() - 1) <= AI_Path.TravelStart)) {
			if ((m_tAnim == eMotionWalkFwd) || (m_tAnim == eMotionRun)) {
				m_tAnim = eMotionStandIdle;
			}
		}

		// если стоит на месте и пытаетс€ бежать...
		int i = ps_Size();		
		if (i > 1) {
			CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
			if (tCurrentPosition.vPosition.similar(tPreviousPosition.vPosition)) {
				if ((m_tAnim == eMotionWalkFwd) || (m_tAnim == eMotionRun)) {
					m_tAnim = eMotionStandIdle;
				}
			}
		}

		// если анимаци€ изменилась, переназначить анимацию
		if (m_tAnimPrevFrame != m_tAnim) {
			FORCE_ANIMATION_SELECT();
		}	
	}
	//--------------------------------------

	// —охранение предыдущей анимации
	m_tAnimPrevFrame = m_tAnim;
}

