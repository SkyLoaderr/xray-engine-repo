#include "stdafx.h"
#include "Actor.h"
#include "ActorAnimation.h"
#include "..\xr_level_controller.h"
#include "hudmanager.h"
#include "UI.h"
#include "xr_weapon_list.h"

static const float y_spin_factor		= 0.4f;
static const float y_shoulder_factor	= 0.4f;
static const float y_head_factor		= 0.2f;
static const float p_spin_factor		= 0.2f;
static const float p_shoulder_factor	= 0.7f;
static const float p_head_factor		= 0.1f;

void __stdcall CActor::SpinCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	R_ASSERT(A);

	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_spin_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_spin_factor;

	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,0);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void __stdcall CActor::ShoulderCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	R_ASSERT(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_shoulder_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_shoulder_factor;

	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,0);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}
void __stdcall CActor::HeadCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	R_ASSERT(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_head_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_head_factor;
	
	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,0);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void CActor::SActorState::STorsoWpn::Create(CKinematics* K, LPCSTR base0, LPCSTR base1)
{
	char			buf[128];
	aim				= K->ID_Cycle(strconcat(buf,base0,"_torso_aim",		base1));
	holster			= K->ID_Cycle(strconcat(buf,base0,"_torso_holster",	base1));
	draw			= K->ID_Cycle(strconcat(buf,base0,"_torso_draw",	base1));
	reload			= K->ID_Cycle(strconcat(buf,base0,"_torso_reload",	base1));
	drop			= K->ID_Cycle(strconcat(buf,base0,"_torso_drop",	base1));
	attack			= K->ID_Cycle(strconcat(buf,base0,"_torso_attack",	base1));
}
void CActor::SActorState::SAnimState::Create(CKinematics* K, LPCSTR base0, LPCSTR base1)
{
	char			buf[128];
	legs_fwd		= K->ID_Cycle(strconcat(buf,base0,base1,"_fwd"));
	legs_back		= K->ID_Cycle(strconcat(buf,base0,base1,"_back"));
	legs_ls			= K->ID_Cycle(strconcat(buf,base0,base1,"_ls"));
	legs_rs			= K->ID_Cycle(strconcat(buf,base0,base1,"_rs"));
}

void CActor::SActorState::Create(CKinematics* K, LPCSTR base)
{
	char			buf[128];
	legs_idle		= K->ID_Cycle(strconcat(buf,base,"_idle"));
	legs_turn		= K->ID_Cycle(strconcat(buf,base,"_turn"));
	death			= K->ID_Cycle(strconcat(buf,base,"_death"));
	m_walk.Create	(K,base,"_walk");
	m_run.Create	(K,base,"_run");
	m_torso[0].Create(K,base,"_1");
	m_torso[1].Create(K,base,"_2");
	m_torso_idle	= K->ID_Cycle(strconcat(buf,base,"_torso_idle"));;

	jump_begin		= K->ID_Cycle(strconcat(buf,base,"_jump_begin"));
	jump_idle		= K->ID_Cycle(strconcat(buf,base,"_jump_idle"));
	landing[0]		= K->ID_Cycle(strconcat(buf,base,"_jump_end"));
	landing[1]		= K->ID_Cycle(strconcat(buf,base,"_jump_end_1"));
}

