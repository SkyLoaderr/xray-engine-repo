////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_animations.cpp
//	Created 	: 21.06.2002
//  Modified 	: 21.06.2002
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"

#define TORSO_ANGLE_DELTA				(PI/30.f)

// sounds
void CAI_Zombie::vfLoadSounds()
{
	pSounds->Create(sndHit[0],TRUE,"actor\\bhit_flesh-1",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndHit[1],TRUE,"actor\\bhit_flesh-2",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndHit[2],TRUE,"actor\\bhit_flesh-3",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndHit[3],TRUE,"actor\\bhit_helmet-1",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndHit[4],TRUE,"actor\\bullet_hit1",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndHit[5],TRUE,"actor\\bullet_hit2",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndHit[6],TRUE,"actor\\ric_conc-1",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndHit[7],TRUE,"actor\\ric_conc-2",0,SOUND_TYPE_MONSTER_INJURING_HUMAN);
	pSounds->Create(sndDie[0],TRUE,"actor\\die0",0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	pSounds->Create(sndDie[1],TRUE,"actor\\die1",0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	pSounds->Create(sndDie[2],TRUE,"actor\\die2",0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	pSounds->Create(sndDie[3],TRUE,"actor\\die3",0,SOUND_TYPE_MONSTER_DYING_HUMAN);
	pSounds->Create(sndSteps[0],TRUE,"Actor\\StepL",0,SOUND_TYPE_MONSTER_WALKING_HUMAN);
	pSounds->Create(sndSteps[1],TRUE,"Actor\\StepR",0,SOUND_TYPE_MONSTER_WALKING_HUMAN);
	pSounds->Create(sndVoices[0],TRUE,"monsters\\zombie\\zombie_1",0,SOUND_TYPE_MONSTER_TALKING_HUMAN);
	pSounds->Create(sndVoices[1],TRUE,"monsters\\zombie\\zombie_2",0,SOUND_TYPE_MONSTER_TALKING_HUMAN);
}

// animations
void CAI_Zombie::vfLoadAnimations()
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	// loading normal animations
	tZombieAnimations.tNormal.tGlobal.tpaDeath[0] = tpVisualObject->ID_Cycle("norm_death");
	tZombieAnimations.tNormal.tGlobal.tpaDeath[1] = tpVisualObject->ID_Cycle("norm_death_1");
	tZombieAnimations.tNormal.tGlobal.tpaDeath[2] = tpVisualObject->ID_Cycle("norm_death_2");
	
	tZombieAnimations.tNormal.tGlobal.tpaAttack[0] = tpVisualObject->ID_Cycle("attack");
	tZombieAnimations.tNormal.tGlobal.tpaAttack[1] = tpVisualObject->ID_Cycle("attack_1");
	tZombieAnimations.tNormal.tGlobal.tpaAttack[2] = tpVisualObject->ID_Cycle("attack_2");
	
	tZombieAnimations.tNormal.tGlobal.tWalk.fwd = tpVisualObject->ID_Cycle("norm_walk_fwd");
	tZombieAnimations.tNormal.tGlobal.tWalk.back = tpVisualObject->ID_Cycle("norm_walk_back");
	tZombieAnimations.tNormal.tGlobal.tWalk.ls = tpVisualObject->ID_Cycle("norm_walk_ls");
	tZombieAnimations.tNormal.tGlobal.tWalk.rs = tpVisualObject->ID_Cycle("norm_walk_rs");

	tZombieAnimations.tNormal.tGlobal.tpaIdle[0] = tpVisualObject->ID_Cycle("norm_idle");
	tZombieAnimations.tNormal.tGlobal.tpaIdle[1] = tpVisualObject->ID_Cycle("norm_idle_1");
	
	tZombieAnimations.tNormal.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle("norm_turn_ls");
	tZombieAnimations.tNormal.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle("norm_turn_rs");
	
	tZombieAnimations.tNormal.tTorso.tpDamageLeft = tpVisualObject->ID_FX("norm_damage_ls");
	tZombieAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_FX("norm_damage_rs");
	
	tpVisualObject->PlayCycle(tZombieAnimations.tNormal.tGlobal.tpaIdle[1]);
}

void CAI_Zombie::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
//	CMotionDef*	tpLegsAnimation=0;
	CMotionDef*	tpGlobalAnimation=0;

	//tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpaDeath[2];
	/**/
	if (fHealth <= 0) {
		switch (m_cBodyState) {
			case BODY_STATE_STAND : {
				for (int i=0 ;i<3; i++)
					if (tZombieAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
						break;
					}
				if (!tpGlobalAnimation)
					tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,3)];
				break;
			}
			case BODY_STATE_CROUCH : {
				break;
			}
			case BODY_STATE_LIE : {
				break;
			}
		}
	}
	else
		if (speed<0.1f) {
			switch (eCurrentState) {
				case aiZombieAttackFire : {
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							tpGlobalAnimation = 0;
							for (int i=0 ;i<3; i++)
								if (tZombieAnimations.tNormal.tGlobal.tpaAttack[i] == m_tpCurrentGlobalAnimation) {
									tpGlobalAnimation = m_tpCurrentGlobalAnimation;
									break;
								}
							
							if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpaAttack[::Random.randI(2,3)];
							break;
						}
						case BODY_STATE_CROUCH : {
							break;
						}
						case BODY_STATE_LIE : {
							break;
						}
					}
					break;
				}
				case aiZombieTurnOver : {
					if (_abs(r_torso_target.yaw - r_torso_current.yaw) <= PI)
						if (_abs(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
							if (r_torso_target.yaw - r_torso_current.yaw >= 0)
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpTurnRight;
							else
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpTurnLeft;
						else
							tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpaIdle[0];
				}
				default : {
					if (_abs(r_torso_target.yaw - r_torso_current.yaw) <= PI)
						if (_abs(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
							if (r_torso_target.yaw - r_torso_current.yaw >= 0)
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpTurnRight;
							else
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpTurnLeft;
						else
							tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpaIdle[1];
					else
						if (PI_MUL_2 - _abs(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
							if (r_torso_target.yaw > r_torso_current.yaw)
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpTurnLeft;
							else
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpTurnRight;
						else
							tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpaIdle[1];
					
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
					AState = &tZombieAnimations.tNormal.tGlobal.tWalk;
					break;
				}
				case BODY_STATE_CROUCH : {
					break;
				}
				case BODY_STATE_LIE : {
					break;
				}
			}
			
			if (dot>0.7f)
				tpGlobalAnimation = AState->fwd;
			else 
				if ((dot<=0.7f)&&(dot>=-0.7f)) {
					Fvector cross; 
					cross.crossproduct(view,move);
					if (cross.y > 0)
						tpGlobalAnimation = AState->rs;
					else
						tpGlobalAnimation = AState->ls;
				}
				else
					tpGlobalAnimation = AState->back;

			switch (eCurrentState) {
				case aiZombieAttackFire : {
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							tpGlobalAnimation = 0;
							for (int i=0 ;i<2; i++)
								if (tZombieAnimations.tNormal.tGlobal.tpaAttack[i] == m_tpCurrentGlobalAnimation) {
									tpGlobalAnimation = m_tpCurrentGlobalAnimation;
									break;
								}
							
							if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
								tpGlobalAnimation = tZombieAnimations.tNormal.tGlobal.tpaAttack[::Random.randI(0,2)];
							break;
						}
						case BODY_STATE_CROUCH : {
							break;
						}
						case BODY_STATE_LIE : {
							break;
						}
					}
					break;
				}
				default : {
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							break;
						}
						case BODY_STATE_CROUCH : {
							break;
						}
						case BODY_STATE_LIE : {
							break;
						}
					}
					break;
				}
			}
		}
	/**/
	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
		if (tpGlobalAnimation) {
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
		}
	}
}