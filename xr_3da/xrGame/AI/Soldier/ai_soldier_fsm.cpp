////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\xr_trims.h"

#define MAX_PATROL_DISTANCE				6.f
#define MIN_PATROL_DISTANCE				1.f
#define	AMMO_NEED_RELOAD				6
#define MIN_COVER_MOVE					120
#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
#define EYE_WEAPON_DELTA				(0*PI/30.f)
#define TORSO_START_SPEED				PI_DIV_4
#define DISTANCE_TO_REACT				2.14f
#define RECHARGE_MEDIAN					(1.f/3.f)
#define RECHARGE_EPSILON				(0.f/6.f)

#define SPECIAL_SQUAD					6
/**/
void CAI_Soldier::Test()
{
	WRITE_TO_LOG("Temporary test state");

	CKinematics* tpVisualObject = PKinematics(pVisual);

	if (Level().iGetKeyState(DIK_LSHIFT)) {
		if (Level().iGetKeyState(DIK_Q))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[0]);
		else if (Level().iGetKeyState(DIK_W))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[1]);
		else if (Level().iGetKeyState(DIK_E))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[2]);
		else if (Level().iGetKeyState(DIK_R))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[3]);
		else if (Level().iGetKeyState(DIK_T))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0]);
		else if (Level().iGetKeyState(DIK_Y))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[1]);
		else if (Level().iGetKeyState(DIK_U))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[2]);
		else if (Level().iGetKeyState(DIK_I))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[0]);
		else if (Level().iGetKeyState(DIK_O))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[1]);
		else if (Level().iGetKeyState(DIK_P))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[2]);
		else if (Level().iGetKeyState(DIK_A))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpWalkLeft);
		else if (Level().iGetKeyState(DIK_S))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpWalkRight);
		else if (Level().iGetKeyState(DIK_D))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpTurnLeft);
		else if (Level().iGetKeyState(DIK_F))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpTurnRight);
		else if (Level().iGetKeyState(DIK_G))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaIdle[1]);
		else if (Level().iGetKeyState(DIK_H))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaLieDown[0]);
		else if (Level().iGetKeyState(DIK_J))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpPointSign);
		else if (Level().iGetKeyState(DIK_K))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[0]);
		else if (Level().iGetKeyState(DIK_L))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[1]);
		else if (Level().iGetKeyState(DIK_Z))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[2]);
		else if (Level().iGetKeyState(DIK_X))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[3]);
		else if (Level().iGetKeyState(DIK_C))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[4]);
		else if (Level().iGetKeyState(DIK_V))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpAim);
		else if (Level().iGetKeyState(DIK_B))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpAttack);
		else if (Level().iGetKeyState(DIK_N))
			tpVisualObject->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageLeft);
		else if (Level().iGetKeyState(DIK_M))
			tpVisualObject->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageRight);
	}
	else
		if (Level().iGetKeyState(DIK_RSHIFT)) {
			if (Level().iGetKeyState(DIK_Q))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tTorso.tpGoAheadSign);
			else if (Level().iGetKeyState(DIK_W))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tTorso.tpRaiseHandSign);
			else if (Level().iGetKeyState(DIK_E))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpReload);
			else if (Level().iGetKeyState(DIK_R))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tLegs.tpJumpBegin);
			else if (Level().iGetKeyState(DIK_T))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tLegs.tpJumpIdle);
			else if (Level().iGetKeyState(DIK_Y))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[0]);
			else if (Level().iGetKeyState(DIK_U))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[1]);
			else if (Level().iGetKeyState(DIK_O))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[0]);
			else if (Level().iGetKeyState(DIK_P))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[1]);
			else if (Level().iGetKeyState(DIK_S))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpDeath);
			else if (Level().iGetKeyState(DIK_D))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpIdle);
			else if (Level().iGetKeyState(DIK_F))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[0]);
			else if (Level().iGetKeyState(DIK_G))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpPointSign);
			else if (Level().iGetKeyState(DIK_H))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft);
			else if (Level().iGetKeyState(DIK_J))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpTurnRight);
			else if (Level().iGetKeyState(DIK_K))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpWalkLeft);
			else if (Level().iGetKeyState(DIK_L))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpWalkRight);
			else if (Level().iGetKeyState(DIK_Z))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpAim);
			else if (Level().iGetKeyState(DIK_X))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpAttack);
			else if (Level().iGetKeyState(DIK_C))
				tpVisualObject->PlayFX(tSoldierAnimations.tCrouch.tTorso.tpDamageLeft);
			else if (Level().iGetKeyState(DIK_V))
				tpVisualObject->PlayFX(tSoldierAnimations.tCrouch.tTorso.tpDamageRight);
			else if (Level().iGetKeyState(DIK_B))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tTorso.tpGoAheadSign);
			else if (Level().iGetKeyState(DIK_N))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tTorso.tpRaiseHandSign);
			else if (Level().iGetKeyState(DIK_M))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpReload);
		}
		else 
			if (Level().iGetKeyState(DIK_LALT)) {
				if (Level().iGetKeyState(DIK_Q))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tLegs.tpJumpBegin);
				else if (Level().iGetKeyState(DIK_W))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tLegs.tpJumpIdle);
				else if (Level().iGetKeyState(DIK_E))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpAttack);
				else if (Level().iGetKeyState(DIK_R))
					tpVisualObject->PlayFX(tSoldierAnimations.tLie.tGlobal.tpDamage);
				else if (Level().iGetKeyState(DIK_T))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpDeath);
				else if (Level().iGetKeyState(DIK_U))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpIdle);
				else if (Level().iGetKeyState(DIK_P))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpReload);
				else if (Level().iGetKeyState(DIK_A))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpStandUp);
				else if (Level().iGetKeyState(DIK_S))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpTurnLeft);
				else if (Level().iGetKeyState(DIK_D))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpTurnRight);
				else if (Level().iGetKeyState(DIK_F))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkBack);
				else if (Level().iGetKeyState(DIK_G))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkForward);
				else if (Level().iGetKeyState(DIK_H))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkLeft);
				else if (Level().iGetKeyState(DIK_J))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkRight);
				else if (Level().iGetKeyState(DIK_K))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[3]);
				else if (Level().iGetKeyState(DIK_L))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[4]);
				else if (Level().iGetKeyState(DIK_Z))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaLieDown[1]);
				else if (Level().iGetKeyState(DIK_X))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaLieDown[2]);
				else if (Level().iGetKeyState(DIK_C))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[1]);
			}
}

