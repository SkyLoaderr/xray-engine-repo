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


// ����������� ��������� �������� (animation spec params)
#define ASP_MOVE_BKWD			(1 << 0) 
#define ASP_DRAG_CORPSE			(1 << 1) 
#define ASP_CHECK_CORPSE		(1 << 2)
#define ASP_ATTACK_RAT			(1 << 3)
#define ASP_ATTACK_RAT_JUMP		(1 << 4)
#define	ASP_STAND_SCARED		(1 << 5)

DEFINE_VECTOR	(CMotionDef*, ANIM_VECTOR, ANIM_IT);

// ������� ��������
struct SAnimItem {

	anim_string	target_name;	// "stand_idle_"
	int			spec_id;		// (-1) - any,  (0 - ...) - ������������� 3

	ANIM_VECTOR	pMotionVect;	// ������ ���������� �� �������� ������

	struct{
		float	linear;
		float	angular;
	} speed;

	EPState		pos_state;
};

// �������� ��������
struct STransition {

	struct {
		bool		state_used;
		EMotionAnim anim;
		EPState		state;
	} from, target;

	EMotionAnim		anim_transition;
	bool			chain;
};

// ������� ��������
struct SMotionItem {
	EMotionAnim		anim;
	bool			is_turn_params;

	struct{
		EMotionAnim	anim_left;			// speed, r_speed got from turn_left member
		EMotionAnim	anim_right;
		float		min_angle;
	} turn;
};

// ������� �������� (���� *flag == true, �� ���������� �������� ��������)
struct SReplacedAnim {
	EMotionAnim cur_anim;
	EMotionAnim new_anim;
	bool		*flag;
};

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// ������ ����?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// ����������� �� �����

#define CRITICAL_STAND_TIME 1400
#define TIME_STAND_RECHECK  2000

// ����������� ������� ������ �� ��������
typedef struct {
	EMotionAnim	anim;				// ��������� ���������� �������� 
	u32			anim_i3; 			

	TTime		time_from;			// �������� ������� ����� ����� �������� hit (��)
	TTime		time_to;		    // �������� ������� ����� ����� �������� hit (��)

	Fvector		trace_offset;		// ����������� �����������
	float		dist;				// ��������� �����������

	u32			flags;				// ����������� �����

	float		damage;				// ���� ��� ������ �����
	float		dir_yaw;			// ���� ����������� ���������� ���� � ������� 
	float		dir_pitch;			//  - || -
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
	ANIM_ITEM_MAP			m_tAnims;			// ����� ��������
	MOTION_ITEM_MAP			m_tMotions;			// ����� ����������� EAction � SMotionItem
	TRANSITION_ANIM_VECTOR	m_tTransitions;		// ������ ��������� �� ����� �������� � ������
	ATTACK_ANIM				aa_all;				// ������ ����

	t_fx_index				default_fx_indexes;
	FX_MAP_STRING			fx_map_string;
	FX_MAP_U16				fx_map_u16;
	bool					map_converted;
};


//////////////////////////////////////////////////////////////////////////
class CMotionManager : public CSharedClass<_motion_shared> {

	REPLACED_ANIM			m_tReplacedAnims;	// �������� �������
	

	CAI_Biting				*pMonster;
	CJumping				*pJumping;
	IRender_Visual			*pVisual;

	// ������ � ��������������������
	SEQ_VECTOR				seq_states;
	SEQ_VECTOR_IT			seq_it;				
	bool					seq_playing;

	// ���������� �������� 
	EMotionAnim				cur_anim; 
	EMotionAnim				prev_anim; 

	// ����������� �������� '���� �� �����'
	TTime					time_start_stand;
	EAction					prev_action;

	// ������ � ���������� �����
	TTime					aa_time_started;		// ����� ������ ��������	
	TTime					aa_time_last_attack;	// ����� ���������� ��������� ����
	ATTACK_ANIM				aa_stack;				// ������ ���� ��� ������� ��������

	u32						spec_params;			// �������������� ���������

	bool					should_play_die_anim;	// ������ ���������� �������� ������

	bool					b_end_transition;		// ��������� �������� ����� ��������
	EMotionAnim				saved_anim;

	TTime					fx_time_last_play;

public:
	
	EAction					m_tAction;
	CMotionDef				*m_tpCurAnim;

public:

				CMotionManager			();
				~CMotionManager			();
	void		Init					(CAI_Biting	*pM);

	// �������� ����� �������� (��������� �� Monster::Load)
	void		AddAnim					(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed, EPState p_s);

	// -------------------------------------

	// �������� �������� �������� (A - Animation, S - Position)
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

	// ��������� ������� m_tAction
	void		ProcessAction			();
	// ����������� ������� �������� �� ������ �� SelectAnimation
	bool		PrepareAnimation		();

	void		CheckTransition			(EMotionAnim from, EMotionAnim to);

	void		OnAnimationEnd			();
	void		ShowDeath				();

	void		SetSpecParams			(u32 param) {spec_params |= param;}
	void		SetCurAnim				(EMotionAnim a) {cur_anim = a;}
	EMotionAnim	GetCurAnim				() {return  cur_anim;} 

	// ������ � ��������������������
	void		Seq_Add					(EMotionAnim a);
	void		Seq_Switch				();					// ������� � ��������� ���������, ���� �������� �� ������� - ���������

	// ������ � ���������� ����
	void		AA_PushAttackAnim		(SAttackAnimation AttackAnim);
	void		AA_PushAttackAnim		(EMotionAnim a, u32 i3, TTime from, TTime to, Fvector &ray, float dist, float damage, float yaw, float pitch, u32 flags = 0);
	bool		AA_CheckTime			(TTime cur_time, SAttackAnimation &anim); 
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}

	// FX's
	void		FX_LoadMap				(LPCSTR section);
	void		FX_ConvertMap			();
	void		FX_Play					(u16 bone, bool is_front, float amount);

		// �������� tpKinematics
	void		UpdateVisual			();

private:	

	// �������� MotionDef ��� ������������ ���� �������� 
	void		Load					(LPCSTR pmt_name, ANIM_VECTOR	*pMotionVect);
	// ������� � �������� MotionDef ��� ����� �������� (��������� �� Monster::Spawn, ����� ���������� Visual)
	void		LoadVisualData			();

	// ������ � ��������������������
	void		Seq_Init				();
	void		Seq_Finish				();

	// ������ � ���������� ����
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);

private:

	// �������������� �������
	EPState		GetState				(EMotionAnim a);

	void		FixBadState				();
	bool		IsMoving				();

	void		CheckReplacedAnim		();

	// sctipting
public:

	// ����� PrepareAnimation() � ��������� ��������
	void		ForceAnimSelect			();


};
