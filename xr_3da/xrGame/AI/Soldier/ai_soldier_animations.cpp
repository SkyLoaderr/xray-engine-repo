////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_animations.cpp
//	Created 	: 21.06.2002
//  Modified 	: 21.06.2002
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"

#define TORSO_ANGLE_DELTA				(PI/30.f)

// bones
void CAI_Soldier::vfAssignBones(CInifile *ini, const char *section)
{
	int head_bone = PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_head"));
	PKinematics(pVisual)->LL_GetInstance(head_bone).set_callback(HeadSpinCallback,this);
	
	int torso_bone = PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_torso"));
	PKinematics(pVisual)->LL_GetInstance(torso_bone).set_callback(SpineSpinCallback,this);
}

void __stdcall CAI_Soldier::HeadSpinCallback(CBoneInstance* B)
{
	CAI_Soldier*		A = dynamic_cast<CAI_Soldier*> (static_cast<CObject*>(B->Callback_Param));
	
	Fmatrix				spin;
	spin.setXYZ			(A->r_current.yaw - A->r_torso_current.yaw, A->r_current.pitch, 0);
	B->mTransform.mulB_43(spin);
}

void __stdcall CAI_Soldier::SpineSpinCallback(CBoneInstance* B)
{
	CAI_Soldier*		A = dynamic_cast<CAI_Soldier*> (static_cast<CObject*>(B->Callback_Param));
	
	Fmatrix				spin;
	spin.setXYZ			(A->r_spine_current.yaw - A->r_torso_current.yaw, A->r_spine_current.pitch, 0);
	B->mTransform.mulB_43(spin);
}

// sounds
void CAI_Soldier::vfLoadSounds()
{
	pSounds->Create3D(sndHit[0],"actor\\bhit_flesh-1");
	pSounds->Create3D(sndHit[1],"actor\\bhit_flesh-2");
	pSounds->Create3D(sndHit[2],"actor\\bhit_flesh-3");
	pSounds->Create3D(sndHit[3],"actor\\bhit_helmet-1");
	pSounds->Create3D(sndHit[4],"actor\\bullet_hit1");
	pSounds->Create3D(sndHit[5],"actor\\bullet_hit2");
	pSounds->Create3D(sndHit[6],"actor\\ric_conc-1");
	pSounds->Create3D(sndHit[7],"actor\\ric_conc-2");
	pSounds->Create3D(sndDie[0],"actor\\die0");
	pSounds->Create3D(sndDie[1],"actor\\die1");
	pSounds->Create3D(sndDie[2],"actor\\die2");
	pSounds->Create3D(sndDie[3],"actor\\die3");
}

