////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_animations.cpp
//	Created 	: 21.06.2002
//  Modified 	: 21.06.2002
//	Author		: Dmitriy Iassenev
//	Description : Animations for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"

void CAI_Soldier::vfLoadAnimations()
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	// loading normal animations
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[0] = tpVisualObject->ID_Cycle_Safe("norm_death");
	tSoldierAnimations.tNormal.tGlobal.tpaDeath[1] = tpVisualObject->ID_Cycle_Safe("norm_death_2");
	tSoldierAnimations.tNormal.tGlobal.tpJumpBegin = tpVisualObject->ID_Cycle_Safe("norm_jump_begin");
	tSoldierAnimations.tNormal.tGlobal.tpJumpIdle = tpVisualObject->ID_Cycle_Safe("norm_jump_idle");
	
	tSoldierAnimations.tNormal.tLegs.tRun.Create(tpVisualObject, "norm_run");
	tSoldierAnimations.tNormal.tLegs.tWalk.Create(tpVisualObject, "norm_walk");
	tSoldierAnimations.tNormal.tLegs.tpIdle = tpVisualObject->ID_Cycle_Safe("norm_idle");
	tSoldierAnimations.tNormal.tLegs.tpTurn = tpVisualObject->ID_Cycle_Safe("norm_turn");
	
	tSoldierAnimations.tNormal.tTorso.tpaAim[0] = tpVisualObject->ID_Cycle_Safe("norm_torso_aim_1");
	tSoldierAnimations.tNormal.tTorso.tpaAim[1] = tpVisualObject->ID_Cycle_Safe("norm_torso_aim_2");
	tSoldierAnimations.tNormal.tTorso.tpaAttack[0] = tpVisualObject->ID_Cycle_Safe("norm_torso_attack_1");
	tSoldierAnimations.tNormal.tTorso.tpaAttack[1] = tpVisualObject->ID_Cycle_Safe("norm_torso_attack_2");
	tSoldierAnimations.tNormal.tTorso.tpaIdle[0] = tpVisualObject->ID_Cycle_Safe("norm_torso_idle_1");
	tSoldierAnimations.tNormal.tTorso.tpaIdle[1] = tpVisualObject->ID_Cycle_Safe("norm_torso_idle_2");
	tSoldierAnimations.tNormal.tTorso.tpDamageLeft = tpVisualObject->ID_Cycle_Safe("norm_torso_damage_left");
	tSoldierAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_Cycle_Safe("norm_torso_damage_right");
	tSoldierAnimations.tNormal.tTorso.tpReload = tpVisualObject->ID_Cycle_Safe("norm_torso_reload");
	
	// loading crouch animations
	tSoldierAnimations.tCrouch.tGlobal.tpDeath = tpVisualObject->ID_Cycle_Safe("cr_death");
	tSoldierAnimations.tCrouch.tGlobal.tpJumpBegin = tpVisualObject->ID_Cycle_Safe("cr_jump_begin");
	tSoldierAnimations.tCrouch.tGlobal.tpJumpIdle = tpVisualObject->ID_Cycle_Safe("cr_jump_idle");
	
	tSoldierAnimations.tCrouch.tLegs.tRun.Create(tpVisualObject, "cr_run");
	tSoldierAnimations.tCrouch.tLegs.tWalk.Create(tpVisualObject, "cr_walk");
	tSoldierAnimations.tCrouch.tLegs.tpIdle = tpVisualObject->ID_Cycle_Safe("cr_idle");
	tSoldierAnimations.tCrouch.tLegs.tpTurn = tpVisualObject->ID_Cycle_Safe("cr_turn");
	
	tSoldierAnimations.tCrouch.tTorso.tpAim = tpVisualObject->ID_Cycle_Safe("cr_torso_aim");
	
	tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tTorso.tpaIdle[0]);
	tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tLegs.tpIdle);
	
	// loading lie animations
	tSoldierAnimations.tLie.tGlobal.tpDeath = tpVisualObject->ID_Cycle_Safe("lie_death");
	tSoldierAnimations.tLie.tGlobal.tpLieDown = tpVisualObject->ID_Cycle_Safe("lie_down");
	
	tSoldierAnimations.tLie.tTorso.tpIdle = tpVisualObject->ID_Cycle_Safe("lie_idle");
	tSoldierAnimations.tLie.tTorso.tpReload = tpVisualObject->ID_Cycle_Safe("lie_reload");
	
	tSoldierAnimations.tLie.tLegs.tWalk.Create(tpVisualObject, "lie_walk");
	
	tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tTorso.tpaIdle[0]);
	tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tLegs.tpIdle);
}