void CAI_Soldier::Die()
{
	WRITE_TO_LOG("die");

	if (m_tpSoundBeingPlayed && m_tpSoundBeingPlayed->feedback)
		m_tpSoundBeingPlayed->feedback->Stop();

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	bActive = false;
	bEnabled = false;
}

void CAI_Soldier::OnTurnOver()
{
	WRITE_TO_LOG("turn over");
}

void CAI_Soldier::OnWaitForAnimation()
{
	WRITE_TO_LOG("wait for animation");
}

void CAI_Soldier::OnWaitForTime()
{
	WRITE_TO_LOG("wait for time");
}

void CAI_Soldier::OnRecharge()
{
	WRITE_TO_LOG("recharge");
}

void CAI_Soldier::OnLookingOver()
{
	WRITE_TO_LOG("looking over");
}

void CAI_Soldier::OnPatrolReturnToRoute()
{
	WRITE_TO_LOG("return to route");
}

void CAI_Soldier::OnPatrolRoute()
{
	WRITE_TO_LOG("patrol route");
}

void CAI_Soldier::OnFollowLeaderPatrol()
{
	WRITE_TO_LOG("follow leader patrol");
}

void CAI_Soldier::OnFight()
{
	WRITE_TO_LOG("fight");
}

void CAI_Soldier::OnFightAlone()
{
	WRITE_TO_LOG("fight alone");
}

void CAI_Soldier::OnFightGroup()
{
	WRITE_TO_LOG("fight group");
}

void CAI_Soldier::OnAttackAlone()
{
	WRITE_TO_LOG("attack alone");
}

void CAI_Soldier::OnDefendAlone()
{
	WRITE_TO_LOG("defend alone");
}

