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

#define		DEFAULT_ANIM	eAnimStandIdle

enum EMotionAnim {
	eAnimStandIdle = 0,
	eAnimStandTurnLeft,
	eAnimStandTurnRight,
	eAnimStandDamaged,

	eAnimSitIdle,
	eAnimLieIdle,

	eAnimSitToSleep,
	eAnimStandSitDown,
	eAnimStandLieDown,
	eAnimLieStandUp,
	eAnimSitStandUp,
	eAnimStandLieDownEat,
	eAnimSitLieDown,
	eAnimLieSitUp,

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
};


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
	ACT_LOOK_AROUND
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

struct STransition {
	bool			ps_from_used;
	EMotionAnim		anim_from;
	EPState			state_from;
	
	bool			ps_target_used;
	EMotionAnim		anim_target;
	EPState			state_target;

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

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// ������ ����?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// ����������� �� �����

#define FORCE_ANIMATION_SELECT() {				\
	m_tpCurAnim = 0;							\
	pMonster->SelectAnimation(pMonster->Direction(),pMonster->Direction(),0); \
}

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



// Motion and animation management
class CMotionManager {

	DEFINE_MAP		(EMotionAnim,		SAnimItem,				ANIM_ITEM_MAP,		ANIM_ITEM_MAP_IT);
	DEFINE_VECTOR	(STransition,		TRANSITION_ANIM_VECTOR, TRANSITION_ANIM_VECTOR_IT);
	DEFINE_MAP		(EAction,			SMotionItem,			MOTION_ITEM_MAP,	MOTION_ITEM_MAP_IT);
	DEFINE_VECTOR	(EMotionAnim,		SEQ_VECTOR,				SEQ_VECTOR_IT);
	DEFINE_VECTOR	(SAttackAnimation,	ATTACK_ANIM,			ATTACK_ANIM_IT);

	ANIM_ITEM_MAP			m_tAnims;			// ����� ��������
	TRANSITION_ANIM_VECTOR	m_tTransitions;		// ������ ��������� �� ����� �������� � ������
	MOTION_ITEM_MAP			m_tMotions;			// ����� ����������� EAction � SMotionItem
	
	CAI_Biting				*pMonster;
	CKinematics				*tpKinematics;

	// ������ � ��������������������
	SEQ_VECTOR				seq_states;
	SEQ_VECTOR_IT			seq_it;				// �������� (������� �������)
	bool					seq_playing;

	EMotionAnim				cur_anim; 
	EMotionAnim				prev_anim; 

	// ������ � ���������� �����
	TTime					aa_time_started;		// ����� ������ ��������	
	TTime					aa_time_last_attack;	// ����� ���������� ��������� ����
	ATTACK_ANIM				aa_all;					// ������ ���� ����
	ATTACK_ANIM				aa_stack;				// ������ ���� ��� ������� ��������

	u32						spec_params;			// �������������� ���������

public:
	EAction					m_tAction;
	CMotionDef				*m_tpCurAnim;

public:
			
				CMotionManager			();
	
	void		Init					(CAI_Biting	*pM, CKinematics *tpKin);
	void		Destroy					();
	
	// �������� ����� ��������, ��������� 
	void		AddAnim					(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed, EPState p_s);
	
	void		AddTransition			(EMotionAnim from,	EMotionAnim to, EMotionAnim trans, bool chain);
	void		AddTransition			(EMotionAnim from,	EPState to,		EMotionAnim trans, bool chain);
	void		AddTransition			(EPState from,		EMotionAnim to, EMotionAnim trans, bool chain);
	void		AddTransition			(EPState from,		EPState to,		EMotionAnim trans, bool chain);
	
	void		LinkAction				(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle);
	void		LinkAction				(EAction act, EMotionAnim pmt_motion);
	
	bool		CheckTransition			(EMotionAnim from, EMotionAnim to);
	void		ApplyParams				();

	// ��������� ������� m_tAction
	void		ProcessAction			();
	// ����������� ������� �������� �� ������ �� SelectAnimation
	bool		PrepareAnimation		();

	void		OnAnimationEnd			();
	void		ShowDeath				();

	void		SetSpecParams			(u32 param) {spec_params |= param;}
	void		SetCurAnim				(EMotionAnim a) {cur_anim = a;}

	// ������ � ��������������������
	void		Seq_Add					(EMotionAnim a);
	void		Seq_Switch				();					// ������� � ��������� ���������, ���� �������� �� ������� - ���������

private:	
	// �������� ��������
	void		Load					(LPCTSTR pmt_name, ANIM_VECTOR	*pMotionVect);

	// ������ � ��������������������
	void		Seq_Init				();
	void		Seq_Finish				();

	// ������ � ���������� ����
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);

	bool		CheckSpecParams			();	

public:
	void		AA_PushAttackAnim		(SAttackAnimation AttackAnim);
	void		AA_PushAttackAnim		(EMotionAnim a, u32 i3, TTime from, TTime to, Fvector &ray, 
										 float dist, float damage, float yaw, float pitch, u32 flags = 0);
	bool		AA_CheckTime			(TTime cur_time, SAttackAnimation &anim); 
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}

private:

	EPState		GetState				(EMotionAnim a);
};


