////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_animations.cpp
//	Created 	: 21.06.2002
//  Modified 	: 21.06.2002
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"

#define TORSO_ANGLE_DELTA				(PI/30.f)

// bones
void CAI_Rat::vfAssignBones(CInifile *ini, const char *section)
{
	int head_bone = PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_head"));
	PKinematics(pVisual)->LL_GetInstance(head_bone).set_callback(HeadSpinCallback,this);
	
	int torso_bone = PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_torso"));
	PKinematics(pVisual)->LL_GetInstance(torso_bone).set_callback(SpineSpinCallback,this);
}

void __stdcall CAI_Rat::HeadSpinCallback(CBoneInstance* B)
{
	CAI_Rat*		A = dynamic_cast<CAI_Rat*> (static_cast<CObject*>(B->Callback_Param));
	
	Fmatrix				spin;
	spin.setXYZ			(A->r_current.yaw - A->r_torso_current.yaw, A->r_current.pitch, 0);
	B->mTransform.mulB_43(spin);
}

void __stdcall CAI_Rat::SpineSpinCallback(CBoneInstance* B)
{
	CAI_Rat*		A = dynamic_cast<CAI_Rat*> (static_cast<CObject*>(B->Callback_Param));
	
	Fmatrix				spin;
	spin.setXYZ			(A->r_spine_current.yaw - A->r_torso_current.yaw, A->r_spine_current.pitch, 0);
	B->mTransform.mulB_43(spin);
}

// sounds
void CAI_Rat::vfLoadSounds()
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
void CAI_Rat::vfLoadAnimations()
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	// loading normal animations
	tRatAnimations.tNormal.tGlobal.tpaDeath[0] = tpVisualObject->ID_Cycle_Safe("norm_death");
	tRatAnimations.tNormal.tGlobal.tpaDeath[1] = tpVisualObject->ID_Cycle_Safe("norm_death_2");
	tRatAnimations.tNormal.tGlobal.tpaDeath[2] = tpVisualObject->ID_Cycle_Safe("norm_death_3");
	
	tRatAnimations.tNormal.tGlobal.tpaAttack[0] = tpVisualObject->ID_Cycle_Safe("attack");
	tRatAnimations.tNormal.tGlobal.tpaAttack[1] = tpVisualObject->ID_Cycle_Safe("attack_1");
	
	tRatAnimations.tNormal.tGlobal.tWalk.Create(tpVisualObject, "norm_walk");
	tRatAnimations.tNormal.tGlobal.tpIdle = tpVisualObject->ID_Cycle_Safe("norm_idle");
	
	tRatAnimations.tNormal.tLegs.tpTurn = tpVisualObject->ID_Cycle_Safe("norm_turn");
	
	tRatAnimations.tNormal.tGlobal.tpDamageLeft = tpVisualObject->ID_Cycle_Safe("norm_torso_damage_left");
	tRatAnimations.tNormal.tGlobal.tpDamageRight = tpVisualObject->ID_Cycle_Safe("norm_torso_damage_right");
	
	tpVisualObject->PlayCycle(tRatAnimations.tNormal.tGlobal.tpIdle);
}

void CAI_Rat::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	CMotionDef*	tpLegsAnimation=0;
	CMotionDef*	tpGlobalAnimation=0;

	if (iHealth <= 0) {
		switch (m_cBodyState) {
			case BODY_STATE_STAND : {
				for (int i=0 ;i<2; i++)
					if (tRatAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
						break;
					}
				if (!tpGlobalAnimation)
					tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)];
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
		if (speed<0.2f) {
			switch (m_cBodyState) {
				case BODY_STATE_STAND : {
					if ((fabsf(r_torso_target.yaw - r_torso_current.yaw) > TORSO_ANGLE_DELTA) && (fabsf(PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw)) > TORSO_ANGLE_DELTA))
						tpLegsAnimation = tRatAnimations.tNormal.tLegs.tpTurn;
					break;
				}
				case BODY_STATE_CROUCH : {
					break;
				}
				case BODY_STATE_LIE : {
					break;
				}
			}
			switch (eCurrentState) {
				case aiRatAttackFire : {
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							for (int i=0 ;i<2; i++)
								if (tRatAnimations.tNormal.tGlobal.tpaAttack[i] == m_tpCurrentGlobalAnimation) {
									tpGlobalAnimation = m_tpCurrentGlobalAnimation;
									break;
								}
							
							if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
								tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaAttack[::Random.randI(0,2)];
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
							tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpIdle;
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
		else {
			//Msg("moving...");
			Fvector view = _view; view.y=0; view.normalize_safe();
			Fvector move = _move; move.y=0; move.normalize_safe();
			float	dot  = view.dotproduct(move);
			
			SAnimState* AState = 0;
			switch (m_cBodyState) {
				case BODY_STATE_STAND : {
					AState = &tRatAnimations.tNormal.tGlobal.tWalk;
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
				case aiRatAttackFire : {
					switch (m_cBodyState) {
						case BODY_STATE_STAND : {
							for (int i=0 ;i<2; i++)
								if (tRatAnimations.tNormal.tGlobal.tpaAttack[i] == m_tpCurrentGlobalAnimation) {
									tpGlobalAnimation = m_tpCurrentGlobalAnimation;
									break;
								}
							
							if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
								tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaAttack[::Random.randI(0,2)];
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
	
	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
		if (tpGlobalAnimation) {
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
		}
	}
}