void CActor::g_SetAnimation( u32 mstate_rl )
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
		
		// Legs
		if		(mstate_rl&mcLanding)	M_legs	= ST->landing[0];
		else if (mstate_rl&mcLanding2)	M_legs	= ST->landing[1];
		else if (mstate_rl&mcTurn)		M_legs	= ST->legs_turn;
		else if (mstate_rl&mcFall)		M_legs	= ST->jump_idle;
		else if (mstate_rl&mcJump)		M_legs	= ST->jump_begin;
		else if (mstate_rl&mcFwd)		M_legs	= AS->legs_fwd;
		else if (mstate_rl&mcBack)		M_legs	= AS->legs_back;
		else if (mstate_rl&mcLStrafe)	M_legs	= AS->legs_ls;
		else if (mstate_rl&mcRStrafe)	M_legs	= AS->legs_rs;
		
		// Torso
		CWeapon* W = dynamic_cast<CWeapon*>(Weapons->ActiveWeapon());
		if (W){
			SActorState::STorsoWpn* TW	= &ST->m_torso[W->HandDependence()-1];
			if (!b_DropActivated&&!fis_zero(f_DropPower)){
				M_torso					= TW->drop;
				m_bAnimTorsoPlayed		= TRUE;
			}else{
				if (!m_bAnimTorsoPlayed){
					switch (W->STATE){
					case CWeapon::eIdle:	M_torso	= TW->aim;		break;
					case CWeapon::eFire:	M_torso	= TW->attack;	break;
					case CWeapon::eReload:	M_torso	= TW->reload;	break;
					case CWeapon::eShowing:	M_torso	= TW->draw;		break;
					case CWeapon::eHiding:	M_torso	= TW->holster;	break;
					}
				}
			}
		}

		if (!M_legs)					M_legs	= ST->legs_idle;
		if (!M_torso){				
			if (m_bAnimTorsoPlayed)		M_torso	= m_current_torso;
			else						M_torso = ST->m_torso_idle;
		}
		
		// есть анимация для всего - запустим / иначе запустим анимацию по частям
		if (m_current_torso!=M_torso){
			if (m_bAnimTorsoPlayed)		PKinematics	(pVisual)->PlayCycle(M_torso,TRUE,AnimTorsoPlayCallBack,this);
			else						PKinematics	(pVisual)->PlayCycle(M_torso);
			m_current_torso=M_torso;
		}
		if (m_current_legs!=M_legs){
			m_current_legs_blend = PKinematics(pVisual)->PlayCycle(M_legs);
			m_current_legs=M_legs;
		}
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
#ifdef _DEBUG
	CHUDManager* HUD	= (CHUDManager*)Level().HUD();
	string128 buf;
	strcpy(buf,"");
	if (mstate_rl&mcAccel)		strcat(buf,"Accel ");
	if (mstate_rl&mcCrouch)		strcat(buf,"Crouch ");
	if (mstate_rl&mcFwd)		strcat(buf,"Fwd ");
	if (mstate_rl&mcBack)		strcat(buf,"Back ");
	if (mstate_rl&mcLStrafe)	strcat(buf,"LStrafe ");
	if (mstate_rl&mcRStrafe)	strcat(buf,"RStrafe ");
	if (mstate_rl&mcJump)		strcat(buf,"Jump ");
	if (mstate_rl&mcFall)		strcat(buf,"Fall ");
	if (mstate_rl&mcTurn)		strcat(buf,"Turn ");
	if (mstate_rl&mcLanding)	strcat(buf,"Landing ");
	if (m_bJumpKeyPressed)		strcat(buf,"+Jumping ");
	HUD->pHUDFont->Color(0xffffffff);
	HUD->pHUDFont->OutSet(120,450);
	HUD->pHUDFont->OutNext("MSTATE:     [%s]",buf);
//	if (buf[0]) 
//		Msg("%s",buf);
	switch (Movement.Environment())
	{
	case CMovementControl::peOnGround:	strcpy(buf,"ground");			break;
	case CMovementControl::peInAir:		strcpy(buf,"air");				break;
	case CMovementControl::peAtWall:	strcpy(buf,"wall");				break;
	}
	HUD->pHUDFont->OutNext	(buf);
	HUD->pHUDFont->OutNext	("Accel     [%3.2f, %3.2f, %3.2f]",VPUSH(NET_SavedAccel));
	HUD->pHUDFont->OutNext	("V         [%3.2f, %3.2f, %3.2f]",VPUSH(Movement.GetVelocity()));
#endif
}

