////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "ai_zombie_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\xr_trims.h"

#define ATTACK_DISTANCE 2.f

void CAI_Zombie::Die()
{
	WRITE_TO_LOG("Dying...");

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction	(dir);
	SelectAnimation		(clTransform.k,dir,AI_Path.fSpeed);

	setEnabled			(false);
}

void CAI_Zombie::Jumping()
{
	if (Movement.Environment() == CMovementControl::peInAir)
		WRITE_TO_LOG("Jumping(air)...")
	else
		if (Movement.Environment() == CMovementControl::peOnGround)
			WRITE_TO_LOG("Jumping(ground)...")
		else
			WRITE_TO_LOG("Jumping(unknown)...")

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	if (m_bStateChanged) {
		m_dwLastRangeSearch = Level().timeServer();
		m_bActionStarted = true;
		m_bJumping = false;
	}
	else
		if ((Movement.Environment() == CMovementControl::peInAir) && (!m_bJumping))
			m_bJumping = true;
		else
			if ((m_bJumping) && (Movement.Environment() == CMovementControl::peOnGround)) {
				m_bJumping = false;
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				eCurrentState = tStateStack.top();
				tStateStack.pop();
			}
}

void CAI_Zombie::FreeHunting()
{
/**/
	WRITE_TO_LOG("Free hunting");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < AI::HIT_JUMP_TIME) && (dwHitTime),aiZombieUnderFire)

//	SelectSound(m_iSoundIndex);
//
//	if ((m_iSoundIndex != -1) && (dwCurTime - tpaDynamicSounds[m_iSoundIndex].dwTime < 300)) {
//		tHitDir.sub(tpaDynamicSounds[m_iSoundIndex].tSavedPosition,vPosition);
//		dwHitTime = tpaDynamicSounds[m_iSoundIndex].dwTime;
//		SWITCH_TO_NEW_STATE(aiZombieUnderFire);
//	}
	
	if (ps_Size() > 1)
		if ((m_bStateChanged) || ((ps_Element(ps_Size() - 1).dwTime - ps_Element(ps_Size() - 2).dwTime < 500)/** && (ps_Element(ps_Size() - 1).vPosition.distance_to(vPosition) < .1f)/**/))
			;//tSavedEnemyPosition.sub(ps_Element(ps_Size() - 1).vPosition,ps_Element(ps_Size() - 2).vPosition);
		else {
			//Fvector tDistance;
			//tDistance.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
//			tSavedEnemyPosition.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
			tSavedEnemyPosition.set(::Random.randF(-1,1),0,::Random.randF(-1,1));
//			if (tDistance.magnitude() < .05f)
//				tSavedEnemyPosition.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
//			else
//				tSavedEnemyPosition.sub(ps_Element(ps_Size() - 1).vPosition,ps_Element(ps_Size() - 2).vPosition);
			//tSavedEnemyPosition.set(::Random.randF(0,1),0,::Random.randF(0,1));
		}
	else {
		tSavedEnemyPosition.set(::Random.randF(-1,1),0,::Random.randF(-1,1));
	}

	tSavedEnemyPosition.normalize();
	tSavedEnemyPosition.mul(1000.f);
	tSavedEnemyPosition.add(vPosition);

	GoToPointViaSubnodes(tSavedEnemyPosition);

	if (!m_bStateChanged)
		SetDirectionLook();

	CHECK_IF_SWITCH_TO_NEW_STATE(Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw, PI_DIV_6),aiZombieTurnOver);

	vfStopFire();

//	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
//		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
//			m_tpSoundBeingPlayed = 0;
//			m_dwLastVoiceTalk = dwCurTime;
//		}
//		if ((dwCurTime - m_dwLastSoundRefresh > m_fVoiceRefreshRate) && ((dwCurTime - m_dwLastVoiceTalk > m_fMaxVoiceIinterval) || ((dwCurTime - m_dwLastVoiceTalk > m_fMinVoiceIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastVoiceTalk - m_fMinVoiceIinterval)/(m_fMaxVoiceIinterval - m_fMinVoiceIinterval))))) {
//			m_dwLastSoundRefresh = dwCurTime;
//			// Play hit-sound
//			m_tpSoundBeingPlayed = &(sndVoices[Random.randI(SND_VOICE_COUNT)]);
//			
//			if (m_tpSoundBeingPlayed->feedback)			
//				return;
//
//			//pSounds->PlayAtPos(*m_tpSoundBeingPlayed,this,vPosition);
//		}
//	}

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Zombie::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
//	DWORD dwCurTime = Level().timeServer();
	
	if (!(Enemy.Enemy)) {
		tHitDir.sub(vPosition,tSavedEnemyPosition);
		vfNormalizeSafe(tHitDir);
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiZombiePursuit);
	}
		
	CHECK_IF_GO_TO_NEW_STATE((Enemy.Enemy->Position().distance_to(vPosition) > ATTACK_DISTANCE),aiZombieAttackRun)

	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(fabsf(r_torso_current.yaw - sTemp.yaw) > 2*PI_DIV_6,aiZombieAttackRun)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	GoToPointViaSubnodes(Enemy.Enemy->Position());
	
	vfSaveEnemy();

	vfAimAtEnemy();

	CHECK_IF_SWITCH_TO_NEW_STATE(!((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))),aiZombieTurnOver);

	vfSetFire(true,Group);

	vfSetMovementType(m_cBodyState,2*m_fMaxSpeed);
}

