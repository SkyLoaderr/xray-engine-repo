////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.h
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\ai_monsters_anims.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Biting Animation class
////////////////////////////////////////////////////////////////////////////////////////////////////////

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


struct SLockAnim {
	EMotionAnim	anim;
	int			i3;
	TTime		from;
	TTime		to;
};


DEFINE_VECTOR(SLockAnim, LOCK_ANIM_VECTOR, LOCK_ANIM_IT);


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation
////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// аттака крыс?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// трассировка не нужна

// Определение времени аттаки по анимации
typedef struct {
	// индексы конкретной анимации 
	u32		anim_i1;
	u32		anim_i2;
	u32		anim_i3;

	TTime	time_from;			// диапазон времени когда можно наносить hit (от)
	TTime	time_to;		    // диапазон времени когда можно наносить hit (до)

	Fvector	trace_offset;		// направление трассировки
	float	dist;				// дистанция трассировки

	// специальные флаги
	u32		flags;				

	float	damage;				// урон при данной атаке
	float	dir_yaw;			// угол направления приложения силы к объекту 
	float	dir_pitch;			//  - || -
} SAttackAnimation;


DEFINE_VECTOR(SAttackAnimation, ATTACK_ANIM, ATTACK_ANIM_IT);

class CAttackAnim {

	TTime			time_started;		// время начала анимации	
	TTime			time_last_attack;	// время последнего нанесения хита

	ATTACK_ANIM		m_all;		// список всех атак
	ATTACK_ANIM		m_stack;	// список атак для текущей анимации

public:
	void		Clear				(); 

	void		SwitchAnimation		(TTime cur_time, u32 i1, u32 i2, u32 i3);
	void		PushAttackAnim		(SAttackAnimation AttackAnim);
	void		PushAttackAnim		(u32 i1, u32 i2, u32 i3, TTime from, TTime to, Fvector &ray, 
									 float dist, float damage, float yaw, float pitch, u32 flags = 0);

	bool		CheckTime			(TTime cur_time, SAttackAnimation &anim); 
	void		UpdateLastAttack	(TTime cur_time) {time_last_attack = cur_time;}

	ATTACK_ANIM	&GetStack			() {return m_stack;}
};