// animations
void CAI_Soldier::vfLoadAnimations()
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	// loading normal animations
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[0] = tpVisualObject->ID_Cycle_Safe("norm_death_0");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[1] = tpVisualObject->ID_Cycle_Safe("norm_death_1");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[2] = tpVisualObject->ID_Cycle_Safe("norm_death_2");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[3] = tpVisualObject->ID_Cycle_Safe("norm_death_3");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[4] = tpVisualObject->ID_Cycle_Safe("norm_death_4");
	
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[0] = tpVisualObject->ID_Cycle_Safe("norm_run_fwd_0");
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[1] = tpVisualObject->ID_Cycle_Safe("norm_run_fwd_1");
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[2] = tpVisualObject->ID_Cycle_Safe("norm_run_fwd_2");
	tSoldierAnimations.tNormal.tGlobal.tpaRunForward[3] = tpVisualObject->ID_Cycle_Safe("norm_run_fwd_3");

	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0] = tpVisualObject->ID_Cycle_Safe("norm_walk_fwd_0");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[1] = tpVisualObject->ID_Cycle_Safe("norm_walk_fwd_1");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[2] = tpVisualObject->ID_Cycle_Safe("norm_walk_fwd_2");
	
	tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[0] = tpVisualObject->ID_Cycle_Safe("norm_walk_back_0");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[1] = tpVisualObject->ID_Cycle_Safe("norm_walk_back_1");
	tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[2] = tpVisualObject->ID_Cycle_Safe("norm_walk_back_2");
	
	tSoldierAnimations.tNormal.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle_Safe("norm_walk_ls");
	tSoldierAnimations.tNormal.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle_Safe("norm_walk_rs");
	
	tSoldierAnimations.tNormal.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle_Safe("norm_turn_ls");
	tSoldierAnimations.tNormal.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle_Safe("norm_turn_rs");
	
	tSoldierAnimations.tNormal.tGlobal.tpLieDown = tpVisualObject->ID_Cycle_Safe("norm_lie_down");
	tSoldierAnimations.tNormal.tGlobal.tpPointSign = tpVisualObject->ID_Cycle_Safe("norm_sign_2");
	tSoldierAnimations.tNormal.tGlobal.tpIdle = tpVisualObject->ID_Cycle_Safe("norm_idle");

	tSoldierAnimations.tNormal.tTorso.tpAim = tpVisualObject->ID_Cycle_Safe("norm_aim");
	tSoldierAnimations.tNormal.tTorso.tpAttack = tpVisualObject->ID_Cycle_Safe("norm_attack");
	tSoldierAnimations.tNormal.tTorso.tpDamageLeft = tpVisualObject->ID_Cycle_Safe("norm_damage_left");
	tSoldierAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_Cycle_Safe("norm_damage_right");
	tSoldierAnimations.tNormal.tTorso.tpReload = tpVisualObject->ID_Cycle_Safe("norm_reload");
	tSoldierAnimations.tNormal.tTorso.tpRaiseHandSign = tpVisualObject->ID_Cycle_Safe("norm_sign_0");
	tSoldierAnimations.tNormal.tTorso.tpGoAheadSign = tpVisualObject->ID_Cycle_Safe("norm_sign_1");
	
	tSoldierAnimations.tNormal.tLegs.tpJumpBegin = tpVisualObject->ID_Cycle_Safe("norm_jump_begin");
	tSoldierAnimations.tNormal.tLegs.tpJumpIdle = tpVisualObject->ID_Cycle_Safe("norm_jump_idle");
	
	// loading crouch animations
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[0] = tpVisualObject->ID_Cycle_Safe("cr_walk_fwd_0");
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[1] = tpVisualObject->ID_Cycle_Safe("cr_walk_fwd_1");
	
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[0] = tpVisualObject->ID_Cycle_Safe("cr_walk_back_0");
	tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[1] = tpVisualObject->ID_Cycle_Safe("cr_walk_back_1");
	
	tSoldierAnimations.tCrouch.tGlobal.tpDeath = tpVisualObject->ID_Cycle_Safe("cr_death_0");
	tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle_Safe("cr_turn_ls");
	tSoldierAnimations.tCrouch.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle_Safe("cr_turn_rs");
	tSoldierAnimations.tCrouch.tGlobal.tpIdle = tpVisualObject->ID_Cycle_Safe("cr_idle");
	tSoldierAnimations.tCrouch.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle_Safe("cr_walk_ls");
	tSoldierAnimations.tCrouch.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle_Safe("cr_walk_rs");
	tSoldierAnimations.tCrouch.tGlobal.tpLieDown = tpVisualObject->ID_Cycle_Safe("cr_lie_down");
	tSoldierAnimations.tCrouch.tGlobal.tpPointSign = tpVisualObject->ID_Cycle_Safe("cr_sign_2");

	tSoldierAnimations.tCrouch.tTorso.tpAim = tpVisualObject->ID_Cycle_Safe("cr_walk_rs");
	tSoldierAnimations.tCrouch.tTorso.tpAttack = tpVisualObject->ID_Cycle_Safe("cr_walk_rs");
	tSoldierAnimations.tCrouch.tTorso.tpReload = tpVisualObject->ID_Cycle_Safe("cr_reload");
	tSoldierAnimations.tCrouch.tTorso.tpDamageLeft = tpVisualObject->ID_Cycle_Safe("cr_damage_ls");
	tSoldierAnimations.tCrouch.tTorso.tpDamageRight = tpVisualObject->ID_Cycle_Safe("cr_damage_rs");
	tSoldierAnimations.tCrouch.tTorso.tpRaiseHandSign = tpVisualObject->ID_Cycle_Safe("cr_sign_0");
	tSoldierAnimations.tCrouch.tTorso.tpGoAheadSign = tpVisualObject->ID_Cycle_Safe("cr_sign_1");

	tSoldierAnimations.tCrouch.tLegs.tpJumpBegin = tpVisualObject->ID_Cycle_Safe("cr_jump_begin");
	tSoldierAnimations.tCrouch.tLegs.tpJumpIdle = tpVisualObject->ID_Cycle_Safe("cr_jump_idle");

	// loading lie animations
	tSoldierAnimations.tLie.tGlobal.tpDeath = tpVisualObject->ID_Cycle_Safe("lie_death");
	tSoldierAnimations.tLie.tGlobal.tpStandUp = tpVisualObject->ID_Cycle_Safe("stand_up");
	tSoldierAnimations.tLie.tGlobal.tpIdle = tpVisualObject->ID_Cycle_Safe("lie_idle");
	tSoldierAnimations.tLie.tGlobal.tpReload = tpVisualObject->ID_Cycle_Safe("lie_reload");
	tSoldierAnimations.tLie.tGlobal.tpAttack = tpVisualObject->ID_Cycle_Safe("lie_attack");
	tSoldierAnimations.tLie.tGlobal.tpDamage = tpVisualObject->ID_Cycle_Safe("lie_damage");
	tSoldierAnimations.tLie.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle_Safe("lie_turn_ls");
	tSoldierAnimations.tLie.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle_Safe("lie_turn_rs");
	tSoldierAnimations.tLie.tGlobal.tpWalkForward = tpVisualObject->ID_Cycle_Safe("lie_walk_fwd");
	tSoldierAnimations.tLie.tGlobal.tpWalkBack = tpVisualObject->ID_Cycle_Safe("lie_walk_back");
	tSoldierAnimations.tLie.tGlobal.tpWalkLeft = tpVisualObject->ID_Cycle_Safe("lie_roll_ls");
	tSoldierAnimations.tLie.tGlobal.tpWalkRight = tpVisualObject->ID_Cycle_Safe("lie_roll_rs");
	tSoldierAnimations.tLie.tGlobal.tpRaiseHandSign = tpVisualObject->ID_Cycle_Safe("lie_sign_0");
	tSoldierAnimations.tLie.tGlobal.tpGoAheadSign = tpVisualObject->ID_Cycle_Safe("lie_sign_1");
	tSoldierAnimations.tLie.tGlobal.tpPointSign = tpVisualObject->ID_Cycle_Safe("lie_sign_2");
}

