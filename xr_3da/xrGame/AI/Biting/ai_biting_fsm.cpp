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

			CurrentState->Reset();
			CurrentState = pS;
			CurrentState->Activate();
	}
}

void CAI_Biting::OnMotionSequenceStart()
{
	// ���������� ���������
	CurrentState->LockState();
	FORCE_ANIMATION_SELECT();
}

void CAI_Biting::OnMotionSequenceEnd()
{
	// ����� ������������ Seq, m_tParams �������� ����������� ������ ��� ��������� ��������
	Motion.m_tParams.ApplyData(this);
	// �������������� �������� ���������
	CurrentState->UnlockState(m_dwCurrentUpdate);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CBitingMotion::Init(CAI_Biting *pData)
{
	m_tSeq.Setup(pData);
	m_tSeq.Init();
}

void CBitingMotion::SetFrameParams(CAI_Biting *pData) 
{
	if (!m_tSeq.isActive()) {

		m_tParams.ApplyData(pData);
		m_tTurn.CheckTurning(pData);

		//!- ��������� ���������� �� ������������� ��������. ��������� (kinda StandUp)
		if ((pData->m_tAnimPrevFrame == eMotionLieIdle) && (pData->m_tAnim != eMotionLieIdle)){
			m_tSeq.Add(eMotionStandUp,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED,STOP_ANIM_END);
			m_tSeq.Switch();
		}

		//!---
	} else {
		m_tSeq.ApplyData();
	}
}

//---------------------------------------------------------------------------------------------------------
