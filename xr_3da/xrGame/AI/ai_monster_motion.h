#pragma once

#include "ai_monster_shared_data.h"


class		CAI_Biting;
class		CJumping;

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
	bool					b_forced_velocity;

	VELOCITY_CHAIN_VEC		velocity_chain;

public:
	
	EAction					m_tAction;
	CMotionDef				*m_tpCurAnim;
	bool					b_ignore_path_velocity_check;

public:

				CMotionManager			();
				~CMotionManager			();
	void		Init					(CAI_Biting	*pM);

	// создание карты анимаций (выполнять на Monster::Load)
	void		AddAnim					(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s);
	void		AddAnim					(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s, LPCSTR fx_front, LPCSTR fx_back, LPCSTR fx_left, LPCSTR fx_right);

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
	void		FinalizeProcessing		();

	// -------------------------------------- 	

	void		ProcessActionEx			();


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
	bool		Seq_Active				() {return seq_playing;}


	// работа с анимациями атак
	void		AA_PushAttackAnim		(SAttackAnimation AttackAnim);
	void		AA_PushAttackAnim		(EMotionAnim a, u32 i3, TTime from, TTime to, const Fvector &ray_from, const Fvector &ray_to, float damage, Fvector &dir, u32 flags = 0);
	bool		AA_CheckTime			(TTime cur_time, SAttackAnimation &anim); 
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}
	void		AA_PushAttackAnimTest	(EMotionAnim a, u32 i3, TTime from, TTime to, float y1, float y2, float p1, float p2, float dist, float damage, Fvector &dir, u32 flags = 0);
	
	// FX's
	void		FX_LoadMap				(LPCSTR section);
	void		FX_ConvertMap			();
	void		FX_Play					(u16 bone, bool is_front, float amount);
	void		FX_Play					(EHitSide side, float amount);
	

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


public:

	// Вызов PrepareAnimation() и установка анимации
	void		ForceAnimSelect			();
	
	void		ForceAngularSpeed		(float vel);
	float		GetAnimTime				(EMotionAnim anim, u32 index);
	float		GetAnimSpeed			(EMotionAnim anim);

	void		VelocityChain_Add		(EMotionAnim anim1, EMotionAnim anim2);
	bool		VelocityChain_GetAnim	(float cur_speed, EMotionAnim target_anim, EMotionAnim &new_anim, float &a_speed);

	bool		IsStandCurAnim			();


	//////////////////////////////////////////////////////////////////////////

	void		Update					();

private:
	void		RetrieveAnimation		();
	EAction		GetActionFromPath		();

	EAction		VelocityIndex2Action	(u32 velocity_index);
	
	void		ApplyAnimation			();

//////////////////////////////////////////////////////////////////////////
// DEBUG

private:
	LPCSTR		GetAnimationName		(EMotionAnim anim);
	LPCSTR		GetActionName			(EAction action);

// end DEBUG
//////////////////////////////////////////////////////////////////////////

};

