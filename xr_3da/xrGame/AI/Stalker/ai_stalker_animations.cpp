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

//static void __stdcall vfPlayCallBack(CBlend* B)
//{
//	CAI_Stalker	*tpStalker			= (CAI_Stalker*)B->CallbackParam;
//	tpStalker->m_bActionStarted = false;
//}

void CAI_Stalker::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	PKinematics(pVisual)->PlayCycle(m_tAnims.A[1]->m_tTorso.A[0]->A[0]->A[0]);
	PKinematics(pVisual)->PlayCycle(m_tAnims.A[1]->m_tInPlace.A[0]);
//	CKinematics	*tpVisualObject		=	PKinematics(pVisual);
//	CMotionDef	*tpLegsAnimation	=	0;
//	CMotionDef	*tpTorsoAnimation	=	0;
//	CMotionDef	*tpGlobalAnimation	=	0;
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
//		switch (eCurrentState) {
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
//			if (eCurrentState == aiStalkerWaitForAnimation)
//				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation,TRUE,vfPlayCallBack,this);
//			else
//				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
//		}
//	}
}
