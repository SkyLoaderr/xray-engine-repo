#pragma once

#include "ai_monster_defs.h"
#include "ai_monster_share.h"


class		CAI_Biting;
class		CJumping;

typedef		ref_str			anim_string;

#define		DEFAULT_ANIM	eAnimStandIdle

// Activities
enum EMotionAnim {
	eAnimStandIdle = 0,
	eAnimStandTurnLeft,
	eAnimStandTurnRight,

	eAnimSitIdle,
	eAnimLieIdle,

	eAnimSitToSleep,
	eAnimLieToSleep,
	eAnimStandSitDown,
	eAnimStandLieDown,
	eAnimLieStandUp,
	eAnimSitStandUp,
	eAnimStandLieDownEat,
	eAnimSitLieDown,
	eAnimLieSitUp,
	eAnimSleepStandUp,

	eAnimWalkFwd,
	eAnimWalkBkwd,
	eAnimWalkTurnLeft,
	eAnimWalkTurnRight,

	eAnimRun,
	eAnimRunTurnLeft,
	eAnimRunTurnRight,
	eAnimFastTurn,					

	eAnimAttack,
	eAnimAttackRat,
	eAnimAttackFromBack,

	eAnimEat,
	eAnimSleep,
	eAnimDie,

	eAnimDragCorpse,
	eAnimCheckCorpse,
	eAnimScared,
	eAnimAttackJump,

	eAnimLookAround,

	eAnimJump,
	eAnimSteal,

	eAnimJumpStart,
	eAnimJumpGlide,		
	eAnimJumpFinish,

	eAnimJumpLeft,
	eAnimJumpRight,

	eAnimStandDamaged,
	eAnimWalkDamaged,
	eAnimRunDamaged,
	
	eAnimSniff,
	eAnimHowling,
	eAnimThreaten,

	eAnimMiscAction_00,

};

// Generic actions
enum EAction {
	ACT_STAND_IDLE = 0,
	ACT_SIT_IDLE,
	ACT_LIE_IDLE,
	ACT_WALK_FWD,
	ACT_WALK_BKWD,
	ACT_RUN,
	ACT_EAT,
	ACT_SLEEP,
	ACT_REST,
	ACT_DRAG,
	ACT_ATTACK,
	ACT_STEAL,
	ACT_LOOK_AROUND,
	ACT_JUMP
};

enum EPState {
	PS_STAND,
	PS_SIT, 
	PS_LIE,
};


// специальные параметры анимаций (animation spec params)
#define ASP_MOVE_BKWD			(1 << 0) 
#define ASP_DRAG_CORPSE			(1 << 1) 
#define ASP_CHECK_CORPSE		(1 << 2)
#define ASP_ATTACK_RAT			(1 << 3)
#define ASP_ATTACK_RAT_JUMP		(1 << 4)
#define	ASP_STAND_SCARED		(1 << 5)
#define ASP_THREATEN			(1 << 6)
#define ASP_BACK_ATTACK			(1 << 7)


DEFINE_VECTOR	(CMotionDef*, ANIM_VECTOR, ANIM_IT);

// элемент анимации
struct SAnimItem {

	anim_string	target_name;	// "stand_idle_"
	int			spec_id;		// (-1) - any,  (0 - ...) - идентификатор 3

	ANIM_VECTOR	pMotionVect;	// вектор указателей на анимации модели

	struct{
		float	linear;
		float	angular;
	} speed;

	EPState		pos_state;
};

// описание перехода
struct STransition {

	struct {
		bool		state_used;
		EMotionAnim anim;
		EPState		state;
	} from, target;

	EMotionAnim		anim_transition;
	bool			chain;
};

// элемент движения
struct SMotionItem {
	EMotionAnim		anim;
	bool			is_turn_params;

	struct{
		EMotionAnim	anim_left;			// speed, r_speed got from turn_left member
		EMotionAnim	anim_right;
		float		min_angle;
	} turn;
};

