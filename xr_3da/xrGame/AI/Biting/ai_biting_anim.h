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

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// ������ ����?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// ����������� �� �����

// ����������� ������� ������ �� ��������
typedef struct {
	// ������� ���������� �������� 
	u32		anim_i1;
	u32		anim_i2;
	u32		anim_i3;

	TTime	time_from;			// �������� ������� ����� ����� �������� hit (��)
	TTime	time_to;		    // �������� ������� ����� ����� �������� hit (��)

	Fvector	trace_offset;		// ����������� �����������
	float	dist;				// ��������� �����������

	// ����������� �����
	u32		flags;				

	float	damage;				// ���� ��� ������ �����
	float	dir_yaw;			// ���� ����������� ���������� ���� � ������� 
	float	dir_pitch;			//  - || -
} SAttackAnimation;


DEFINE_VECTOR(SAttackAnimation, ATTACK_ANIM, ATTACK_ANIM_IT);

class CAttackAnim {

	TTime			time_started;		// ����� ������ ��������	
	TTime			time_last_attack;	// ����� ���������� ��������� ����

	ATTACK_ANIM		m_all;		// ������ ���� ����
	ATTACK_ANIM		m_stack;	// ������ ���� ��� ������� ��������

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

