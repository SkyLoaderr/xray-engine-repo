#pragma once

#include "ai_monster_shared_data.h"


class		CAI_Biting;
class		CJumping;

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
	TTime					aa_time_last_attack;	// ����� ���������� ��������� ����

	// -------------------------------------------------------------------------

	u32						spec_params;			// �������������� ���������

	bool					should_play_die_anim;	// ������ ���������� �������� ������

	bool					b_end_transition;		// ��������� �������� ����� ��������
	EMotionAnim				saved_anim;

	TTime					fx_time_last_play;

	bool					bad_motion_fixed;		// true, ���� ������ �������� ���������, �� ����� �� �����
	bool					b_forced_velocity;

	// -------------------------------------------------------------------------------------
	// Acceleration
	
	struct {
		bool					active;				
		bool					enable_braking;	// �� ������������ ��� ����������

		EAccelType				type;	
		
		float					calm;
		float					aggressive;

		VELOCITY_CHAIN_VEC		chain;
	} m_accel;

	// ---------------------------------------------------------------------------------------


	SStepInfo					step_info;

public:
	
	EAction					m_tAction;
	CMotionDef				*m_tpCurAnim;

public:

				CMotionManager			();
				~CMotionManager			();
	void		Init					(CAI_Biting	*pM);

	// �������� ����� �������� (��������� �� Monster::Load)
	void		AddAnim					(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s);
	void		AddAnim					(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s, LPCSTR fx_front, LPCSTR fx_back, LPCSTR fx_left, LPCSTR fx_right);

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

	// -------------------------------------- 	

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
	void		Seq_Finish				();
	EMotionAnim	Seq_CurAnim				() {return ((seq_playing) ? *seq_it : eAnimStandIdle );}
	bool		Seq_Active				() {return seq_playing;}


	// ������ � ���������� ����
	void		AA_Load					(LPCSTR section);
	bool		AA_TimeTest				(SAAParam &params);
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}
	void		AA_GetParams			(SAAParam &params, LPCSTR anim_name);

	// Steps
	void		STEPS_Load				(LPCSTR section, u8 legs_num);
	void		STEPS_Initialize		();
	void		STEPS_Update			(u8 legs_num);
	

	// FX's
	void		FX_Play					(EHitSide side, float amount);
	

	// �������� tpKinematics
	void		UpdateVisual			();


	bool		BadMotionFixed			() {return bad_motion_fixed;}

private:	

	// �������� MotionDef ��� ������������ ���� �������� 
	void		Load					(LPCSTR pmt_name, ANIM_VECTOR	*pMotionVect);
	// ������� � �������� MotionDef ��� ����� �������� (��������� �� Monster::Spawn, ����� ���������� Visual)
	void		LoadVisualData			();

	// ������ � ��������������������
	void		Seq_Init				();

	// ������ � ���������� ����
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);

	// �������������� �������
	EPState		GetState				(EMotionAnim a);

	void		FixBadState				();
	bool		IsMoving				();

	void		CheckReplacedAnim		();


public:

	// ����� PrepareAnimation() � ��������� ��������
	void		ForceAnimSelect			();
	
	void		ForceAngularSpeed		(float vel);
	float		GetAnimTime				(EMotionAnim anim, u32 index);
	float		GetAnimTime				(LPCSTR anim_name);
	float		GetAnimSpeed			(EMotionAnim anim);

	bool		IsStandCurAnim			();


	//////////////////////////////////////////////////////////////////////////

	void		Update					();

private:
	void		SelectAnimation			();
	void		SelectVelocities		();
	
	EAction		GetActionFromPath		();
	EAction		VelocityIndex2Action	(u32 velocity_index);
	
//////////////////////////////////////////////////////////////////////////
// DEBUG

private:
	LPCSTR		GetAnimationName		(EMotionAnim anim);
	LPCSTR		GetActionName			(EAction action);

// end DEBUG
//////////////////////////////////////////////////////////////////////////

public:
	//-------------------------------------------------------------------------------
	// Acceleration
		
		void	accel_init				();
		void	accel_load				(LPCSTR section);

		void	accel_activate			(EAccelType type);
	IC	void	accel_deactivate		() {m_accel.active = false;	m_accel.enable_braking = false;}
	IC	void	accel_set_braking		(bool val = true) {m_accel.enable_braking = val;}

		float	accel_get				(EAccelValue val = eAV_Accel);

	IC	bool	accel_active			(EAccelValue val = eAV_Accel) {return (val == eAV_Accel) ? m_accel.active : m_accel.enable_braking;}

		void	accel_chain_add			(EMotionAnim anim1, EMotionAnim anim2);
		bool	accel_chain_get			(float cur_speed, EMotionAnim target_anim, EMotionAnim &new_anim, float &a_speed);

		bool	accel_check_braking		(float before_interval);

	//-------------------------------------------------------------------------------


};