void CAI_Soldier::OnPursuitAlone()
{
	WRITE_TO_LOG("pursuit alone");
}

void CAI_Soldier::OnFindAlone()
{
	WRITE_TO_LOG("find alone");
}

void CAI_Soldier::OnRetreatAlone()
{
	WRITE_TO_LOG("retreat alone");
}

void CAI_Soldier::OnHurtAlone()
{
	WRITE_TO_LOG("hurt alone");
}

void CAI_Soldier::OnAttackAloneNonFire()
{
	WRITE_TO_LOG("attack alone non-fire");
}

void CAI_Soldier::OnAttackAloneFire()
{
	WRITE_TO_LOG("attack alone fire");
}

void CAI_Soldier::OnDefendAloneNonFire()
{
	WRITE_TO_LOG("defend alone non-fire");
}

void CAI_Soldier::OnDefendAloneFire()
{
	WRITE_TO_LOG("defend alone fire");
}

void CAI_Soldier::OnPursuitAloneNonFire()
{
	WRITE_TO_LOG("pursuit alone non-fire");
}

void CAI_Soldier::OnPursuitAloneFire()
{
	WRITE_TO_LOG("pursuti alone fire");
}

void CAI_Soldier::OnFindAloneNonFire()
{
	WRITE_TO_LOG("find alone non-fire");
}

void CAI_Soldier::OnFindAloneFire()
{
	WRITE_TO_LOG("find alone fire");
}

void CAI_Soldier::OnRetreatAloneNonFire()
{
	WRITE_TO_LOG("retreat alone none-fire");
}

void CAI_Soldier::OnRetreatAloneFire()
{
	WRITE_TO_LOG("retreat alone fire");
}

void CAI_Soldier::OnRetreatAloneDialog()
{
	WRITE_TO_LOG("retreat alone dialog");
}

void CAI_Soldier::OnAttackAloneNonFireFire()
{
	WRITE_TO_LOG("attack alone non-fire fire");
}

void CAI_Soldier::OnAttackAloneNonFireRun()
{
	WRITE_TO_LOG("attack alone non-fire run");
}

void CAI_Soldier::OnAttackAloneNonFireSteal()
{
	WRITE_TO_LOG("attack alone non-fire steal");
}

void CAI_Soldier::OnAttackAloneNonFireDialog()
{
	WRITE_TO_LOG("attack alone non-fire dialog");
}

void CAI_Soldier::OnAttackAloneFireFire()
{
	WRITE_TO_LOG("attack alone fire fire");
}

void CAI_Soldier::OnAttackAloneFireRun()
{
	WRITE_TO_LOG("attack alone fire run");
}

void CAI_Soldier::OnAttackAloneFireSteal()
{
	WRITE_TO_LOG("attack alone fire steal");
}

void CAI_Soldier::OnAttackAloneFireDialog()
{
	WRITE_TO_LOG("attack alone fire dialog");
}

void CAI_Soldier::OnDefendAloneNonFireFire()
{
	WRITE_TO_LOG("defend alone non-fire fire");
}

void CAI_Soldier::OnDefendAloneNonFireRun()
{
	WRITE_TO_LOG("defend alone non-fire run");
}

void CAI_Soldier::OnDefendAloneNonFireSteal()
{
	WRITE_TO_LOG("defend alone non-fire steal");
}

void CAI_Soldier::OnDefendAloneNonFireDialog()
{
	WRITE_TO_LOG("defend alone non-fire dialog");
}

void CAI_Soldier::OnDefendAloneFireFire()
{
	WRITE_TO_LOG("defend alone fire fire");
}

void CAI_Soldier::OnDefendAloneFireRun()
{
	WRITE_TO_LOG("defend alone fire run");
}

void CAI_Soldier::OnDefendAloneFireSteal()
{
	WRITE_TO_LOG("defend alone fire steal");
}

void CAI_Soldier::OnDefendAloneFireDialog()
{
	WRITE_TO_LOG("defend alone fire dialog");
}

void CAI_Soldier::OnAttackGroup()
{
	WRITE_TO_LOG("attack group");
}

