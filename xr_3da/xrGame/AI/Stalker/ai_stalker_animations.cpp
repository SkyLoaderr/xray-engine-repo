////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "ai_stalker_animations.h"

static const float y_spin_factor		= 0.4f;
static const float y_shoulder_factor	= 0.4f;
static const float y_head_factor		= 0.2f;
static const float p_spin_factor		= 0.2f;
static const float p_shoulder_factor	= 0.7f;
static const float p_head_factor		= 0.1f;

LPCSTR caStateNames			[dwStateCount] = {
	"cr_",
	"norm_",
};

LPCSTR caWeaponNames		[dwWeaponCount] = {
	"0_",
	"1_",
	"2_",
	"3_",
};

LPCSTR caWeaponActionNames	[dwWeaponActionCount] = {
	"aim_",
	"attack_",
	"draw_",
	"drop_",
	"holster_",
	"reload_",
	"pick_",
};

LPCSTR caMovementNames		[dwMovementCount] = {
	"run_",
	"walk_",
};

LPCSTR caMovementActionNames[dwMovementActionCount] = {
	"fwd_",
	"back_",
	"ls_",
	"rs_",
};

LPCSTR caInPlaceNames		[dwInPlaceCount] = {
	"idle",
	"turn",
	"jump_begin",
	"jump_idle",
	"jump_end",
	"jump_end_1",
};

LPCSTR caGlobalNames		[dwGlobalCount] = {
	"death_",
	"damage_",
};


#define TORSO_ANGLE_DELTA		(PI/30.f)

void CAI_Stalker::vfAssignBones(CInifile *ini, const char *section)
{
	int head_bone		= PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_head"));
	PKinematics(pVisual)->LL_GetInstance(head_bone).set_callback(HeadCallback,this);
	
	int shoulder_bone	= PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_shoulder"));
	PKinematics(pVisual)->LL_GetInstance(shoulder_bone).set_callback(ShoulderCallback,this);
	
	int spin_bone		= PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_spin"));
	PKinematics(pVisual)->LL_GetInstance(spin_bone).set_callback(SpinCallback,this);
}

void __stdcall CAI_Stalker::HeadCallback(CBoneInstance* B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	spin.setXYZ				(angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch)*p_head_factor, angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw)*y_head_factor), 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::ShoulderCallback(CBoneInstance* B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	spin.setXYZ				(angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch)*p_shoulder_factor, angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw)*y_shoulder_factor), 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::SpinCallback(CBoneInstance* B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	spin.setXYZ				(angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch)*p_spin_factor, angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw)*y_spin_factor), 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::LegsSpinCallback(CBoneInstance* B)
{
//	CAI_Stalker*		A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	
//	Fmatrix				spin;
//	float				bone_yaw	= A->r_torso_current.yaw - A->r_model_yaw - A->r_model_yaw_delta;
//	float				bone_pitch	= A->r_torso_current.pitch;
//	clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
//	spin.setXYZ			(bone_yaw,bone_pitch,0);
//	B->mTransform.mulB_43(spin);
}

void CAI_Stalker::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics	&tVisualObject		=	*(PKinematics(pVisual));
	CMotionDef	*tpGlobalAnimation	=	0;
	CMotionDef	*tpTorsoAnimation	=	0;
	CMotionDef	*tpLegsAnimation	=	0;

	if (g_Health() <= 0) {
		tpGlobalAnimation = m_tAnims.A[1]->m_tGlobal.A[0]->A[0];
	}
	else {
		if (!tpTorsoAnimation)
			if (Weapons->ActiveWeapon())
				if (m_eCurrentState == eStalkerStateRecharge) {
					switch (Weapons->ActiveWeaponID()) {
						case 0 : {
							tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[3]->A[5]->A[0];
							break;
						}
						case 1 : {
							tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[1]->A[5]->A[0];
							break;
						}
						case 2 : {
							tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[2]->A[5]->A[0];
							break;
						}
					}
				}
				else {
					switch (Weapons->ActiveWeaponID()) {
						case 0 : {
							tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[3]->A[0]->A[0];
							break;
						}
						case 1 : {
							tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[1]->A[0]->A[0];
							break;
						}
						case 2 : {
							tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[2]->A[0]->A[0];
							break;
						}
					}
				}

		if (!tpLegsAnimation)
			tpLegsAnimation = m_tAnims.A[1]->m_tInPlace.A[0];
	}
	
	if ((tpGlobalAnimation) && (m_tpCurrentGlobalAnimation != tpGlobalAnimation)) {
		tVisualObject.PlayCycle(m_tpCurrentGlobalAnimation = tpGlobalAnimation);
	}
	else {
		if ((tpTorsoAnimation) && (m_tpCurrentTorsoAnimation != tpTorsoAnimation))
			tVisualObject.PlayCycle(m_tpCurrentTorsoAnimation = tpTorsoAnimation);
		if ((tpLegsAnimation) && (m_tpCurrentLegsAnimation != tpLegsAnimation))
			tVisualObject.PlayCycle(m_tpCurrentLegsAnimation = tpLegsAnimation);
	}
