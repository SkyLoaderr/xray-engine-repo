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
	// attack
	::Sound->Create(m_tpaSoundAttack[0],TRUE,"monsters\\zombie\\attack\\zm1att_1",0,SOUND_TYPE_MONSTER_ATTACKING_ANIMAL);
	::Sound->Create(m_tpaSoundAttack[1],TRUE,"monsters\\zombie\\attack\\zm1att_2",0,SOUND_TYPE_MONSTER_ATTACKING_ANIMAL);
	::Sound->Create(m_tpaSoundAttack[2],TRUE,"monsters\\zombie\\attack\\zm1att_3",0,SOUND_TYPE_MONSTER_ATTACKING_ANIMAL);
	::Sound->Create(m_tpaSoundAttack[3],TRUE,"monsters\\zombie\\attack\\zm1att_4",0,SOUND_TYPE_MONSTER_ATTACKING_ANIMAL);

	// death
	::Sound->Create(m_tpaSoundDeath[0],TRUE,"monsters\\zombie\\death\\zm1die_1",0,SOUND_TYPE_MONSTER_DYING_ANIMAL);
	::Sound->Create(m_tpaSoundDeath[1],TRUE,"monsters\\zombie\\death\\zm1die_2",0,SOUND_TYPE_MONSTER_DYING_ANIMAL);
	::Sound->Create(m_tpaSoundDeath[2],TRUE,"monsters\\zombie\\death\\zm1die_3",0,SOUND_TYPE_MONSTER_DYING_ANIMAL);

	// hit
	::Sound->Create(m_tpaSoundHit[0],TRUE,"monsters\\zombie\\hit\\zm2a1__1",0,SOUND_TYPE_MONSTER_INJURING_ANIMAL);
	::Sound->Create(m_tpaSoundHit[1],TRUE,"monsters\\zombie\\hit\\zm2a1__2",0,SOUND_TYPE_MONSTER_INJURING_ANIMAL);
	::Sound->Create(m_tpaSoundHit[2],TRUE,"monsters\\zombie\\hit\\zm2a1__3",0,SOUND_TYPE_MONSTER_INJURING_ANIMAL);

	// idle
	::Sound->Create(m_tpaSoundIdle[0],TRUE,"monsters\\zombie\\idle\\hh1a0mo1",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundIdle[1],TRUE,"monsters\\zombie\\idle\\hh1a0mo2",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundIdle[2],TRUE,"monsters\\zombie\\idle\\hh1a0mo3",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);

	// notice
	::Sound->Create(m_tpaSoundNotice[0],TRUE,"monsters\\zombie\\notice\\hh1to1_1",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);

	// pursuit
	::Sound->Create(m_tpaSoundPursuit[0],TRUE,"monsters\\zombie\\pursuit\\hh1a1__1",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundPursuit[1],TRUE,"monsters\\zombie\\pursuit\\hh1a1__2",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundPursuit[2],TRUE,"monsters\\zombie\\pursuit\\hh1a1__3",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundPursuit[3],TRUE,"monsters\\zombie\\pursuit\\hh1a1__4",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundPursuit[4],TRUE,"monsters\\zombie\\pursuit\\hh1a1__5",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundPursuit[5],TRUE,"monsters\\zombie\\pursuit\\hh1a1__6",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);

	// resurrect
	::Sound->Create(m_tpaSoundResurrect[0],TRUE,"monsters\\zombie\\resurrect\\hh1a3__1",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundResurrect[1],TRUE,"monsters\\zombie\\resurrect\\hh1a3__2",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundResurrect[2],TRUE,"monsters\\zombie\\resurrect\\hh1a3__3",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundResurrect[3],TRUE,"monsters\\zombie\\resurrect\\hh1a3__4",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundResurrect[4],TRUE,"monsters\\zombie\\resurrect\\hh1a3__5",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
	::Sound->Create(m_tpaSoundResurrect[5],TRUE,"monsters\\zombie\\resurrect\\hh1a3__6",0,SOUND_TYPE_MONSTER_TALKING_ANIMAL);
}

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
	
	m_tZombieAnimations.tNormal.tGlobal.tpWalkForwardCSIP = tpVisualObject->ID_Cycle("norm_walk_fwd_1");

	m_tZombieAnimations.tNormal.tGlobal.tpRunForward	 = tpVisualObject->ID_Cycle("norm_run_fwd");
	
	m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[0] = tpVisualObject->ID_Cycle("norm_death_idle_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[1] = tpVisualObject->ID_Cycle("norm_death_idle_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[2] = tpVisualObject->ID_Cycle("norm_death_idle_2");

	m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[0] = tpVisualObject->ID_Cycle("norm_stand_up_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[1] = tpVisualObject->ID_Cycle("norm_stand_up_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[2] = tpVisualObject->ID_Cycle("norm_stand_up_2");

	m_tZombieAnimations.tNormal.tGlobal.tpTurnLeft	 = tpVisualObject->ID_Cycle("norm_turn_ls");
	m_tZombieAnimations.tNormal.tGlobal.tpTurnRight  = tpVisualObject->ID_Cycle("norm_turn_rs");
	
	m_tZombieAnimations.tNormal.tTorso.tpDamageLeft  = tpVisualObject->ID_FX("norm_damage_ls");
	m_tZombieAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_FX("norm_damage_rs");

	tpVisualObject->PlayCycle(m_tZombieAnimations.tNormal.tGlobal.tpaIdle[0]);
}

void CAI_Zombie::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics* tpVisualObject = PKinematics(pVisual);
	CMotionDef*	tpGlobalAnimation = 0;

	if (!g_Alive()) {
		for (int i=0 ;i<3; i++)
			if (m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[i] == m_tpCurrentGlobalAnimation) {
				Msg("Death idle");
				tpGlobalAnimation = m_tpCurrentGlobalAnimation;
				break;
			}
		
		if (!tpGlobalAnimation) {
			for (int i=0 ;i<3; i++)
				if (m_tZombieAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
					tpGlobalAnimation = m_tpCurrentGlobalAnimation;
					if (!m_tpCurrentGlobalBlend->playing) {
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[i];
						Msg("Death idle");
					}
					else
						Msg("Death");
					break;
				}
			if (!tpGlobalAnimation) {
				tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,3)];
				Msg("Death");
			}
		}
	}
	else
		if (eCurrentState == aiZombieResurrect) {
			for (int i=0; i<3; i++)
				if (m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[i] == m_tpCurrentGlobalAnimation) {
					tpGlobalAnimation = m_tpCurrentGlobalAnimation;
					break;
				}
			if (!tpGlobalAnimation) {
				for ( i=0; i<3; i++)
					if (m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[i] == m_tpCurrentGlobalAnimation) {
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[i];
						break;
					}
				
				if (!tpGlobalAnimation)
					for ( i=0; i<3; i++)
						if (m_tZombieAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[i];
							break;
						}
			}
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
							for (int i=0 ;i<2; i++)
								if (m_tZombieAnimations.tNormal.tGlobal.tpaIdle[i] == m_tpCurrentGlobalAnimation) {
									tpGlobalAnimation = m_tpCurrentGlobalAnimation;
									break;
								}
							
							if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaIdle[::Random.randI(0,2)];
						}
						else
							if (_abs(m_fSpeed - m_fAttackSpeed) < EPS_L)
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpRunForward;
							else
								if (_abs(m_fSpeed - m_fMaxSpeed) < EPS_L)
									tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpWalkForwardCSIP;
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
							for (int i=0 ;i<2; i++)
								if (m_tZombieAnimations.tNormal.tGlobal.tpaIdle[i] == m_tpCurrentGlobalAnimation) {
									tpGlobalAnimation = m_tpCurrentGlobalAnimation;
									break;
								}
							
							if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaIdle[::Random.randI(0,2)];
						}
						else
							if (_abs(m_fSpeed - m_fAttackSpeed) < EPS_L)
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpRunForward;
							else
								if (_abs(m_fSpeed - m_fMaxSpeed) < EPS_L)
									tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpWalkForwardCSIP;
								else
									tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tWalk.fwd;
	
	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
		if (tpGlobalAnimation) {
			if (tpGlobalAnimation == m_tZombieAnimations.tNormal.tGlobal.tpRunForward) {
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
				m_tpCurrentGlobalBlend->timeCurrent = ::Random.randF(m_tpCurrentGlobalBlend->timeTotal);
			}
			else
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
		}
	}
}