void CAI_Soldier::OnDefendGroup()
{
	WRITE_TO_LOG("defend group");
}

void CAI_Soldier::OnPursuitGroup()
{
	WRITE_TO_LOG("pursuit group");
}

void CAI_Soldier::OnFindGroup()
{
	WRITE_TO_LOG("find group");
}

void CAI_Soldier::OnRetreatGroup()
{
	WRITE_TO_LOG("retreat group");
}

void CAI_Soldier::OnHurtGroup()
{
	WRITE_TO_LOG("hurt group");
}

void CAI_Soldier::OnAttackGroupNonFire()
{
	WRITE_TO_LOG("attack group non-fire");
}

void CAI_Soldier::OnAttackGroupFire()
{
	WRITE_TO_LOG("attack group fire");
}

void CAI_Soldier::OnDefendGroupNonFire()
{
	WRITE_TO_LOG("defend group non-fire");
}

void CAI_Soldier::OnDefendGroupFire()
{
	WRITE_TO_LOG("defend group fire");
}

void CAI_Soldier::OnPursuitGroupNonFire()
{
	WRITE_TO_LOG("pursuit group non-fire");
}

void CAI_Soldier::OnPursuitGroupFire()
{
	WRITE_TO_LOG("pursuit group fire");
}

void CAI_Soldier::OnFindGroupNonFire()
{
	WRITE_TO_LOG("find group non-fire");
}

void CAI_Soldier::OnFindGroupFire()
{
	WRITE_TO_LOG("find group fire");
}

void CAI_Soldier::OnRetreatGroupNonFire()
{
	WRITE_TO_LOG("retreat group non-fire");
}

void CAI_Soldier::OnRetreatGroupFire()
{
	WRITE_TO_LOG("retreat group fire");
}

void CAI_Soldier::OnRetreatGroupDialog()
{
	WRITE_TO_LOG("retreat group dialog");
}

void CAI_Soldier::OnAttackGroupNonFireFire()
{
	WRITE_TO_LOG("attack group non-fire fire");
}

void CAI_Soldier::OnAttackGroupNonFireRun()
{
	WRITE_TO_LOG("attack group non-fire run");
}

void CAI_Soldier::OnAttackGroupNonFireSteal()
{
	WRITE_TO_LOG("attack group non-fire steal");
}

void CAI_Soldier::OnAttackGroupNonFireDialog()
{
	WRITE_TO_LOG("attack group non-fire dialog");
}

void CAI_Soldier::OnAttackGroupFireFire()
{
	WRITE_TO_LOG("attack group fire fire");
}

void CAI_Soldier::OnAttackGroupFireRun()
{
	WRITE_TO_LOG("attack group fire run");
}

void CAI_Soldier::OnAttackGroupFireSteal()
{
	WRITE_TO_LOG("attack group fire steal");
}

void CAI_Soldier::OnAttackGroupFireDialog()
{
	WRITE_TO_LOG("attack group fire dialog");
}

void CAI_Soldier::OnDefendGroupNonFireFire()
{
	WRITE_TO_LOG("defend group non-fire fire");
}

void CAI_Soldier::OnDefendGroupNonFireRun()
{
	WRITE_TO_LOG("defend group non-fire run");
}

void CAI_Soldier::OnDefendGroupNonFireSteal()
{
	WRITE_TO_LOG("defend group non-fire steal");
}

void CAI_Soldier::OnDefendGroupNonFireDialog()
{
	WRITE_TO_LOG("defend group non-fire dialog");
}

void CAI_Soldier::OnDefendGroupFireFire()
{
	WRITE_TO_LOG("defend group fire fire");
}

void CAI_Soldier::OnDefendGroupFireRun()
{
	WRITE_TO_LOG("defend group fire run");
}

void CAI_Soldier::OnDefendGroupFireSteal()
{
	WRITE_TO_LOG("defend group fire steal");
}

void CAI_Soldier::OnDefendGroupFireDialog()
{
	WRITE_TO_LOG("defend group fire dialog");
}

