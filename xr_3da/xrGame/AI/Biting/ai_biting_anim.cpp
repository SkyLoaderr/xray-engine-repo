////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"
#include "ai_biting_space.h"

using namespace AI_Biting;

namespace AI_Biting {
	LPCSTR caStateNames			[] = {
		"stand_",				// 0
		"sit_",					// 1
		"lie_",					// 2
		0
	};

	LPCSTR caGlobalNames		[] = {
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
		"damage_",				// 13
		"scared_",				// 14
		"die_",					// 15	
		"lie_down_",			// 16
		"stand_up_",			// 17
		"liedown_eat_",			// 18
		0
	};
};

static void __stdcall vfPlayCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->OnAnimationEnd();
}

void CAI_Biting::OnAnimationEnd()
{
	m_tpCurrentGlobalAnimation = 0;

	if (Motion.m_tSeq.Playing) {		// Sequence activated?
		Motion.m_tSeq.Switch();	
		if (Motion.m_tSeq.Finished) {
			Motion.m_tSeq.Init();		// сброс
			// ѕосле проигрывани€ Seq, m_tParams содержит необходимые данные дл€ установки анимации
			Motion.m_tParams.ApplyData(this);
			// восстановление текущего состо€ни€
			CurrentState->UnlockState(m_dwCurrentUpdate);
		}
	}
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	if (bShowDeath)	{
		m_tpCurrentGlobalAnimation = m_tAnimations.A[ePostureStand].A[eActionDie].A[::Random.randI((int)m_tAnimations.A[ePostureStand].A[eActionDie].A.size())];
		PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
		bShowDeath  = false;
		return;
	}
	
	if (g_Alive())
		if (!m_tpCurrentGlobalAnimation && m_bActionFinished) {

			int i1,i2,i3;
			MotionToAnim(m_tAnim,i1,i2,i3);
			if (i3 == -1) {
				i3 = ::Random.randI((int)m_tAnimations.A[i1].A[i2].A.size());
			}
		
			m_tpCurrentGlobalAnimation = m_tAnimations.A[i1].A[i2].A[i3];
			PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);

			if (i2 == 9) FillAttackStructure(i3, m_dwCurrentUpdate);
			else if (i2 == 10) FillAttackStructure(4, m_dwCurrentUpdate); // атака крыс
			else m_tAttack.time_started = 0;

		}
}


void CAI_Biting::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	switch(motion) {
		case eMotionStandIdle:		index1 = 0; index2 = 0;	 index3 = -1;	break;
		case eMotionLieIdle:		index1 = 2; index2 = 0;	 index3 = -1;	break;
		case eMotionStandTurnLeft:	index1 = 0; index2 = 1;	 index3 = -1;	break;
		case eMotionWalkFwd:		index1 = 0; index2 = 2;	 index3 = -1;	break;
		case eMotionWalkBkwd:		index1 = 0; index2 = 3;  index3 = -1;	break;
		case eMotionWalkTurnLeft:	index1 = 0; index2 = 4;  index3 = -1;	break;
		case eMotionWalkTurnRight:	index1 = 0; index2 = 5;  index3 = -1;	break;
		case eMotionRun:			index1 = 0; index2 = 6;  index3 = -1;	break;
		case eMotionRunTurnLeft:	index1 = 0; index2 = 7;  index3 = -1;	break;
		case eMotionRunTurnRight:	index1 = 0; index2 = 8;  index3 = -1;	break;
		case eMotionAttack:			index1 = 0; index2 = 9;  index3 = -1;	break;
		case eMotionAttackRat:		index1 = 0; index2 = 10; index3 = -1;	break;
		case eMotionFastTurnLeft:	index1 = 0; index2 = 11; index3 = -1;	break;
		case eMotionEat:			index1 = 2; index2 = 12; index3 = -1;	break;
		case eMotionDamage:			index1 = 0; index2 = 13; index3 = -1;	break;
		case eMotionScared:			index1 = 0; index2 = 14; index3 = -1;	break;
		case eMotionDie:			index1 = 0; index2 = 15; index3 = -1;	break;
		case eMotionLieDown:		index1 = 0; index2 = 16; index3 = -1;	break;
		case eMotionStandUp:		index1 = 2; index2 = 17; index3 = -1;	break;
		case eMotionCheckCorpse:	index1 = 0; index2 = 0;	 index3 = 2;	break;
		case eMotionLieDownEat:		index1 = 0; index2 = 18; index3 = -1;	break;
	}
}