//
//	if (fHealth <= 0) {
//		switch (m_cBodyState) {
//			case BODY_STATE_STAND : {
//				for (int i=0 ;i<5; i++)
//					if (tStalkerAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
//						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
//						break;
//					}
//				if (!tpGlobalAnimation)
//					tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,5)];
//				break;
//			}
//			case BODY_STATE_CROUCH : {
//				for (int i=0 ;i<5; i++)
//					if (tStalkerAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
//						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
//						break;
//					}
//				if (!tpGlobalAnimation)
//					tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,5)];
//				break;
//			}
//			case BODY_STATE_LIE : {
//				tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpDeath;
//				break;
//			}
//		}
//	}
//	else
//		switch (m_eCurrentState) {
//			case aiStalkerWaitForAnimation : {
//				tpGlobalAnimation = m_tpAnimationBeingWaited;
//				tpTorsoAnimation = tpLegsAnimation = 0;
//				break;
//			}
//			case aiStalkerAttackAloneFireFire : 
//			case aiStalkerAttackGroupFireFire : {
//				if (m_bFiring)
//					switch (m_cBodyState) {
//						case BODY_STATE_STAND : {
//							tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpAttack;
//							break;
//						}
//						case BODY_STATE_CROUCH : {
//							tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpAttack;
//							break;
//						}
//						case BODY_STATE_LIE : {
//							tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpAttack;
//							break;
//						}
//					}
//				else
//					if (speed < EPS_L)
//						switch (m_cBodyState) {
//							case BODY_STATE_STAND : {
//								tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpAim;
//								break;
//							}
//							case BODY_STATE_CROUCH : {
//								tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpAim;
//								break;
//							}
//							case BODY_STATE_LIE : {
//								tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpIdle;
//								break;
//							}
//						}
//					else
//						tpGlobalAnimation = m_tpaMovementAnimations[m_cBodyState][m_cMovementType];
//				tpTorsoAnimation = tpLegsAnimation = 0;
//				break;
//			}
//			case aiStalkerRecharge : {
//				switch (m_cBodyState) {
//					case BODY_STATE_STAND : {
//						tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpReload;
//						break;
//					}
//					case BODY_STATE_CROUCH : {
//						tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpReload;
//						break;
//					}
//					case BODY_STATE_LIE : {
//						tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpReload;
//						break;
//					}
//				}
//				break;
//			}
//			default : {
//				if (speed < .1f) {
//					// turning around || standing idle
//					if (_abs(r_torso_target.yaw - r_torso_current.yaw) <= PI)
//						if (_abs(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
//							if (r_torso_target.yaw - r_torso_current.yaw >= 0)
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpTurnRight;
//										break;
//									}
//								}
//							else
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpTurnLeft;
//										break;
//									}
//								}
//						else
//							switch (m_cBodyState) {
//								case BODY_STATE_STAND : {
//									tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpaIdle[1];
//									break;
//								}
//								case BODY_STATE_CROUCH : {
//									tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpIdle;
//									break;
//								}
//								case BODY_STATE_LIE : {
//									tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpIdle;
//									break;
//								}
//							}
//					else
//						if (PI_MUL_2 - _abs(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
//							if (r_torso_target.yaw > r_torso_current.yaw)
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpTurnLeft;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpTurnLeft;
//										break;
//									}
//								}
//							else
//								switch (m_cBodyState) {
//									case BODY_STATE_STAND : {
//										tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_CROUCH : {
//										tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpTurnRight;
//										break;
//									}
//									case BODY_STATE_LIE : {
//										tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpTurnRight;
//										break;
//									}
//								}
//						else {
//							switch (m_cBodyState) {
//								case BODY_STATE_STAND : {
//									tpGlobalAnimation = tStalkerAnimations.tNormal.tGlobal.tpaIdle[1];
//									break;
//								}
//								case BODY_STATE_CROUCH : {
//									tpGlobalAnimation = tStalkerAnimations.tCrouch.tGlobal.tpIdle;
//									break;
//								}
//								case BODY_STATE_LIE : {
//									tpGlobalAnimation = tStalkerAnimations.tLie.tGlobal.tpIdle;
//									break;
//								}
//							}
//							break;
//						}
//					// torso
//				}
//				else {
//					//Msg("moving...");
//					tpGlobalAnimation = m_tpaMovementAnimations[m_cBodyState][m_cMovementType];
//					if (!tpGlobalAnimation) {
//						tpGlobalAnimation = tpGlobalAnimation;
//					}
//					//torso
//				}
//			}
//		}
//	
//	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
//		//Msg("restarting animation..."); 
//		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
//		if (tpGlobalAnimation) {
//			if (m_eCurrentState == aiStalkerWaitForAnimation)
//				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation,TRUE,vfPlayCallBack,this);
//			else
//				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
//		}
//	}
}