void CAI_Soldier::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiSoldierDie : {
				Die();
				break;
			}
			case aiSoldierTurnOver : {
				OnTurnOver();
				break;
			}
			case aiSoldierWaitForAnimation : {
				OnWaitForAnimation();
				break;
			}
			case aiSoldierWaitForTime : {
				OnWaitForTime();
				break;
			}
			case aiSoldierRecharge : {
				OnRecharge();
				break;
			}
			case aiSoldierLookingOver : {
				OnLookingOver();
				break;
			}
			case aiSoldierPatrolReturnToRoute : {
				OnPatrolReturnToRoute();
				break;
			}
			case aiSoldierPatrolRoute : {
				OnPatrolRoute();
				break;
			}
			case aiSoldierFollowLeaderPatrol : {
				OnFollowLeaderPatrol();
				break;
			}
			case aiSoldierFight : {
				OnFight();
				break;
			}
			case aiSoldierFightAlone : {
				OnFightAlone();
				break;
			}
			case aiSoldierFightGroup : {
				OnFightGroup();
				break;
			}
			case aiSoldierAttackAlone : {
				OnAttackAlone();
				break;
			}
			case aiSoldierDefendAlone : {
				OnDefendAlone();
				break;
			}
			case aiSoldierPursuitAlone : {
				OnPursuitAlone();
				break;
			}
			case aiSoldierFindAlone : {
				OnFindAlone();
				break;
			}
			case aiSoldierRetreatAlone : {
				OnRetreatAlone();
				break;
			}
			case aiSoldierHurtAlone : {
				OnHurtAlone();
				break;
			}
			case aiSoldierAttackAloneNonFire : {
				OnAttackAloneNonFire();
				break;
			}
			case aiSoldierAttackAloneFire : {
				OnAttackAloneFire();
				break;
			}
			case aiSoldierDefendAloneNonFire : {
				OnDefendAloneNonFire();
				break;
			}
			case aiSoldierDefendAloneFire : {
				OnDefendAloneFire();
				break;
			}
			case aiSoldierPursuitAloneNonFire : {
				OnPursuitAloneNonFire();
				break;
			}
			case aiSoldierPursuitAloneFire : {
				OnPursuitAloneFire();
				break;
			}
			case aiSoldierFindAloneNonFire : {
				OnFindAloneNonFire();
				break;
			}
			case aiSoldierFindAloneFire : {
				OnFindAloneFire();
				break;
			}
			case aiSoldierRetreatAloneNonFire : {
				OnRetreatAloneNonFire();
				break;
			}
			case aiSoldierRetreatAloneFire : {
				OnRetreatAloneFire();
				break;
			}
			case aiSoldierRetreatAloneDialog : {
				OnRetreatAloneDialog();
				break;
			}
			case aiSoldierAttackAloneNonFireFire : {
				OnAttackAloneNonFireFire();
				break;
			}
			case aiSoldierAttackAloneNonFireRun : {
				OnAttackAloneNonFireRun();
				break;
			}
			case aiSoldierAttackAloneNonFireSteal : {
				OnAttackAloneNonFireSteal();
				break;
			}
			case aiSoldierAttackAloneNonFireDialog : {
				OnAttackAloneNonFireDialog();
				break;
			}
			case aiSoldierAttackAloneFireFire : {
				OnAttackAloneFireFire();
				break;
			}
			case aiSoldierAttackAloneFireRun : {
				OnAttackAloneFireRun();
				break;
			}
			case aiSoldierAttackAloneFireSteal : {
				OnAttackAloneFireSteal();
				break;
			}
			case aiSoldierAttackAloneFireDialog : {
				OnAttackAloneFireDialog();
				break;
			}
			case aiSoldierDefendAloneNonFireFire : {
				OnDefendAloneNonFireFire();
				break;
			}
			case aiSoldierDefendAloneNonFireRun : {
				OnDefendAloneNonFireRun();
				break;
			}
			case aiSoldierDefendAloneNonFireSteal : {
				OnDefendAloneNonFireSteal();
				break;
			}
			case aiSoldierDefendAloneNonFireDialog : {
				OnDefendAloneNonFireDialog();
				break;
			}
			case aiSoldierDefendAloneFireFire : {
				OnDefendAloneFireFire();
				break;
			}
			case aiSoldierDefendAloneFireRun : {
				OnDefendAloneFireRun();
				break;
			}
			case aiSoldierDefendAloneFireSteal : {
				OnDefendAloneFireSteal();
				break;
			}
			case aiSoldierDefendAloneFireDialog : {
				OnDefendAloneFireDialog();
				break;
			}
			case aiSoldierAttackGroup : {
				OnAttackGroup();
				break;
			}
			case aiSoldierDefendGroup : {
				OnDefendGroup();
				break;
			}
			case aiSoldierPursuitGroup : {
				OnPursuitGroup();
				break;
			}
			case aiSoldierFindGroup : {
				OnFindGroup();
				break;
			}
			case aiSoldierRetreatGroup : {
				OnRetreatGroup();
				break;
			}
			case aiSoldierHurtGroup : {
				OnHurtGroup();
				break;
			}
			case aiSoldierAttackGroupNonFire : {
				OnAttackGroupNonFire();
				break;
			}
			case aiSoldierAttackGroupFire : {
				OnAttackGroupFire();
				break;
			}
			case aiSoldierDefendGroupNonFire : {
				OnDefendGroupNonFire();
				break;
			}
			case aiSoldierDefendGroupFire : {
				OnDefendGroupFire();
				break;
			}
			case aiSoldierPursuitGroupNonFire : {
				OnPursuitGroupNonFire();
				break;
			}
			case aiSoldierPursuitGroupFire : {
				OnPursuitGroupFire();
				break;
			}
			case aiSoldierFindGroupNonFire : {
				OnFindGroupNonFire();
				break;
			}
			case aiSoldierFindGroupFire : {
				OnFindGroupFire();
				break;
			}
			case aiSoldierRetreatGroupNonFire : {
				OnRetreatGroupNonFire();
				break;
			}
			case aiSoldierRetreatGroupFire : {
				OnRetreatGroupFire();
				break;
			}
			case aiSoldierRetreatGroupDialog : {
				OnRetreatGroupDialog();
				break;
			}
			case aiSoldierAttackGroupNonFireFire : {
				OnAttackGroupNonFireFire();
				break;
			}
			case aiSoldierAttackGroupNonFireRun : {
				OnAttackGroupNonFireRun();
				break;
			}
			case aiSoldierAttackGroupNonFireSteal : {
				OnAttackGroupNonFireSteal();
				break;
			}
			case aiSoldierAttackGroupNonFireDialog : {
				OnAttackGroupNonFireDialog();
				break;
			}
			case aiSoldierAttackGroupFireFire : {
				OnAttackGroupFireFire();
				break;
			}
			case aiSoldierAttackGroupFireRun : {
				OnAttackGroupFireRun();
				break;
			}
			case aiSoldierAttackGroupFireSteal : {
				OnAttackGroupFireSteal();
				break;
			}
			case aiSoldierAttackGroupFireDialog : {
				OnAttackGroupFireDialog();
				break;
			}
			case aiSoldierDefendGroupNonFireFire : {
				OnDefendGroupNonFireFire();
				break;
			}
			case aiSoldierDefendGroupNonFireRun : {
				OnDefendGroupNonFireRun();
				break;
			}
			case aiSoldierDefendGroupNonFireSteal : {
				OnDefendGroupNonFireSteal();
				break;
			}
			case aiSoldierDefendGroupNonFireDialog : {
				OnDefendGroupNonFireDialog();
				break;
			}
			case aiSoldierDefendGroupFireFire : {
				OnDefendGroupFireFire();
				break;
			}
			case aiSoldierDefendGroupFireRun : {
				OnDefendGroupFireRun();
				break;
			}
			case aiSoldierDefendGroupFireSteal : {
				OnDefendGroupFireSteal();
				break;
			}
			case aiSoldierDefendGroupFireDialog : {
				OnDefendGroupFireDialog();
				break;
			}
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
	m_dwLastUpdate = Level().timeServer() - m_fTimeUpdateDelta;
}
