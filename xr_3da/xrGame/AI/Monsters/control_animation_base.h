#pragma once

#include "ai_monster_shared_data.h"
#include "../../../SkeletonAnimated.h"
#include "control_combase.h"
#include "anim_triple.h"
#include "control_jump.h"

class		CJumping;


struct SEventVelocityBounce : public ControlCom::IEventData {
	float	m_ratio;

	IC		SEventVelocityBounce(float ratio) : m_ratio(ratio) {}

};


//////////////////////////////////////////////////////////////////////////
class CControlAnimationBase :	public CSharedClass<_motion_shared, CLASS_ID>, 
								public CControl_ComBase {
		typedef CControl_ComBase inherited;


		REPLACED_ANIM			m_tReplacedAnims;	// �������� �������

		CJumping				*pJumping;

		// ������ � ��������������������
		SEQ_VECTOR				seq_states;
		SEQ_VECTOR_IT			seq_it;				
		bool					seq_playing;

		// ���������� �������� 
		EMotionAnim				prev_motion; 

		// ����������� �������� '���� �� �����'
		TTime					time_start_stand;
		EAction					prev_action;

		// ������ � ���������� �����
		TTime					aa_time_last_attack;	// ����� ���������� ��������� ����

		// -------------------------------------------------------------------------

		u32						spec_params;			// �������������� ���������

		TTime					fx_time_last_play;

		bool					bad_motion_fixed;		// true, ���� ������ �������� ���������, �� ����� �� �����

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

		EMotionAnim					spec_anim; 

private:

	ANIM_TO_MOTION_MAP			m_anim_motion_map;

public:

	EAction					m_tAction;

	float					m_prev_character_velocity;

public:

	// Control_ComBase interface
	virtual void		reinit			();
	virtual void		on_event		(ControlCom::EEventType, ControlCom::IEventData*);	
	virtual void		on_start_control(ControlCom::EContolType type);
	virtual void		on_stop_control	(ControlCom::EContolType type);
	virtual void		update_frame	();


			void		ScheduledInit	();

	// �������� ����� �������� (��������� �� Monster::Load)
	void		AddAnim					(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s);
	void		AddAnim					(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s, LPCSTR fx_front, LPCSTR fx_back, LPCSTR fx_left, LPCSTR fx_right);

	// -------------------------------------

	// �������� �������� �������� (A - Animation, S - Position)
	void		AddTransition			(EMotionAnim from,	EMotionAnim to, EMotionAnim trans, bool chain, bool skip_aggressive = false);
	void		AddTransition			(EMotionAnim from,	EPState to,		EMotionAnim trans, bool chain, bool skip_aggressive = false);
	void		AddTransition			(EPState from,		EMotionAnim to, EMotionAnim trans, bool chain, bool skip_aggressive = false);
	void		AddTransition			(EPState from,		EPState to,		EMotionAnim trans, bool chain, bool skip_aggressive = false);

	// -------------------------------------

	void		LinkAction				(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle);
	void		LinkAction				(EAction act, EMotionAnim pmt_motion);

	// -------------------------------------

	void		AddReplacedAnim			(bool *b_flag, EMotionAnim pmt_cur_anim, EMotionAnim pmt_new_anim);

	// -------------------------------------
	void		CheckTransition			(EMotionAnim from, EMotionAnim to);

	void		ShowDeath				();

	void		SetSpecParams			(u32 param) {spec_params |= param;}
	void		SetCurAnim				(EMotionAnim a) {cur_anim_info().motion = a;}
	EMotionAnim	GetCurAnim				() {return  cur_anim_info().motion;} 

	// ������ � ��������������������
	void		Seq_Init				();
	void		Seq_Add					(EMotionAnim a);
	void		Seq_Switch				();					// ������� � ��������� ���������, ���� �������� �� ������� - ���������
	bool		Seq_Active				() {return false;}

	//-------------------------------------------------------------------------------
	// Triple Animation
	void		TA_Activate				(const SAnimationTripleData &data);
	void		TA_PointBreak			();
	void		TA_FillData				(SAnimationTripleData &data, LPCSTR s1, LPCSTR s2, LPCSTR s3, bool execute_once, bool skip_prep);
	bool		TA_IsActive				();
	void		TA_Deactivate			();

	void		AnimSpec_Play			(EMotionAnim a);
	bool		AnimSpec_Active			() {return (spec_anim != eAnimUndefined);}
	void		AnimSpec_Stop			() {spec_anim = eAnimUndefined;}

	// ������ � ���������� ����
	void		AA_Load					(LPCSTR section);
	bool		AA_TimeTest				(SAAParam &params);
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}
	void		AA_GetParams			(SAAParam &params, LPCSTR anim_name);

	// FX's
	void		FX_Play					(EHitSide side, float amount);

	bool		BadMotionFixed			() {return bad_motion_fixed;}

	MotionID	get_motion_id			(EMotionAnim a, u32 index = u32(-1));

private:	

	void		UpdateAnimCount			();

	// ������ � ���������� ����
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);

	// �������������� �������
	EPState		GetState				(EMotionAnim a);

	void		CheckReplacedAnim		();

	CMotionDef	*get_motion_def			(ANIM_ITEM_MAP_IT &it, u32 index);

public:

	float		GetCurAnimTime			();
	float		GetAnimSpeed			(EMotionAnim anim);

	bool		IsStandCurAnim			();

	void		ValidateAnimation		();

	//////////////////////////////////////////////////////////////////////////

private:
	void		update					();

private:
	void		SelectAnimation			();
	void		SelectVelocities		();

	EAction		GetActionFromPath		();
	EAction		VelocityIndex2Action	(u32 velocity_index);


	void		stop_now				();

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
	bool	accel_chain_test		();

	bool	accel_check_braking		(float before_interval);

	// -----------------------------------------------------
	// Jumps

	void	ActivateJump			();
	void	DeactivateJump			();
	bool	JumpActive				();

	// Critical Action

	bool	IsCriticalAction		();

	void	CheckVelocityBounce		();

	// Other
	void	SetTurnAnimation		();

	// MotionDef to animation name translation
	void		AddAnimTranslation		(const MotionID &motion, LPCSTR str);
	shared_str	GetAnimTranslation		(const MotionID &motion);
public:

	// ���������� � ������� ��������
	SCurrentAnimationInfo	m_cur_anim;
	SCurrentAnimationInfo	&cur_anim_info() {return m_cur_anim;}

	void					select_animation	();
	void					set_animation_speed	();

	void					jump				(CObject *obj, const SControlJumpData &ta);
	void					jump				(const SControlJumpData &ta);
	void					load_jump_data		(SControlJumpData &data, LPCSTR s1, LPCSTR s2, LPCSTR s3, u32 vel_mask);
};

