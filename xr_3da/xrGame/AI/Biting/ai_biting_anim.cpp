////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"

namespace AI_Biting {
	LPCSTR caStateNames			[] = {
		"stand_",
		"sit_",
		"lie_",
		0
	};

	LPCSTR caGlobalNames		[] = {
		"idle_",
		"walk_fwd_",
		"walk_bkwd_",
		"walk_ls_",
		"walk_rs_",
		"run_",
		"attack_",
		"eat_",
		"damage_",
		0
	};
};

static void __stdcall vfPlayCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->m_tpCurrentGlobalAnimation = 0;
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
	
	if (!m_tpCurrentGlobalAnimation) {
		m_tpCurrentGlobalAnimation = m_tAnims.A[m_tPostureAnim].A[m_tActionAnim].A[0 * ::Random.randI(m_tAnims.A[m_tPostureAnim].A[m_tActionAnim].A.size())];
		PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
	}

//	if (!m_tpCurrentGlobalAnimation) {
//		m_tpCurrentGlobalAnimation = m_tAnims.A[0].A[0].A[::Random.randI(m_tAnims.A[0].A[0].A.size())];
//		PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
//	}

}
