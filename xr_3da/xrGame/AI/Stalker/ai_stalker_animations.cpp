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

LPCSTR caBodyPartNames		[dwBodyPartCount] = {
	"torso_",
	"",
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
	"begin",
	"idle",
	"end",
	"end_1",
};

LPCSTR caGlobalNames		[dwGlobalCount] = {
	"death",
	"damage",
};


#define TORSO_ANGLE_DELTA		(PI/30.f)

// animations
void CAI_Stalker::vfLoadAnimations()
{
//	CKinematics* tpVisualObject = PKinematics(pVisual);
//	
//	// loading normal animations
//	tStalkerAnimations.tNormal.tGlobal.tpaDeath[0] = tpVisualObject->ID_Cycle("norm_death_0");
//	tStalkerAnimations.tNormal.tGlobal.tpaDeath[1] = tpVisualObject->ID_Cycle("norm_death_1");
//	tStalkerAnimations.tNormal.tGlobal.tpaDeath[2] = tpVisualObject->ID_Cycle("norm_death_2");
//	tStalkerAnimations.tNormal.tGlobal.tpaDeath[3] = tpVisualObject->ID_Cycle("norm_death_3");
//	tStalkerAnimations.tNormal.tGlobal.tpaDeath[4] = tpVisualObject->ID_Cycle("norm_death_4");
//	
//	tStalkerAnimations.tNormal.tGlobal.tpaRunForward[0] = tpVisualObject->ID_Cycle("norm_run_fwd_0");
//	tStalkerAnimations.tNormal.tGlobal.tpaRunForward[1] = tpVisualObject->ID_Cycle("norm_run_fwd_1");
//	tStalkerAnimations.tNormal.tGlobal.tpaRunForward[2] = tpVisualObject->ID_Cycle("norm_run_fwd_2");
//	tStalkerAnimations.tNormal.tGlobal.tpaRunForward[3] = tpVisualObject->ID_Cycle("norm_run_fwd_3");
//
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[0] = tpVisualObject->ID_Cycle("norm_walk_fwd_0");
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[1] = tpVisualObject->ID_Cycle("norm_walk_fwd_1");
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[2] = tpVisualObject->ID_Cycle("norm_walk_fwd_2");
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[3] = tpVisualObject->ID_Cycle("norm_walk_fwd_3");
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[4] = tpVisualObject->ID_Cycle("norm_walk_fwd_4");
//	
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkBack[0] = tpVisualObject->ID_Cycle("norm_walk_back_0");
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkBack[1] = tpVisualObject->ID_Cycle("norm_walk_back_1");
//	tStalkerAnimations.tNormal.tGlobal.tpaWalkBack[2] = tpVisualObject->ID_Cycle("norm_walk_back_2");
//	
//	tStalkerAnimations.tNormal.tGlobal.tpaLieDown[0] = tpVisualObject->ID_Cycle("norm_lie_down_0");
//	tStalkerAnimations.tNormal.tGlobal.tpaLieDown[1] = tpVisualObject->ID_Cycle("norm_lie_down_1");
//	tStalkerAnimations.tNormal.tGlobal.tpaLieDown[2] = tpVisualObject->ID_Cycle("norm_lie_down_2");
//
//	tStalkerAnimations.tNormal.tGlobal.tpaIdle[0] = tpVisualObject->ID_Cycle("norm_idle");
//	tStalkerAnimations.tNormal.tGlobal.tpaIdle[1] = tpVisualObject->ID_Cycle("norm_idle_1");
//
//	tStalkerAnimations.tNormal.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle("norm_walk_ls");
//	tStalkerAnimations.tNormal.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle("norm_walk_rs");
//	
//	tStalkerAnimations.tNormal.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle("norm_turn_ls");
//	tStalkerAnimations.tNormal.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle("norm_turn_rs");
//	
//	tStalkerAnimations.tNormal.tGlobal.tpPointSign = tpVisualObject->ID_Cycle("norm_sign_2");
//
//	tStalkerAnimations.tNormal.tGlobal.tpAim = tpVisualObject->ID_Cycle("norm_aim");
//	tStalkerAnimations.tNormal.tGlobal.tpAttack = tpVisualObject->ID_Cycle("norm_attack");
//	tStalkerAnimations.tNormal.tGlobal.tpReload = tpVisualObject->ID_Cycle("norm_reload");
//
//	tStalkerAnimations.tNormal.tTorso.tpDamageLeft = tpVisualObject->ID_FX("norm_damage_ls");
//	tStalkerAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_FX("norm_damage_rs");
//	tStalkerAnimations.tNormal.tTorso.tpRaiseHandSign = tpVisualObject->ID_Cycle("norm_sign_0");
//	tStalkerAnimations.tNormal.tTorso.tpGoAheadSign = tpVisualObject->ID_Cycle("norm_sign_1");
//	
//	tStalkerAnimations.tNormal.tLegs.tpJumpBegin = tpVisualObject->ID_Cycle("norm_jump_begin");
//	tStalkerAnimations.tNormal.tLegs.tpJumpIdle = tpVisualObject->ID_Cycle("norm_jump_idle");
//	
//	// loading crouch animations
//	tStalkerAnimations.tCrouch.tGlobal.tpaWalkForward[0] = tpVisualObject->ID_Cycle("cr_walk_fwd_0");
//	tStalkerAnimations.tCrouch.tGlobal.tpaWalkForward[1] = tpVisualObject->ID_Cycle("cr_walk_fwd_1");
//	
//	tStalkerAnimations.tCrouch.tGlobal.tpaWalkBack[0] = tpVisualObject->ID_Cycle("cr_walk_back_0");
//	tStalkerAnimations.tCrouch.tGlobal.tpaWalkBack[1] = tpVisualObject->ID_Cycle("cr_walk_back_1");
//	
//	tStalkerAnimations.tCrouch.tGlobal.tpaLieDown[0] = tpVisualObject->ID_Cycle("cr_lie_down_0");
//	tStalkerAnimations.tCrouch.tGlobal.tpaLieDown[1] = tpVisualObject->ID_Cycle("cr_lie_down_1");
//
//	tStalkerAnimations.tCrouch.tGlobal.tpDeath = tpVisualObject->ID_Cycle("cr_death_0");
//	tStalkerAnimations.tCrouch.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle("cr_turn_ls");
//	tStalkerAnimations.tCrouch.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle("cr_turn_rs");
//	tStalkerAnimations.tCrouch.tGlobal.tpIdle = tpVisualObject->ID_Cycle("cr_idle");
//	tStalkerAnimations.tCrouch.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle("cr_walk_ls");
//	tStalkerAnimations.tCrouch.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle("cr_walk_rs");
//	tStalkerAnimations.tCrouch.tGlobal.tpPointSign = tpVisualObject->ID_Cycle("cr_sign_2");
//
//	tStalkerAnimations.tCrouch.tGlobal.tpAim = tpVisualObject->ID_Cycle("cr_aim");
//	tStalkerAnimations.tCrouch.tGlobal.tpAttack = tpVisualObject->ID_Cycle("cr_attack");
//	tStalkerAnimations.tCrouch.tGlobal.tpReload = tpVisualObject->ID_Cycle("cr_reload");
//	tStalkerAnimations.tCrouch.tTorso.tpDamageLeft = tpVisualObject->ID_FX("cr_damage_ls");
//	tStalkerAnimations.tCrouch.tTorso.tpDamageRight = tpVisualObject->ID_FX("cr_damage_rs");
//	tStalkerAnimations.tCrouch.tTorso.tpRaiseHandSign = tpVisualObject->ID_Cycle("cr_sign_0");
//	tStalkerAnimations.tCrouch.tTorso.tpGoAheadSign = tpVisualObject->ID_Cycle("cr_sign_1");
//
//	tStalkerAnimations.tCrouch.tLegs.tpJumpBegin = tpVisualObject->ID_Cycle("cr_jump_begin");
//	tStalkerAnimations.tCrouch.tLegs.tpJumpIdle = tpVisualObject->ID_Cycle("cr_jump_idle");
//
//	// loading lie animations
//	tStalkerAnimations.tLie.tGlobal.tpDeath = tpVisualObject->ID_Cycle("lie_death");
//	tStalkerAnimations.tLie.tGlobal.tpStandUp = tpVisualObject->ID_Cycle("lie_stand_up");
//	tStalkerAnimations.tLie.tGlobal.tpIdle = tpVisualObject->ID_Cycle("lie_idle");
//	tStalkerAnimations.tLie.tGlobal.tpReload = tpVisualObject->ID_Cycle("lie_reload");
//	tStalkerAnimations.tLie.tGlobal.tpAttack = tpVisualObject->ID_Cycle("lie_attack");
//	tStalkerAnimations.tLie.tGlobal.tpDamage = tpVisualObject->ID_FX("lie_damage");
//	tStalkerAnimations.tLie.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle("lie_turn_ls");
//	tStalkerAnimations.tLie.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle("lie_turn_rs");
//	tStalkerAnimations.tLie.tGlobal.tpWalkForward = tpVisualObject->ID_Cycle("lie_walk_fwd");
//	tStalkerAnimations.tLie.tGlobal.tpWalkBack = tpVisualObject->ID_Cycle("lie_walk_back");
//	tStalkerAnimations.tLie.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle("lie_roll_ls");
//	tStalkerAnimations.tLie.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle("lie_roll_rs");
//
//	ZeroMemory				(m_tpaMovementAnimations,sizeof(m_tpaMovementAnimations));
//	
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_0] = tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[0];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_1] = tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[1];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_2] = tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[2];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_3] = tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[3];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_FORWARD_4] = tStalkerAnimations.tNormal.tGlobal.tpaWalkForward[4];
//	
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_0] = tStalkerAnimations.tNormal.tGlobal.tpaRunForward[0];
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_1] = tStalkerAnimations.tNormal.tGlobal.tpaRunForward[1];
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_2] = tStalkerAnimations.tNormal.tGlobal.tpaRunForward[2];
//	m_tpaMovementAnimations[BODY_STATE_STAND][RUN_FORWARD_3] = tStalkerAnimations.tNormal.tGlobal.tpaRunForward[3];
//	
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_BACK_0] = tStalkerAnimations.tNormal.tGlobal.tpaWalkBack[0];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_BACK_1] = tStalkerAnimations.tNormal.tGlobal.tpaWalkBack[1];
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_BACK_2] = tStalkerAnimations.tNormal.tGlobal.tpaWalkBack[2];
//
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_LEFT] = tStalkerAnimations.tNormal.tGlobal.tpWalkLeft;
//	m_tpaMovementAnimations[BODY_STATE_STAND][WALK_RIGHT] = tStalkerAnimations.tNormal.tGlobal.tpWalkRight;
//
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_FORWARD_0] = tStalkerAnimations.tCrouch.tGlobal.tpaWalkForward[0];
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_FORWARD_1] = tStalkerAnimations.tCrouch.tGlobal.tpaWalkForward[1];
//	
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_BACK_0] = tStalkerAnimations.tCrouch.tGlobal.tpaWalkBack[0];
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_BACK_1] = tStalkerAnimations.tCrouch.tGlobal.tpaWalkBack[1];
//
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_LEFT] = tStalkerAnimations.tCrouch.tGlobal.tpWalkLeft;
//	m_tpaMovementAnimations[BODY_STATE_CROUCH][WALK_RIGHT] = tStalkerAnimations.tCrouch.tGlobal.tpWalkRight;
//
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_FORWARD_0] = tStalkerAnimations.tLie.tGlobal.tpWalkForward;
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_BACK_0] = tStalkerAnimations.tLie.tGlobal.tpWalkBack;
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_LEFT] = tStalkerAnimations.tLie.tGlobal.tpWalkLeft;
//	m_tpaMovementAnimations[BODY_STATE_LIE][WALK_RIGHT] = tStalkerAnimations.tLie.tGlobal.tpWalkRight;
//
//	m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tStalkerAnimations.tNormal.tGlobal.tpaIdle[1]);
}

//static void __stdcall vfPlayCallBack(CBlend* B)
//{
//	CAI_Stalker	*tpStalker			= (CAI_Stalker*)B->CallbackParam;
//	tpStalker->m_bActionStarted = false;
//}

//void CAI_Stalker::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
//{
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
//}
