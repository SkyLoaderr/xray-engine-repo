////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_animations.cpp
//	Created 	: 21.06.2002
//  Modified 	: 06.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"

// sounds
void CAI_Zombie::vfLoadSounds()
{
	pSounds->Create(m_tpaSoundHit[0],TRUE,"monsters\\rat\\rat_1",0,SOUND_TYPE_MONSTER_INJURING_ANIMAL);
	pSounds->Create(m_tpaSoundDie[0],TRUE,"monsters\\rat\\rat_2",0,SOUND_TYPE_MONSTER_DYING_ANIMAL);
	pSounds->Create(m_tpaSoundAttack[0],TRUE,"monsters\\rat\\rat_2",0,SOUND_TYPE_MONSTER_ATTACKING_ANIMAL);
	pSounds->Create(m_tpaSoundVoice[0],TRUE,"monsters\\rat\\rat_1",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	pSounds->Create(m_tpaSoundVoice[1],TRUE,"monsters\\rat\\rat_2",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
}

// animations
void CAI_Zombie::vfLoadAnimations()
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	
	// loading normal animations
	m_tZombieAnimations.tNormal.tGlobal.tpaDeath[0]  = tpVisualObject->ID_Cycle("norm_death_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeath[1]  = tpVisualObject->ID_Cycle("norm_death_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeath[2]  = tpVisualObject->ID_Cycle("norm_death_2");
	
	m_tZombieAnimations.tNormal.tGlobal.tpaAttack[0] = tpVisualObject->ID_Cycle("norm_attack_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaAttack[1] = tpVisualObject->ID_Cycle("norm_attack_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaAttack[2] = tpVisualObject->ID_Cycle("norm_attack_2");
	
	m_tZombieAnimations.tNormal.tGlobal.tpaIdle[0]	 = tpVisualObject->ID_Cycle("norm_idle_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaIdle[1]   = tpVisualObject->ID_Cycle("norm_idle_1");
	
	m_tZombieAnimations.tNormal.tGlobal.tWalk.fwd	 = tpVisualObject->ID_Cycle("norm_walk_fwd_0");
	m_tZombieAnimations.tNormal.tGlobal.tWalk.back	 = tpVisualObject->ID_Cycle("norm_walk_back");
	m_tZombieAnimations.tNormal.tGlobal.tWalk.ls	 = tpVisualObject->ID_Cycle("norm_walk_ls");
	m_tZombieAnimations.tNormal.tGlobal.tWalk.rs	 = tpVisualObject->ID_Cycle("norm_walk_rs");
	
	m_tZombieAnimations.tNormal.tGlobal.tWalkForwardCSIP = tpVisualObject->ID_Cycle("norm_walk_fwd_1");

	m_tZombieAnimations.tNormal.tGlobal.tRunForward	 = tpVisualObject->ID_Cycle("norm_run_fwd");
	m_tZombieAnimations.tNormal.tGlobal.tStandUp	 = tpVisualObject->ID_Cycle("norm_stand_up");

	m_tZombieAnimations.tNormal.tGlobal.tpTurnLeft	 = tpVisualObject->ID_Cycle("norm_turn_ls");
	m_tZombieAnimations.tNormal.tGlobal.tpTurnRight  = tpVisualObject->ID_Cycle("norm_turn_rs");
	
	m_tZombieAnimations.tNormal.tTorso.tpDamageLeft  = tpVisualObject->ID_FX("norm_damage_ls");
	m_tZombieAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_FX("norm_damage_rs");

	tpVisualObject->PlayCycle(m_tZombieAnimations.tNormal.tGlobal.tpaIdle[0]);
}

void CAI_Zombie::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	CMotionDef*	tpGlobalAnimation=0;

	if (fHealth <= 0) {
		for (int i=0 ;i<2; i++)
			if (m_tZombieAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
				tpGlobalAnimation = m_tpCurrentGlobalAnimation;
				break;
			}
		if (!tpGlobalAnimation)
			if (m_tpCurrentGlobalAnimation == m_tZombieAnimations.tNormal.tGlobal.tpaIdle[1])
				tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaDeath[0];
			else
				tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)];
	}
	else
		if (m_bFiring) {
			for (int i=0 ;i<3; i++)
				if (m_tZombieAnimations.tNormal.tGlobal.tpaAttack[i] == m_tpCurrentGlobalAnimation) {
					tpGlobalAnimation = m_tpCurrentGlobalAnimation;
					break;
				}
			
			if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
				tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaAttack[::Random.randI(0,3)];
			tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaAttack[2];
		}
		else
			if (_abs(r_torso_target.yaw - r_torso_current.yaw) <= PI)
				if (_abs(r_torso_target.yaw - r_torso_current.yaw) >= EPS_L)
					if (r_torso_target.yaw - r_torso_current.yaw >= 0)
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnRight;
					else
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnLeft;
				else
					if (m_fSpeed < 0.2f) {
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaIdle[0];
					}
					else
						if (_abs(m_fSpeed - m_fAttackSpeed) < EPS_L)
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tRunForward;
						else
							if (_abs(m_fSpeed - m_fMaxSpeed) < EPS_L)
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tWalkForwardCSIP;
							else
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tWalk.fwd;
			else
				if (PI_MUL_2 - _abs(r_torso_target.yaw - r_torso_current.yaw) >= EPS_L)
					if (r_torso_target.yaw > r_torso_current.yaw)
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnLeft;
					else
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnRight;
				else
					if (m_fSpeed < 0.2f) {
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaIdle[0];
					}
					else
						if (_abs(m_fSpeed - m_fAttackSpeed) < EPS_L)
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tRunForward;
						else
							if (_abs(m_fSpeed - m_fMaxSpeed) < EPS_L)
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tWalkForwardCSIP;
							else
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tWalk.fwd;
	
	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
		if (tpGlobalAnimation) {
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
		}
	}
}