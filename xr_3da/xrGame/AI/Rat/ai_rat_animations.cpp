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
	
	tRatAnimations.tNormal.tGlobal.tpaAttack[0] = tpVisualObject->ID_Cycle_Safe("attack_1");
	tRatAnimations.tNormal.tGlobal.tpaAttack[1] = tpVisualObject->ID_Cycle_Safe("attack_2");
	tRatAnimations.tNormal.tGlobal.tpaAttack[1] = tpVisualObject->ID_Cycle_Safe("attack_3");
	
	tRatAnimations.tNormal.tGlobal.tpaIdle[0] = tpVisualObject->ID_Cycle_Safe("norm_idle_1");
	tRatAnimations.tNormal.tGlobal.tpaIdle[1] = tpVisualObject->ID_Cycle_Safe("norm_idle_2");
	
	tRatAnimations.tNormal.tGlobal.tpTurnLeft = tpVisualObject->ID_Cycle_Safe("norm_turn_ls");
	tRatAnimations.tNormal.tGlobal.tpTurnRight = tpVisualObject->ID_Cycle_Safe("norm_turn_rs");
	
	tRatAnimations.tNormal.tGlobal.tWalk.Create(tpVisualObject, "norm_walk");
	
	tRatAnimations.tNormal.tGlobal.tRun.Create(tpVisualObject, "norm_run");
	
	tpVisualObject->PlayCycle(tRatAnimations.tNormal.tGlobal.tpaIdle[0]);
}

void CAI_Rat::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	CMotionDef*	tpGlobalAnimation=0;

	if (iHealth <= 0) {
		for (int i=0 ;i<2; i++)
			if (tRatAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
				tpGlobalAnimation = m_tpCurrentGlobalAnimation;
				break;
			}
		if (!tpGlobalAnimation)
			tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)];
	}
	else
		if (m_bFiring) {
			for (int i=0 ;i<3; i++)
				if (tRatAnimations.tNormal.tGlobal.tpaAttack[i] == m_tpCurrentGlobalAnimation) {
					tpGlobalAnimation = m_tpCurrentGlobalAnimation;
					break;
				}
			
			if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
				tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaAttack[::Random.randI(0,3)];
			
			tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaAttack[0];
		}
		else
			if ((r_torso_target.yaw*r_torso_current.yaw >= 0) || (fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI))
				if (fabsf(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
					if (r_torso_target.yaw - r_torso_current.yaw >= 0)
						tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpTurnRight;
					else
						tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpTurnLeft;
				else
					if (speed < 0.2f)
						tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaIdle[0];
					else
						if (fabsf(m_fCurSpeed - m_fMaxSpeed) < EPS_L) 
							tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tRun.fwd;
						else
							tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tWalk.fwd;
			else
				if (PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA)
					if (r_torso_target.yaw > r_torso_current.yaw)
						tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpTurnLeft;
					else
						tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpTurnRight;
				else
					if (speed < 0.2f)
						tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tpaIdle[0];
					else
						if (fabsf(m_fCurSpeed - m_fMaxSpeed) < EPS_L) 
							tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tRun.fwd;
						else
							tpGlobalAnimation = tRatAnimations.tNormal.tGlobal.tWalk.fwd;
	
	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
		if (tpGlobalAnimation) {
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
		}
	}
}