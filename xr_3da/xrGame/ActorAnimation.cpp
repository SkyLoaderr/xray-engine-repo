#include "stdafx.h"
#include "Actor.h"
#include "ActorAnimation.h"
#include "..\xr_level_controller.h"

static const float y_spin_factor		= 0.3f;
static const float y_shoulder_factor	= 0.6f;
static const float y_head_factor		= 0.1f;
static const float p_spin_factor		= 0.3f;
static const float p_shoulder_factor	= 0.6f;
static const float p_head_factor		= 0.1f;

#include "hudmanager.h"

float angle_normalize2(float a)
{
	float angle = angle_normalize(a);
	if (angle>PI) angle-=PI_MUL_2;
	return angle;
}

void __stdcall CActor::SpinCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	R_ASSERT(A);

	Fmatrix				spin;
	float				bone_yaw	= angle_normalize2(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_spin_factor;
	float				bone_pitch	= A->r_torso.pitch*p_spin_factor;
	// clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
	spin.setXYZ			(bone_yaw,bone_pitch,0);
	B->mTransform.mulB_43(spin);

	CHUDManager* HUD	= (CHUDManager*)Level().HUD();
	string128 buf;
	HUD->pHUDFont->Color(0xffffffff);
	HUD->pHUDFont->OutSet(120,520);
	HUD->pHUDFont->OutNext("Bone Yaw:      [%3.2f]",bone_yaw);
}
void __stdcall CActor::ShoulderCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	R_ASSERT(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize2(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_shoulder_factor;
	float				bone_pitch	= A->r_torso.pitch*p_shoulder_factor;
	// clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
	spin.setXYZ			(bone_yaw,bone_pitch,0);
	B->mTransform.mulB_43(spin);
}
void __stdcall CActor::HeadCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	R_ASSERT(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize2(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_head_factor;
	float				bone_pitch	= A->r_torso.pitch*p_head_factor;
	// clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
	spin.setXYZ			(bone_yaw,bone_pitch,0);
	B->mTransform.mulB_43(spin);
}

void CActor::SActorState::SAnimState::Create(CKinematics* K, LPCSTR base0, LPCSTR base1){
	char			buf[128];
	legs_fwd		= K->ID_Cycle(strconcat(buf,base0,base1,"_fwd"));
	legs_back		= K->ID_Cycle(strconcat(buf,base0,base1,"_back"));
	legs_ls			= K->ID_Cycle(strconcat(buf,base0,base1,"_ls"));
	legs_rs			= K->ID_Cycle(strconcat(buf,base0,base1,"_rs"));
}

void CActor::SActorState::Create(CKinematics* K, LPCSTR base){
	char			buf[128];
	idle			= K->ID_Cycle(strconcat(buf,base,"_idle"));
	torso_aim		= K->ID_Cycle(strconcat(buf,base,"_torso_aim"));
	legs_turn		= K->ID_Cycle(strconcat(buf,base,"_turn"));
	death			= K->ID_Cycle(strconcat(buf,base,"_death"));
	m_walk.Create	(K,base,"_walk");
	m_run.Create	(K,base,"_run");

	jump_begin		= K->ID_Cycle(strconcat(buf,base,"_jump_begin"));
	jump_idle		= K->ID_Cycle(strconcat(buf,base,"_jump_idle"));
}

void CActor::g_SetAnimation( DWORD mstate_rl )
{
	if (g_Alive())
	{
		SActorState* ST = 0;
		SActorState::SAnimState*		AS = 0;
		
		if (mstate_rl&mcCrouch)			ST = &m_crouch;
		else							ST = &m_normal;

		if (isAccelerated(mstate_rl))	AS = &ST->m_run;
		else							AS = &ST->m_walk;
		
		// анимации
		CMotionDef* M_legs	= 0;
		CMotionDef* M_torso	= 0;
		CMotionDef* M_all	= 0;
		
		if (mstate_rl&mcFwd)			M_legs = AS->legs_fwd;
		else if (mstate_rl&mcBack)		M_legs = AS->legs_back;
		else if (mstate_rl&mcLStrafe)	M_legs = AS->legs_ls;
		else if (mstate_rl&mcRStrafe)	M_legs = AS->legs_rs;
		else if (mstate_rl&mcJump){
			if ((m_current_legs==ST->jump_begin)&&(!m_current_legs_blend->playing)){
				M_legs = ST->jump_idle;
			}else if (m_current_legs==ST->jump_idle){
				M_legs = ST->jump_idle;
			}else M_legs = ST->jump_begin;
		}
		else if (mstate_rl&mcTurn)		M_legs = ST->legs_turn;
		
		// на ноги есть анимация - установим еще и на торс / иначе Idle для всего
		if (M_legs)						M_torso = ST->torso_aim;
		else							M_all = ST->idle;
		
		// есть анимация для всего - запустим / иначе запустим анимацию по частям
		if (M_all){
			if(m_current_torso!=M_all){ 
				PKinematics	(pVisual)->PlayCycle(M_all);
				m_current_torso = M_all;
				m_current_legs	= M_all;
			}
		}else{
			if (m_current_torso!=M_torso){
				PKinematics	(pVisual)->PlayCycle(M_torso);
				m_current_torso=M_torso;
			}
			if (m_current_legs!=M_legs){
				m_current_legs_blend = PKinematics(pVisual)->PlayCycle(M_legs);
				m_current_legs=M_legs;
			}
		}
		
		// end of turn
		/*		if (M_legs == AS->legs_turn){
		if (!m_current_legs_blend->playing)
		mstate_real &= ~mcTurn;
		}
		*/
	}else{
		if (m_current_legs||m_current_torso){
			SActorState* ST = 0;
			if (mstate_rl&mcCrouch)	ST = &m_crouch;
			else					ST = &m_normal;
			mstate_real			= 0;
			m_current_legs		= 0;
			m_current_torso		= 0;
			PKinematics	(pVisual)->PlayCycle(ST->death);
		}
	}
}

