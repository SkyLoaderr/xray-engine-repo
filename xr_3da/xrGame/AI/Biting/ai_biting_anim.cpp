////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"
#include "ai_biting_anim.h"

LPCSTR caBitingStateNames			[] = {
	"stand_",				// 0
	"sit_",					// 1
	"lie_",					// 2
	0
};

LPCSTR caBitingGlobalNames		[] = {
	"idle_",				// 0
	"idle_rs_",				// 1	
	"walk_fwd_",			// 2
	"walk_bkwd_",			// 3
	"walk_ls_",				// 4
	"walk_rs_",				// 5
	"run_",					// 6
	"run_ls_",				// 7
	"run_rs_",				// 8
	"attack_",				// 9
	"attack2_",				// 10
	"attack_ls_",			// 11
	"eat_",					// 12
	"damaged_",				// 13
	"scared_",				// 14
	"die_",					// 15	
	"lie_down_",			// 16
	"stand_up_",			// 17
	"liedown_eat_",			// 18
	"attack_jump_",			// 19
	0
};



static void __stdcall vfPlayCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->OnAnimationEnd();
}

void CAI_Biting::OnAnimationEnd()
{
	m_tpCurAnim = 0;

	if (Motion.m_tSeq.Playing) {		// Sequence activated?
		Motion.m_tSeq.Switch();	
		if (Motion.m_tSeq.Finished) {
			Motion.m_tSeq.Init();		// сброс
			// После проигрывания Seq, m_tParams содержит необходимые данные для установки анимации
			Motion.m_tParams.ApplyData(this);
			// восстановление текущего состояния
			CurrentState->UnlockState(m_dwCurrentUpdate);
		}
	}
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	
	// TODO: no magic numbers!!!
	if (bShowDeath)	{
		m_tpCurAnim = m_tAnimations.A[0].A[15].A[::Random.randI((int)m_tAnimations.A[0].A[15].A.size())];
		PKinematics(Visual())->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);
		bShowDeath  = false;
		return;
	}
	
	if (g_Alive())
		if (!m_tpCurAnim && m_bActionFinished) {

			int i1, i2, i3;
			i1 = i2 = i3 = 0;			// bug protection ;) todo: find out the reason
			MotionToAnim(m_tAnim,i1,i2,i3);
			if (i3 == -1) {
				i3 = ::Random.randI((int)m_tAnimations.A[i1].A[i2].A.size());
			}
		
			m_tpCurAnim = m_tAnimations.A[i1].A[i2].A[i3];
			PKinematics(Visual())->PlayCycle(m_tpCurAnim,TRUE,vfPlayCallBack,this);

			if (i2 == 9) FillAttackStructure(i3, m_dwCurrentUpdate);
			else if (i2 == 10) FillAttackStructure(4, m_dwCurrentUpdate); // атака крыс
			else if (i2 == 19) FillAttackStructure(5, m_dwCurrentUpdate); // атака крыс|прыжок
			else m_tAttack.time_started = 0;
		}
}

bool CAI_Biting::IsInMotion()
{
	if ((m_tAnim != eMotionStandTurnLeft) && (m_tAnim != eMotionWalkFwd) && (m_tAnim != eMotionWalkBkwd) && 
		(m_tAnim != eMotionWalkTurnLeft) && (m_tAnim != eMotionWalkTurnRight) && (m_tAnim != eMotionRun) && 
		(m_tAnim != eMotionRunTurnLeft) && (m_tAnim != eMotionRunTurnRight) && (m_tAnim != eMotionFastTurnLeft)) {
		return true;
	}
	return false;
}