// подмена анимаций (если *flag == true, то необходимо заменить анимацию)
struct SReplacedAnim {
	EMotionAnim cur_anim;
	EMotionAnim new_anim;
	bool		*flag;
};

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// аттака крыс?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// трассировка не нужна

#define CRITICAL_STAND_TIME 1400
#define TIME_STAND_RECHECK  2000

// Определение времени аттаки по анимации
typedef struct {
	EMotionAnim	anim;				// параметры конкретной анимации 
	u32			anim_i3;

	TTime		time_from;			// диапазон времени когда можно наносить hit (от)
	TTime		time_to;		    // диапазон времени когда можно наносить hit (до)
	
	Fvector		trace_from;			// направление трассировки (относительно центра)
	Fvector		trace_to;
	
	u32			flags;				// специальные флаги

	float		damage;				// урон при данной атаке
	Fvector		hit_dir;			// угол направления приложения силы к объекту

} SAttackAnimation;


DEFINE_MAP		(EMotionAnim,		SAnimItem,				ANIM_ITEM_MAP,		ANIM_ITEM_MAP_IT);
DEFINE_VECTOR	(STransition,		TRANSITION_ANIM_VECTOR, TRANSITION_ANIM_VECTOR_IT);
DEFINE_MAP		(EAction,			SMotionItem,			MOTION_ITEM_MAP,	MOTION_ITEM_MAP_IT);
DEFINE_VECTOR	(EMotionAnim,		SEQ_VECTOR,				SEQ_VECTOR_IT);
DEFINE_VECTOR	(SAttackAnimation,	ATTACK_ANIM,			ATTACK_ANIM_IT);
DEFINE_VECTOR	(SReplacedAnim,		REPLACED_ANIM,			REPLACED_ANIM_IT);


#define HIT_SIDE_COUNT		2
#define HIT_BACK	0
#define HIT_FRONT	1

#define HIT_HEIGHT_COUNT	2
#define HIT_LOW		0
#define HIT_HIGH	1


struct t_fx_index {
	s8 front;	
	s8 back;	
};

DEFINE_MAP(u16,		t_fx_index,	FX_MAP_U16,			FX_MAP_U16_IT);			
DEFINE_MAP(ref_str,	t_fx_index,	FX_MAP_STRING,		FX_MAP_STRING_IT);


class _motion_shared : public CSharedResource {
public:
	ANIM_ITEM_MAP			m_tAnims;			// карта анимаций
	MOTION_ITEM_MAP			m_tMotions;			// карта соответсвий EAction к SMotionItem
	TRANSITION_ANIM_VECTOR	m_tTransitions;		// вектор переходов из одной анимации в другую
	ATTACK_ANIM				aa_all;				// список атак

	t_fx_index				default_fx_indexes;
	FX_MAP_STRING			fx_map_string;
	FX_MAP_U16				fx_map_u16;
	bool					map_converted;
};


//////////////////////////////////////////////////////////////////////////
class CMotionManager : public CSharedClass<_motion_shared> {

	REPLACED_ANIM			m_tReplacedAnims;	// анимации подмены
	

	CAI_Biting				*pMonster;
	CJumping				*pJumping;
	IRender_Visual			*pVisual;

	// работа с последовательностями
	SEQ_VECTOR				seq_states;
	SEQ_VECTOR_IT			seq_it;				
	bool					seq_playing;

	// сохранённые анимации 
	EMotionAnim				cur_anim; 
	EMotionAnim				prev_anim; 

	// исправления сосояния 'бега на месте'
	TTime					time_start_stand;
	EAction					prev_action;

	// работа с анимациями атаки
	TTime					aa_time_started;		// время начала анимации	
	TTime					aa_time_last_attack;	// время последнего нанесения хита
	ATTACK_ANIM				aa_stack;				// список атак для текущей анимации

	// -------------------------------------------------------------------------

	u32						spec_params;			// дополнительные параметры