void CAI_Soldier::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	//R_ASSERT(fsimilar(_view.magnitude(),1));
	//R_ASSERT(fsimilar(_move.magnitude(),1));

	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	CMotionDef*	tpLegsAnimation=0;
	CMotionDef*	tpTorsoAnimation=0;
	CMotionDef*	tpGlobalAnimation=0;

	if (iHealth <= 0) {
		switch (m_cBodyState) {
			case BODY_STATE_STAND : {
				for (int i=0 ;i<5; i++)
					if (tSoldierAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
						break;
					}
				if (!tpGlobalAnimation)
					tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,5)];
				break;
			}
			case BODY_STATE_CROUCH : {
				tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpDeath;
				break;
			}
			case BODY_STATE_LIE : {
				tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpDeath;
				break;
			}
		}
	}
	else
		switch (eCurrentState) {
			case aiSoldierLyingDown : {
				switch (m_cBodyState) {
					case BODY_STATE_STAND : {
						tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpLieDown;
						break;
					}
					case BODY_STATE_CROUCH : {
						tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpLieDown;
						break;
					}
					case BODY_STATE_LIE : {
						break;
					}
				}
				tpTorsoAnimation = tpLegsAnimation = 0;
				break;
			}
			case aiSoldierStandingUp : {
				switch (m_cBodyState) {
					case BODY_STATE_STAND : {
						break;
					}
					case BODY_STATE_CROUCH : {
						break;
					}
					case BODY_STATE_LIE : {
						tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpStandUp;
						break;
					}
				}
				tpTorsoAnimation = tpLegsAnimation = 0;
				break;
			}
			case aiSoldierJumping : {
				switch (m_cBodyState) {
					case BODY_STATE_STAND : {
						if ((m_tpCurrentLegsAnimation != tSoldierAnimations.tNormal.tLegs.tpJumpBegin) && (m_tpCurrentLegsAnimation != tSoldierAnimations.tNormal.tLegs.tpJumpIdle)) {
							tpLegsAnimation = tSoldierAnimations.tNormal.tLegs.tpJumpBegin;
							//Msg("Jump begin");
						}
						else
							//if ((ps_Size() < 2) || (ps_Element(ps_Size() - 1).vPosition.y - ps_Element(ps_Size() - 2).vPosition.y > -EPS_L)) {
							if (m_tpCurrentLegsBlend->playing) {
								tpLegsAnimation = m_tpCurrentLegsAnimation;
								//Msg("Jump continueing");
							}
							else {
								tpLegsAnimation = tSoldierAnimations.tNormal.tLegs.tpJumpIdle;
								//Msg("Jump idle");
							}
						break;
					}
					case BODY_STATE_CROUCH : {
						if ((m_tpCurrentLegsAnimation != tSoldierAnimations.tCrouch.tLegs.tpJumpBegin) && (m_tpCurrentLegsAnimation != tSoldierAnimations.tCrouch.tLegs.tpJumpIdle))
							tpLegsAnimation = tSoldierAnimations.tCrouch.tLegs.tpJumpBegin;
						else
							if (m_tpCurrentLegsBlend->playing)
								tpLegsAnimation = m_tpCurrentLegsAnimation;
							else
								tpLegsAnimation = tSoldierAnimations.tCrouch.tLegs.tpJumpIdle;
						break;
												}
					case BODY_STATE_LIE : {
						tpLegsAnimation = tSoldierAnimations.tLie.tGlobal.tpWalkForward;
						break;
					}
				}
				break;
			}
			default : {
				// not moving
				if (speed < 0.2f) {
					// turning around || standing idle
					if (fabsf(r_torso_target.yaw - r_torso_current.yaw) <= PI)
						if (fabsf(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
							if (r_torso_target.yaw - r_torso_current.yaw >= 0)
								switch (m_cBodyState) {
									case BODY_STATE_STAND : {
										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnRight;
										break;
									}
									case BODY_STATE_CROUCH : {
										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnRight;
										break;
									}
									case BODY_STATE_LIE : {
										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnRight;
										break;
									}
								}
							else
								switch (m_cBodyState) {
									case BODY_STATE_STAND : {
										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnLeft;
										break;
									}
									case BODY_STATE_CROUCH : {
										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft;
										break;
									}
									case BODY_STATE_LIE : {
										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnLeft;
										break;
									}
								}
						else
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpIdle;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpIdle;
									break;
								}
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
									break;
								}
							}
					else
						if (PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
							if (r_torso_target.yaw > r_torso_current.yaw)
								switch (m_cBodyState) {
									case BODY_STATE_STAND : {
										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnLeft;
										break;
									}
									case BODY_STATE_CROUCH : {
										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft;
										break;
									}
									case BODY_STATE_LIE : {
										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnLeft;
										break;
									}
								}
							else
								switch (m_cBodyState) {
									case BODY_STATE_STAND : {
										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpTurnRight;
										break;
									}
									case BODY_STATE_CROUCH : {
										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpTurnRight;
										break;
									}
									case BODY_STATE_LIE : {
										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpTurnRight;
										break;
									}
								}
						switch (m_cBodyState) {
							case BODY_STATE_STAND : {
								tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpIdle;
								break;
							}
							case BODY_STATE_CROUCH : {
								tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpIdle;
								break;
							}
							case BODY_STATE_LIE : {
								tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
								break;
							}
						}
					// torso
					switch (eCurrentState) {
						case aiSoldierRecharge : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpReload;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpReload;
									break;
								}
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpReload;
									break;
								}
							}
							break;
						}
						/**/
						case aiSoldierAttackFire : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpAttack;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAttack;
									break;
								}
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpAttack;
									break;
								}
							}
							break;
						}
						case aiSoldierAttackRun : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpAim;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAim;
									break;
								}
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
									break;
								}
							}
							break;
						}
						/**
						default : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpIdle;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpIdle;
									break;
								}
								case BODY_STATE_LIE : {
									tpTorsoAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
									break;
								}
							}
							break;
						}
						/**/
					}
				}
				else {
					//Msg("moving...");
					Fvector view = _view, move = _move; 
					move.y = view.y = 0; 
					view.normalize_safe();
					move.normalize_safe();
					float dot = view.dotproduct(move);
					
					if ((speed >= m_fMaxSpeed - EPS_L) && (m_cBodyState == BODY_STATE_STAND) && (dot > .7f)) {
						for (int i=0; i<4; i++)
							if (tSoldierAnimations.tNormal.tGlobal.tpaRunForward[i] == m_tpCurrentGlobalAnimation) {
								tpGlobalAnimation = m_tpCurrentGlobalAnimation;
								break;
							}
						if (!tpGlobalAnimation)
							tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaRunForward[::Random.randI(0,4)];
					}
					else
						if (dot > .7f)
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									for (int i=0; i<3; i++)
										if (tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[i] == m_tpCurrentGlobalAnimation) {
											tpGlobalAnimation = m_tpCurrentGlobalAnimation;
											break;
										}
									if (!tpGlobalAnimation)
										tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[::Random.randI(0,3)];
									break;
								}
								case BODY_STATE_CROUCH : {
									for (int i=0; i<2; i++)
										if (tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[i] == m_tpCurrentGlobalAnimation) {
											tpGlobalAnimation = m_tpCurrentGlobalAnimation;
											break;
										}
									if (!tpGlobalAnimation)
										tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[::Random.randI(0,2)];
									break;
								}
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpWalkForward;
									break;
								}
							}
						else 
							if ((dot<=0.7f)&&(dot>=-0.7f)) {
								Fvector cross; 
								cross.crossproduct(view,move);
								if (cross.y > 0)
									switch (m_cBodyState) {
										case BODY_STATE_STAND : {
											tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpWalkRight;
											break;
										}
										case BODY_STATE_CROUCH : {
											tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpWalkRight;
											break;
										}
										case BODY_STATE_LIE : {
											tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpWalkRight;
											break;
										}
									}
								else
									switch (m_cBodyState) {
										case BODY_STATE_STAND : {
											tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpWalkLeft;
											break;
										}
										case BODY_STATE_CROUCH : {
											tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpWalkLeft;
											break;
										}
										case BODY_STATE_LIE : {
											tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpWalkLeft;
											break;
										}
									}
							}
							else
								switch (m_cBodyState) {
									case BODY_STATE_STAND : {
										for (int i=0; i<3; i++)
											if (tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[i] == m_tpCurrentGlobalAnimation) {
												tpGlobalAnimation = m_tpCurrentGlobalAnimation;
												break;
											}
										if (!tpGlobalAnimation)
											tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[::Random.randI(0,3)];
										break;
									}
									case BODY_STATE_CROUCH : {
										for (int i=0; i<2; i++)
											if (tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[i] == m_tpCurrentGlobalAnimation) {
												tpGlobalAnimation = m_tpCurrentGlobalAnimation;
												break;
											}
										if (!tpGlobalAnimation)
											tpGlobalAnimation = tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[::Random.randI(0,2)];
										break;
									}
									case BODY_STATE_LIE : {
										tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpWalkBack;
										break;
									}
								}
					//torso
					switch (eCurrentState) {
						case aiSoldierRecharge : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpReload;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpReload;
									break;
														 }
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpReload;
									break;
								}
							}
							break;
						}
						case aiSoldierAttackFire : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpAttack;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAttack;
									break;
														 }
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpAttack;
									break;
													  }
							}
							break;
						}
						case aiSoldierAttackRun : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpAim;
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAim;
									break;
								}
								case BODY_STATE_LIE : {
									tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
									break;
								}
							}
							break;
						}
						/**
						default : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									for (int i=0 ;i<2; i++)
										if (tSoldierAnimations.tNormal.tTorso.tpaIdle[i] == m_tpCurrentTorsoAnimation) {
											tpTorsoAnimation = m_tpCurrentTorsoAnimation;
											break;
										}
									
									if (!tpTorsoAnimation)
										tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaIdle[::Random.randI(0,2)];
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAim;
									break;
								}
								case BODY_STATE_LIE : {
									tpTorsoAnimation = tSoldierAnimations.tLie.tGlobal.tpIdle;
									break;
								}
							}
							break;
						}
						/**/
					}
				}
			}
		}
	
	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
		//Msg("restarting animation..."); 
		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
		if (tpGlobalAnimation) {
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
		}
	}

	if (tpTorsoAnimation != m_tpCurrentTorsoAnimation) { 
		//Msg("restarting animation..."); 
		m_tpCurrentTorsoAnimation = tpTorsoAnimation;
		if (tpTorsoAnimation) {
			m_tpCurrentTorsoBlend = tpVisualObject->PlayCycle(tpTorsoAnimation);
		}
	}

	if (tpLegsAnimation != m_tpCurrentLegsAnimation) { 
		//Msg("restarting animation..."); 
		m_tpCurrentLegsAnimation = tpLegsAnimation;
		if (tpLegsAnimation) {
			m_tpCurrentLegsBlend = tpVisualObject->PlayCycle(tpLegsAnimation);
			if (tpLegsAnimation == m_walk.fwd) {
				m_tpCurrentLegsBlend->timeCurrent = ::Random.randF(m_tpCurrentLegsBlend->timeTotal);
			}
		}
	}
}