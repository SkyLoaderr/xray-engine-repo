////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.h
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\ai_monsters_anims.h"

extern LPCSTR caStateNames			[];
extern LPCSTR caGlobalNames			[];

class CBitingAnimations {
public:
	typedef CAniCollection<CAniVector,caGlobalNames> CStateAnimations;
	CAniCollection<CStateAnimations,caStateNames>	m_tAnims;
	CMotionDef		*m_tpCurrentGlobalAnimation;
	CBlend			*m_tpCurrentGlobalBlend;
	u8				m_bAnimationIndex;
	
					CBitingAnimations()
	{
		m_tpCurrentGlobalAnimation = 0;
	};
	
	virtual	void	Load(CKinematics *tpKinematics)
	{
		m_tAnims.Load	(tpKinematics,"");
	};
};