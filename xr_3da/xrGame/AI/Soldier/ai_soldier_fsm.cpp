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
#define TORSO_START_SPEED				PI_DIV_2
#define TORSO_SLOW_SPEED				PI_DIV_4
#define TORSO_FAST_SPEED				PI
#define DISTANCE_TO_REACT				2.14f
#define RECHARGE_MEDIAN					(1.f/3.f)
#define RECHARGE_EPSILON				(0.f/6.f)
#define SPECIAL_SQUAD					6
//#define AMMO_NEED_RELOAD				6
//#define MIN_COVER_MOVE					120
//#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
//#define EYE_WEAPON_DELTA				(0*PI/30.f)

void CAI_Soldier::OnTurnOver()
{
	WRITE_TO_LOG("turn over");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	CHECK_IF_SWITCH_TO_NEW_STATE(bfCheckForDanger(),aiSoldierFight)

	if ((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))) {
		m_ePreviousState = tStateStack.top();
		GO_TO_PREV_STATE
	}

	q_look.o_look_speed = r_spine_speed = r_torso_speed = TORSO_START_SPEED;

	vfStopFire();
	
	vfSetMovementType(WALK_NO);
}

void CAI_Soldier::OnWaitForAnimation()
{
	WRITE_TO_LOG("wait for animation");

	vfSetMovementType(WALK_NO);

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	CHECK_IF_SWITCH_TO_NEW_STATE(bfCheckForDanger(),aiSoldierFight)

	if (m_bStateChanged)
		m_bActionStarted = true;
		
	CHECK_IF_GO_TO_PREV_STATE(!m_bActionStarted);
}

void CAI_Soldier::OnWaitForTime()
{
	WRITE_TO_LOG("wait for time");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	CHECK_IF_SWITCH_TO_NEW_STATE(bfCheckForDanger(),aiSoldierFight)

	if (m_bStateChanged)
		m_dwLastRangeSearch = Level().timeServer();
		
	CHECK_IF_GO_TO_PREV_STATE(Level().timeServer() - m_dwLastRangeSearch > m_dwTimeBeingWaited);
}

void CAI_Soldier::OnRecharge()
{
	WRITE_TO_LOG("recharge");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	CHECK_IF_GO_TO_PREV_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == Weapons->ActiveWeapon()->GetAmmoMagSize()));
	
	CHECK_IF_GO_TO_PREV_STATE((Weapons->ActiveWeapon()) && (!(Weapons->ActiveWeapon()->GetAmmoCurrent())));
	
	vfStopFire();
	
	if (Weapons->ActiveWeapon())
		Weapons->ActiveWeapon()->Reload();

	if (m_cBodyState != BODY_STATE_STAND)
        vfSetMovementType(m_cMovementType);
	else {
		Squat();
		vfSetMovementType(m_cMovementType);
	}
}

void CAI_Soldier::OnLookingOver()
{
	WRITE_TO_LOG("looking over");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((g_Squad() != SPECIAL_SQUAD) && bfCheckForDanger(),aiSoldierFight)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	AI_Path.TravelPath.clear();
	AI_Path.TravelStart = 0;

	if (m_tpaPatrolPoints.size() > 1)
		CHECK_IF_SWITCH_TO_NEW_STATE(this == Leader,aiSoldierPatrolRoute)
	else
		SWITCH_TO_NEW_STATE(aiSoldierFollowLeaderPatrol);

	StandUp();
	
	SetDirectionLook();
	vfStopFire();
	vfSetMovementType(WALK_NO);
	r_torso_speed = TORSO_SLOW_SPEED;
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
