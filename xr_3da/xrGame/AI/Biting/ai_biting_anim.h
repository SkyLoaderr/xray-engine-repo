////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.h
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////
#pragma once

class CAI_Biting;

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
	EMotionAnim		anim_from;
	EMotionAnim		anim_target;
	EMotionAnim		anim_transition;
	bool			chain;
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

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// аттака крыс?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// трассировка не нужна

#define FORCE_ANIMATION_SELECT() {				\
	m_tpCurAnim = 0;							\
	pMonster->SelectAnimation(pMonster->Direction(),pMonster->Direction(),0); \
}

// Определение времени аттаки по анимации
typedef struct {
	EMotionAnim	anim;				// параметры конкретной анимации 
	u32			anim_i3; 			

	TTime		time_from;			// диапазон времени когда можно наносить hit (от)
	TTime		time_to;		    // диапазон времени когда можно наносить hit (до)

	Fvector		trace_offset;		// направление трассировки
	float		dist;				// дистанция трассировки

	u32			flags;				// специальные флаги

	float		damage;				// урон при данной атаке
	float		dir_yaw;			// угол направления приложения силы к объекту 
	float		dir_pitch;			//  - || -
} SAttackAnimation;



// Motion and animation management
class CMotionManager {

	DEFINE_MAP		(EMotionAnim,		SAnimItem,				ANIM_ITEM_MAP,		ANIM_ITEM_MAP_IT);
	DEFINE_VECTOR	(STransition,		TRANSITION_ANIM_VECTOR, TRANSITION_ANIM_VECTOR_IT);
	DEFINE_MAP		(EAction,			SMotionItem,			MOTION_ITEM_MAP,	MOTION_ITEM_MAP_IT);
	DEFINE_VECTOR	(EMotionAnim,		SEQ_VECTOR,				SEQ_VECTOR_IT);
	DEFINE_VECTOR	(SAttackAnimation,	ATTACK_ANIM,			ATTACK_ANIM_IT);

	ANIM_ITEM_MAP			m_tAnims;			// карта анимаций
	TRANSITION_ANIM_VECTOR	m_tTransitions;		// вектор переходов из одной анимации в другую
	MOTION_ITEM_MAP			m_tMotions;			// карта соответсвий EAction к SMotionItem
	
	CAI_Biting				*pMonster;
	CKinematics				*tpKinematics;

	// работа с последовательностями
	SEQ_VECTOR				seq_states;
	SEQ_VECTOR_IT			seq_it;				// итератор (текущий элемент)
	bool					seq_playing;

	EMotionAnim				cur_anim; 
	EMotionAnim				prev_anim; 

	// работа с анимациями атаки
	TTime					aa_time_started;		// время начала анимации	
	TTime					aa_time_last_attack;	// время последнего нанесения хита
	ATTACK_ANIM				aa_all;		// список всех атак
	ATTACK_ANIM				aa_stack;	// список атак для текущей анимации

public:

	EAction					m_tAction;
	CMotionDef				*m_tpCurAnim;

public:
			
				CMotionManager			();
	
	void		Init					(CAI_Biting	*pM, CKinematics *tpKin);
	void		Destroy					();
	
	// создание карты анимаций, переходов 
	void		AddAnim					(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed);
	void		AddTransition			(EMotionAnim from, EMotionAnim to, EMotionAnim trans, bool chain);
	
	void		LinkAction				(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle);
	void		LinkAction				(EAction act, EMotionAnim pmt_motion);
	
	bool		CheckTransition			(EMotionAnim from, EMotionAnim to);
	void		ApplyParams				();

	// выполнить текущий m_tAction
	void		ProcessAction			();
	// подготовить текущую анимацию на запрос из SelectAnimation
	bool		PrepareAnimation		();

	void		OnAnimationEnd			();
	void		ShowDeath				();

private:	

	// работа с последовательностями
	void		Seq_Init				();
	void		Seq_Add					(EMotionAnim a);
	void		Seq_Switch				();					// Перейти в следующее состояние, если такового не имеется - завершить
	void		Seq_Finish				();

	// загрузка анимаций
	void		Load					(LPCTSTR pmt_name, ANIM_VECTOR	*pMotionVect);

	// работа с анимациями атак
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);
public:
	void		AA_PushAttackAnim		(SAttackAnimation AttackAnim);
	void		AA_PushAttackAnim		(EMotionAnim a, u32 i3, TTime from, TTime to, Fvector &ray, 
										 float dist, float damage, float yaw, float pitch, u32 flags = 0);
	bool		AA_CheckTime			(TTime cur_time, SAttackAnimation &anim); 
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}
};