	bool					should_play_die_anim;	// должен отыгрывать анимацию смерти

	bool					b_end_transition;		// запомнить анимацию конца перехода
	EMotionAnim				saved_anim;

	TTime					fx_time_last_play;

	bool					bad_motion_fixed;		// true, если монстр пытается двигаться, но стоит на месте

public:
	
	EAction					m_tAction;
	CMotionDef				*m_tpCurAnim;

public:

				CMotionManager			();
				~CMotionManager			();
	void		Init					(CAI_Biting	*pM);

	// создание карты анимаций (выполнять на Monster::Load)
	void		AddAnim					(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed, EPState p_s);

	// -------------------------------------

	// добавить анимацию перехода (A - Animation, S - Position)
	void		AddTransition			(EMotionAnim from,	EMotionAnim to, EMotionAnim trans, bool chain);
	void		AddTransition			(EMotionAnim from,	EPState to,		EMotionAnim trans, bool chain);
	void		AddTransition			(EPState from,		EMotionAnim to, EMotionAnim trans, bool chain);
	void		AddTransition			(EPState from,		EPState to,		EMotionAnim trans, bool chain);

	// -------------------------------------

	void		LinkAction				(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle);
	void		LinkAction				(EAction act, EMotionAnim pmt_motion);

	// -------------------------------------

	void		AddReplacedAnim			(bool *b_flag, EMotionAnim pmt_cur_anim, EMotionAnim pmt_new_anim);
	
	// -------------------------------------

	void		PrepareSharing			(); 
	void		NotifyShareLoaded		(); 
	
	// -------------------------------------- 	
	
	void		ApplyParams				();

	// выполнить текущий m_tAction
	void		ProcessAction			();
	// подготовить текущую анимацию на запрос из SelectAnimation
	bool		PrepareAnimation		();

	void		CheckTransition			(EMotionAnim from, EMotionAnim to);

	void		OnAnimationEnd			();
	void		ShowDeath				();

	void		SetSpecParams			(u32 param) {spec_params |= param;}
	void		SetCurAnim				(EMotionAnim a) {cur_anim = a;}
	EMotionAnim	GetCurAnim				() {return  cur_anim;} 

	// работа с последовательностями
	void		Seq_Add					(EMotionAnim a);
	void		Seq_Switch				();					// Перейти в следующее состояние, если такового не имеется - завершить
	void		Seq_Finish				();
	EMotionAnim	Seq_CurAnim				() {return ((seq_playing) ? *seq_it : eAnimStandIdle );}


	// работа с анимациями атак
	void		AA_PushAttackAnim		(SAttackAnimation AttackAnim);
	void		AA_PushAttackAnim		(EMotionAnim a, u32 i3, TTime from, TTime to, const Fvector &ray_from, const Fvector &ray_to, float damage, Fvector &dir, u32 flags = 0);
	bool		AA_CheckTime			(TTime cur_time, SAttackAnimation &anim); 
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}

	// FX's
	void		FX_LoadMap				(LPCSTR section);
	void		FX_ConvertMap			();
	void		FX_Play					(u16 bone, bool is_front, float amount);

	// Обновить tpKinematics
	void		UpdateVisual			();


	bool		BadMotionFixed			() {return bad_motion_fixed;}

private:	

	// загрузка MotionDef для определённого типа анимации 
	void		Load					(LPCSTR pmt_name, ANIM_VECTOR	*pMotionVect);
	// очистка и загрузка MotionDef для карты анимаций (выполнять на Monster::Spawn, после обновления Visual)
	void		LoadVisualData			();

	// работа с последовательностями
	void		Seq_Init				();

	// работа с анимациями атак
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);

	// дополнительные функции
	EPState		GetState				(EMotionAnim a);

	void		FixBadState				();
	bool		IsMoving				();

	void		CheckReplacedAnim		();

	// sctipting
public:

	// Вызов PrepareAnimation() и установка анимации
	void		ForceAnimSelect			();


};






