////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.h
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\ai_monsters_anims.h"
#include "ai_biting_space.h"

namespace AI_Biting {
	extern LPCSTR caStateNames		[];
	extern LPCSTR caGlobalNames		[];
};

#define FORCE_ANIMATION_SELECT() {\
	m_tpCurrentGlobalAnimation = 0; \
	SelectAnimation(Direction(),Direction(),0);\
}


class CBitingAnimations {
protected:
	CBitingAnimations()
	{
		m_tpCurrentGlobalAnimation = 0;
	};

	virtual	void	Load(CKinematics *tpKinematics)
	{
		m_tAnimations.Load	(tpKinematics,"");
	};

protected:
	typedef CAniCollection<CAniVector,AI_Biting::caGlobalNames> CStateAnimations;
	CAniCollection<CStateAnimations,AI_Biting::caStateNames>	m_tAnimations;
	u8				m_bAnimationIndex;

public:
	CMotionDef		*m_tpCurrentGlobalAnimation;
};



