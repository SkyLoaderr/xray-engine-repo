////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.h
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\ai_monsters_anims.h"

#define FORCE_ANIMATION_SELECT() {\
	m_tpCurAnim = 0; \
	SelectAnimation(Direction(),Direction(),0);\
}

extern LPCSTR caBitingStateNames	[];
extern LPCSTR caBitingGlobalNames	[];

class CBitingAnimations {
protected:
	CBitingAnimations() {
		
		m_tpCurAnim = 0;

	};

	void	Load(CKinematics *tpKinematics) {
		m_tAnimations.Load	(tpKinematics,"");
	};

protected:
	typedef CAniCollection<CAniVector,caBitingGlobalNames> CStateAnimations;
	
	CAniCollection<CStateAnimations,caBitingStateNames>	m_tAnimations;
	u8				m_bAnimationIndex;

public:
	CMotionDef		*m_tpCurAnim;
};



// Attack 
typedef struct {
	u32		i_anim;				// анимация аттаки

	TTime	time_started;		//
	TTime	time_from;			// диапазон времени когда можно наносить hit
	TTime	time_to;

	Fvector	TraceFrom;
	float	dist;

	TTime	LastAttack;
	bool	b_fire_anyway;		// трассировка не нужна
	bool	b_attack_rat;

} SAttackAnimation;