void CAI_Zombie::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
//	DWORD dwCurTime = Level().timeServer();
	
	if (!(Enemy.Enemy)) {
		tHitDir.sub(vPosition,tSavedEnemyPosition);
		vfNormalizeSafe(tHitDir);
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiZombiePursuit);
	}
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE((fabsf(r_torso_current.yaw - sTemp.yaw) < 2*PI_DIV_6) && (tDistance.magnitude() <= ATTACK_DISTANCE),aiZombieAttackFire);

	INIT_SQUAD_AND_LEADER;
	
	GoToPointViaSubnodes(Enemy.Enemy->Position());
	
	vfSaveEnemy();
	
	vfAimAtEnemy();
	
	CHECK_IF_SWITCH_TO_NEW_STATE(!((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))),aiZombieTurnOver);
	
	vfSetFire(false,Group);

	if (vPosition.distance_to(tSavedEnemyPosition) < 2.5f)
		vfSetMovementType(m_cBodyState,2.f*m_fMaxSpeed);
	else
		vfSetMovementType(m_cBodyState,m_fMaxSpeed);
}

void CAI_Zombie::TurnOver()
{
	WRITE_TO_LOG("Turning over...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
		
//	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))) {
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		m_ePreviousState = tStateStack.top();
		GO_TO_PREV_STATE
	}

	vfSetFire(false,Group);

	if (!AI_Path.TravelPath.empty())
		AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
	else
		AI_Path.TravelStart = 0;
	
	vfSetMovementType(m_cBodyState,m_fCurSpeed);

	//r_torso_speed = PI_DIV_2/1;
	//r_spine_speed = PI_DIV_2/1;
	//q_look.o_look_speed = PI_DIV_2/1;
}

void CAI_Zombie::UnderFire()
{
	WRITE_TO_LOG("UnderFire");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
	SetDirectionLook();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfSetFire(false,Group);

	vfSetMovementType(m_cBodyState,0);

	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
//	DWORD dwCurTime = Level().timeServer();
	
	mk_rotation(tHitDir,r_torso_target);

	CHECK_IF_SWITCH_TO_NEW_STATE(!((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))),aiZombieTurnOver);
	
	dwHitTime = 0;
	GO_TO_NEW_STATE(aiZombiePursuit)
}

void CAI_Zombie::Pursuit()
{
	WRITE_TO_LOG("Pursuit");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < AI::HIT_JUMP_TIME) && (dwHitTime),aiZombieUnderFire)
	
	SelectSound(m_iSoundIndex);

	if ((m_iSoundIndex != -1) && (dwCurTime - tpaDynamicSounds[m_iSoundIndex].dwTime < 300)) {
		tHitDir.sub(tpaDynamicSounds[m_iSoundIndex].tSavedPosition,vPosition);
		dwHitTime = tpaDynamicSounds[m_iSoundIndex].dwTime;
		SWITCH_TO_NEW_STATE(aiZombieUnderFire);
	}
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorFreeHunting,Squad,Leader);

	if (!AI_Path.fSpeed)
		if (m_bStateChanged) {
			SelectorFreeHunting.m_tDirection = tHitDir;
			SelectorFreeHunting.m_tDirection.normalize_safe();
			SelectorFreeHunting.m_tDirection.invert();
			vfSearchForBetterPosition(SelectorFreeHunting,Squad,Leader);
			vfBuildPathToDestinationPoint(0);
		}
		else {
			GO_TO_PREV_STATE;
		}

	if (!m_bStateChanged)
		SetDirectionLook();

	CHECK_IF_SWITCH_TO_NEW_STATE(!((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))),aiZombieTurnOver);

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Zombie::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		/**/
		switch(eCurrentState) {
			case aiZombieAttackFire : {
				AttackFire();
				break;
			}
			case aiZombieAttackRun : {
				AttackRun();
				break;
			}
			case aiZombieDie : {
				Die();
				break;
			}
			case aiZombieJumping :  {
				Jumping();
				break;
			}
			case aiZombieFreeHunting : {
				FreeHunting();
				break;
			}
			case aiZombieTurnOver : {
				TurnOver();
				break;
			}
			case aiZombieUnderFire : {
				UnderFire();
				break;
			}
			case aiZombiePursuit : {
				Pursuit();
				break;
			}
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
		/**/
		//break;
	}
	while (!bStopThinking);
}