void CAI_Soldier::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	//R_ASSERT(fsimilar(_view.magnitude(),1));
	//R_ASSERT(fsimilar(_move.magnitude(),1));

	// choose motion animation
	/**/
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	CMotionDef*	tpLegsAnimation=0;
	CMotionDef*	tpHandsAnimation=0;
	CMotionDef*	tpTorsoAnimation=0;
	CMotionDef*	tpGlobalAnimation=0;

	if (iHealth <= 0) {
		switch (m_cBodyState) {
			case BODY_STATE_STAND : {
				for (int i=0 ;i<2; i++)
					if (tSoldierAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
						break;
					}
				if (!tpGlobalAnimation)
					tpGlobalAnimation = tSoldierAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)];
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
		//m_tpCurrentGlobalAnimation = 0;
	}
	else
		switch (eCurrentState) {
			case aiSoldierLyingDown : {
				tpGlobalAnimation = tSoldierAnimations.tLie.tGlobal.tpLieDown;
				tpTorsoAnimation = tpHandsAnimation = tpLegsAnimation = 0;
				break;
			}
			default : {
				if (speed<0.2f) {
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							if ((!(AI_Path.TravelPath.size()) || (eCurrentState != aiSoldierPatrolRoute))) {
								if (r_torso_target.yaw - r_torso_current.yaw > PI/30.f)
									tpLegsAnimation = tSoldierAnimations.tNormal.tLegs.tpTurn;
								else
									tpLegsAnimation = tSoldierAnimations.tNormal.tLegs.tpIdle;
							}
							else
								tpLegsAnimation = m_tpCurrentLegsAnimation;
							
							break;
						}
						case BODY_STATE_CROUCH : {
							if (r_torso_target.yaw - r_torso_current.yaw > PI/30.f)
								tpLegsAnimation = tSoldierAnimations.tCrouch.tLegs.tpTurn;
							else
								tpLegsAnimation = tSoldierAnimations.tCrouch.tLegs.tpIdle;
							break;
												 }
						case BODY_STATE_LIE : {
							//Msg("Animation Selection : can't turn while lying...");
							tpLegsAnimation = tSoldierAnimations.tLie.tLegs.tWalk.fwd;
							break;
						}
					}
					switch (eCurrentState) {
						case aiSoldierReload : {
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
									tpTorsoAnimation = tSoldierAnimations.tLie.tTorso.tpReload;
									break;
								}
							}
							break;
						}
						case aiSoldierAttackFire : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									/**
									for (int i=0 ;i<2; i++)
										if (tSoldierAnimations.tNormal.tTorso.tpaAttack[i] == m_tpCurrentTorsoAnimation) {
											tpTorsoAnimation = m_tpCurrentTorsoAnimation;
											break;
										}
									
									if (!tpTorsoAnimation)
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAttack[::Random.randI(0,2)];
									/**/
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAttack[1];
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAim;
									break;
														 }
								case BODY_STATE_LIE : {
									tpTorsoAnimation = tSoldierAnimations.tLie.tTorso.tpIdle;
									break;
													  }
							}
							break;
						}
						case aiSoldierAttackRun : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									for (int i=0 ;i<2; i++)
										if (tSoldierAnimations.tNormal.tTorso.tpaAim[i] == m_tpCurrentTorsoAnimation) {
											tpTorsoAnimation = m_tpCurrentTorsoAnimation;
											break;
										}
									
									if (!tpTorsoAnimation)
										tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAim[::Random.randI(0,2)];
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAim;
									break;
								}
								case BODY_STATE_LIE : {
									tpTorsoAnimation = tSoldierAnimations.tLie.tTorso.tpIdle;
									break;
								}
							}
							break;
						}
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
									tpTorsoAnimation = tSoldierAnimations.tLie.tTorso.tpIdle;
									break;
								}
							}
							break;
						}
					}
				}
				else {
					//Msg("moving...");
					Fvector view = _view; view.y=0; view.normalize_safe();
					Fvector move = _move; move.y=0; move.normalize_safe();
					float	dot  = view.dotproduct(move);
					
					SAnimState* AState = 0;
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							AState = &tSoldierAnimations.tNormal.tLegs.tWalk;
							break;
						}
						case BODY_STATE_CROUCH : {
							AState = &tSoldierAnimations.tCrouch.tLegs.tWalk;
							break;
						}
						case BODY_STATE_LIE : {
							AState = &tSoldierAnimations.tLie.tLegs.tWalk;
							break;
						}
					}
					
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							if (speed >= m_fMaxSpeed - EPS_L)
								AState = &tSoldierAnimations.tNormal.tLegs.tRun;
							break;
						}
						case BODY_STATE_CROUCH : {
							if (speed >= m_fMaxSpeed*m_fCrouchCoefficient)
								AState = &tSoldierAnimations.tCrouch.tLegs.tRun;
							break;
						}
						case BODY_STATE_LIE : {
							break;
						}
					}
					
					if (dot>0.7f)
						tpLegsAnimation = AState->fwd;
					else 
						if ((dot<=0.7f)&&(dot>=-0.7f)) {
							Fvector cross; 
							cross.crossproduct(view,move);
							if (cross.y > 0)
								tpLegsAnimation = AState->rs;
							else
								tpLegsAnimation = AState->ls;
						}
						else
							tpLegsAnimation = AState->back;

					switch (eCurrentState) {
						case aiSoldierReload : {
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
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpReload;
									break;
								}
							}
							break;
						}
						case aiSoldierAttackFire : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									/**
									for (int i=0 ;i<2; i++)
										if (tSoldierAnimations.tNormal.tTorso.tpaAttack[i] == m_tpCurrentTorsoAnimation) {
											tpTorsoAnimation = m_tpCurrentTorsoAnimation;
											break;
										}
									
									if (!tpTorsoAnimation)
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAttack[::Random.randI(0,2)];
									/**/
									tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAttack[1];
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAim;
									break;
														 }
								case BODY_STATE_LIE : {
									tpTorsoAnimation = tSoldierAnimations.tLie.tTorso.tpIdle;
									break;
													  }
							}
							break;
						}
						case aiSoldierAttackRun : {
							switch (m_cBodyState) {
								case BODY_STATE_STAND : {
									for (int i=0 ;i<2; i++)
										if (tSoldierAnimations.tNormal.tTorso.tpaAim[i] == m_tpCurrentTorsoAnimation) {
											tpTorsoAnimation = m_tpCurrentTorsoAnimation;
											break;
										}
									
									if (!tpTorsoAnimation)
										tpTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAim[::Random.randI(0,2)];
									break;
								}
								case BODY_STATE_CROUCH : {
									tpTorsoAnimation = tSoldierAnimations.tCrouch.tTorso.tpAim;
									break;
								}
								case BODY_STATE_LIE : {
									tpTorsoAnimation = tSoldierAnimations.tLie.tTorso.tpIdle;
									break;
								}
							}
							break;
						}
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
									tpTorsoAnimation = tSoldierAnimations.tLie.tTorso.tpIdle;
									break;
								}
							}
							break;
						}
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

	if (tpTorsoAnimation != m_tpCurrentTorsoAnimation) { 
		//Msg("restarting animation..."); 
		m_tpCurrentTorsoAnimation = tpTorsoAnimation;
		if (tpTorsoAnimation) {
			m_tpCurrentTorsoBlend = tpVisualObject->PlayCycle(tpTorsoAnimation);
		}
	}

	if (tpHandsAnimation != m_tpCurrentHandsAnimation) { 
		//Msg("restarting animation..."); 
		m_tpCurrentHandsAnimation = tpHandsAnimation;
		if (tpHandsAnimation) {
			m_tpCurrentHandsBlend = tpVisualObject->PlayCycle(tpHandsAnimation);
		}
	}
}