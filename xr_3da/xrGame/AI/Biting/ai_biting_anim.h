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
class CAI_Biting;

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

// Lock animation 
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





///////////////////////////////////////////////////////////////////////////////////////////
//
//		NEW ANIMATION MANAGMENT
//
///////////////////////////////////////////////////////////////////////////////////////////

typedef string64 anim_string;

enum EAction {
	ACT_STAND_IDLE = 0,
	ACT_SIT_IDLE,
	ACT_LIE_IDLE,
	ACT_WALK_FWD,
	ACT_WALK_BKWD,
	ACT_RUN,
	ACT_EAT,
	ACT_SLEEP,
	ACT_DRAG,
	ACT_ATTACK,
	ACT_STEAL,
	ACT_LOOK_AROUND
};

DEFINE_VECTOR	(CMotionDef*, ANIM_VECTOR, ANIM_IT);

struct SAnimItem {

	anim_string	target_name;	// "stand_idle_"
	int			spec_id;		// (-1) - any,  (0 - ...) - идентификатор 3
				
	ANIM_VECTOR	pMotionVect;	// вектор указателей на анимации модели

	struct{
		float	linear;
		float	angular;
	} speed;

};

struct STransition {
	EMotionAnim	anim_from;
	EMotionAnim anim_target;
	EMotionAnim anim_transition;
	bool		chain;
};

struct SMotionItem {
	EMotionAnim		anim;
	bool			is_turn_params;

	struct{
		EMotionAnim	anim_left;			// speed, r_speed got from turn_left member
		EMotionAnim	anim_right;
		float		min_angle;
	} turn;
};


// Motion and animation Management
class CMotionManager {

	DEFINE_MAP		(EMotionAnim, SAnimItem, ANIM_ITEM_MAP, ANIM_ITEM_MAP_IT);
	DEFINE_VECTOR	(STransition, TRANSITION_ANIM_VECTOR, TRANSITION_ANIM_VECTOR_IT);
	DEFINE_VECTOR	(SMotionItem, MOTION_ITEM_VECTOR, MOTION_ITEM_VECTOR_IT);
	

	ANIM_ITEM_MAP			m_tAnims;
	TRANSITION_ANIM_VECTOR	m_tTransitions;
	MOTION_ITEM_VECTOR		m_tMotions;

	EAction					m_tAction;
	
	EMotionAnim				cur_anim; 
	EMotionAnim				prev_anim; 

	CMotionDef				*m_tpCurAnim;
	CKinematics				*tpKinematics;


	CMotionSequence			m_tSeq;
public:
			
			CMotionManager	();
	
	void	Init			(CKinematics *tpKin);
	void	Destroy			();
	
	// создание карты анимаций, переходов 
	void	AddAnim			(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed);
	void	AddTransition	(EMotionAnim from, EMotionAnim to, EMotionAnim trans, bool chain);
	
	void	AddMotion		(EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle);
	void	AddMotion		(EMotionAnim pmt_motion);
	
	bool	CheckTransition	(EMotionAnim from, EMotionAnim to);
	
//	LPCTSTR	GetTargetName	(EMotionAnim ma);
//	void	GetSpeedParams	(EMotionAnim ma);
//	void	ApplyParams		(CAI_Biting *pM);
//	void	CheckSpecFlags	();

	void	ProcessAction	();
	
	void	Play			();

	void	OnAnimationEnd	() {m_tpCurAnim = 0;}
	void	ShowDeath		();

private: 
	void	Load			(LPCTSTR pmt_name, ANIM_VECTOR	*pMotionVect);

};

