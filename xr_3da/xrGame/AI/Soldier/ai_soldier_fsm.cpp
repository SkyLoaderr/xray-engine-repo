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

#define ATTACK_NON_FIRE_FIRE_DISTANCE	5.f
#define ATTACK_FIRE_FIRE_DISTANCE		15.f
//#define AMMO_NEED_RELOAD				6
//#define MIN_COVER_MOVE					120
//#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
//#define EYE_WEAPON_DELTA				(0*PI/30.f)

void CAI_Soldier::OnFight()
{
	WRITE_TO_LOG("fight");
	
	switch (tfUpdateActionType()) {
		case ACTION_TYPE_GROUP  : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierFightGroup);
		case ACTION_TYPE_ALONE  : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierFightAlone);
		default				    : GO_TO_PREV_STATE_THIS_UPDATE;
	}
}

void CAI_Soldier::OnFightAlone()
{
	WRITE_TO_LOG("fight alone");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionTypeChanged());

	switch (tfUpdateFightTypeAlone()) {
		case FIGHT_TYPE_ATTACK  : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierAttackAlone);
		case FIGHT_TYPE_DEFEND  : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierDefendAlone);
		case FIGHT_TYPE_RETREAT : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierRetreatAlone);
		case FIGHT_TYPE_PURSUIT : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierPursuitAlone);
		case FIGHT_TYPE_FIND	: SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierFindAlone);
		case FIGHT_TYPE_HURT	: SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierHurtAlone);
	}
}

void CAI_Soldier::OnFightGroup()
{
	WRITE_TO_LOG("fight group");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionTypeChanged());

	switch (tfUpdateFightTypeGroup()) {
		case FIGHT_TYPE_ATTACK  : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierAttackGroup);
		case FIGHT_TYPE_DEFEND  : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierDefendGroup);
		case FIGHT_TYPE_RETREAT : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierRetreatGroup);
		case FIGHT_TYPE_PURSUIT : SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierPursuitGroup);
		case FIGHT_TYPE_FIND	: SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierFindGroup);
		case FIGHT_TYPE_HURT	: SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierHurtGroup);
	}
}

void CAI_Soldier::OnAttackAlone()
{
	WRITE_TO_LOG("attack alone");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	getGroup()->m_tpaSuspiciousNodes.clear();
	if ((Enemy.Enemy && bfFireEnemy(Enemy.Enemy)) || (tSavedEnemy && bfFireEnemy(tSavedEnemy)))
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierAttackAloneFire)
	else
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierAttackAloneNonFire)
}

void CAI_Soldier::OnDefendAlone()
{
	WRITE_TO_LOG("defend alone");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	getGroup()->m_tpaSuspiciousNodes.clear();
	if ((Enemy.Enemy && bfFireEnemy(Enemy.Enemy)) || (tSavedEnemy && bfFireEnemy(tSavedEnemy)))
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierDefendAloneFire)
	else
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierDefendAloneNonFire)
}

void CAI_Soldier::OnPursuitAlone()
{
	WRITE_TO_LOG("pursuit alone");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	getGroup()->m_tpaSuspiciousNodes.clear();
	if ((Enemy.Enemy && bfFireEnemy(Enemy.Enemy)) || (tSavedEnemy && bfFireEnemy(tSavedEnemy)))
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierPursuitAloneFire)
	else
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierPursuitAloneNonFire)
}

void CAI_Soldier::OnFindAlone()
{
	WRITE_TO_LOG("find alone");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	if ((Enemy.Enemy && bfFireEnemy(Enemy.Enemy)) || (tSavedEnemy && bfFireEnemy(tSavedEnemy)))
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierFindAloneFire)
	else
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierFindAloneNonFire)
}

void CAI_Soldier::OnRetreatAlone()
{
	WRITE_TO_LOG("retreat alone");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	getGroup()->m_tpaSuspiciousNodes.clear();
	if ((Enemy.Enemy && bfFireEnemy(Enemy.Enemy)) || (tSavedEnemy && bfFireEnemy(tSavedEnemy)))
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierRetreatAloneFire)
	else
		SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierRetreatAloneNonFire)
}

void CAI_Soldier::OnHurtAlone()
{
	WRITE_TO_LOG("hurt alone");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfAmIDead());

	m_dwLastRangeSearch = dwHitTime;

	//AI_Path.TravelPath.clear();

	DWORD dwCurTime = m_dwCurrentUpdate;

	vfStopFire();

	m_dwLastRangeSearch = dwCurTime;
	
	tWatchDirection.sub(tHitPosition,vPosition);
	float fDistance = tWatchDirection.magnitude();
	mk_rotation(tWatchDirection,r_torso_target);
		
	r_torso_speed = 1*PI_DIV_2;
	
	if ((tStateList.size() > 1) && (tStateList[tStateList.size() - 2].eState != aiSoldierRetreatAloneFire) && (AI_Path.fSpeed > EPS_L)) {
		if (m_cBodyState != BODY_STATE_LIE) {
			if (m_cBodyState == BODY_STATE_STAND)
				m_tpAnimationBeingWaited = tSoldierAnimations.tNormal.tGlobal.tpaLieDown[1];
			else
				m_tpAnimationBeingWaited = tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[1];
			Lie();
			SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierWaitForAnimation);
		}
		
		r_torso_speed = TORSO_START_SPEED;
		r_torso_target.yaw = r_torso_current.yaw;

		if (fabsf(r_torso_target.yaw - r_torso_current.yaw) >= PI/30)
			return;

		vfSetMovementType(WALK_NO);
	}

	dwHitTime = DWORD(-1);
	GO_TO_PREV_STATE
}

void CAI_Soldier::OnAttackAloneNonFire()
{
	WRITE_TO_LOG("attack alone non-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(!bfCheckForEntityVisibility(Enemy.Enemy),aiSoldierAttackAloneNonFireSteal)

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfTooFarToEnemy(Enemy.Enemy,ATTACK_NON_FIRE_FIRE_DISTANCE) || bfNeedRecharge(),aiSoldierAttackAloneNonFireRun)
	
	SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierAttackAloneNonFireFire)
}

void CAI_Soldier::OnAttackAloneFire()
{
	WRITE_TO_LOG("attack alone fire");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(!bfCheckForEntityVisibility(Enemy.Enemy) && bfTooFarToEnemy(Enemy.Enemy,ATTACK_FIRE_FIRE_DISTANCE) && !bfNeedRecharge() && !bfNoAmmo(),aiSoldierAttackAloneFireSteal)

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(!bfTooFarToEnemy(Enemy.Enemy,ATTACK_FIRE_FIRE_DISTANCE) && (!bfNeedRecharge()),aiSoldierAttackAloneFireFire)

	SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierAttackAloneFireRun)
}

void CAI_Soldier::OnDefendAloneNonFire()
{
	WRITE_TO_LOG("defend alone non-fire");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(!bfCheckForEntityVisibility(Enemy.Enemy),aiSoldierDefendAloneNonFireSteal)

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfTooFarToEnemy(Enemy.Enemy,ATTACK_NON_FIRE_FIRE_DISTANCE),aiSoldierDefendAloneNonFireRun)
	
	SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierDefendAloneNonFireFire)
}

void CAI_Soldier::OnDefendAloneFire()
{
	WRITE_TO_LOG("defend alone fire");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(!bfCheckForEntityVisibility(Enemy.Enemy),aiSoldierDefendAloneNonFireSteal)

	SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(aiSoldierDefendAloneNonFireFire)

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfTooFarToEnemy(Enemy.Enemy,ATTACK_FIRE_FIRE_DISTANCE),aiSoldierDefendAloneNonFireRun)
}

void CAI_Soldier::OnPursuitAloneNonFire()
{
	WRITE_TO_LOG("pursuit alone non-fire");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());
}

void CAI_Soldier::OnPursuitAloneFire()
{
	WRITE_TO_LOG("pursuti alone fire");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());
}

void CAI_Soldier::OnFindAloneNonFire()
{
	WRITE_TO_LOG("find alone non-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());
}

void CAI_Soldier::OnFindAloneFire()
{
	WRITE_TO_LOG("find alone fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged());

	if (m_bStateChanged)
		m_bActionStarted = false;

	vfStopFire();
	
	CHECK_IF_SWITCH_TO_NEW_STATE(bfNeedRecharge(),aiSoldierRecharge)

	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];

	if (this == Leader) {
 		if (!m_bActionStarted) {
			if (m_bStateChanged) {
				if (!Group.m_tpaSuspiciousNodes.size()) {
					vfFindAllSuspiciousNodes(dwSavedEnemyNodeID,tSavedEnemyPosition,tSavedEnemyPosition,max(20.f,min(8.f*vPosition.distance_to(tSavedEnemyPosition)/4.5f,40.f)),Group);
					vfClasterizeSuspiciousNodes(Group);
				}
			}
			vfInitSelector(SelectorPatrol,Squad,Leader);

			if (AI_Path.bNeedRebuild)
				vfBuildPathToDestinationPoint(0);
			else
				vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);

			if (!bfTooBigDistance(tSavedEnemyPosition,5.f) || AI_Node == tpSavedEnemyNode)
				m_bActionStarted = true;
		}
	}
	if (m_bActionStarted || (this != Leader)) {
		int iIndex = ifFindDynamicObject(tSavedEnemy);
		if (iIndex != -1) {
			if (this == Group.Members[0]) {
				if (AI_Path.fSpeed < EPS_L) {
					for (int i=0, iLastIndex = -1; i<Group.m_tpaSuspiciousNodes.size(); i++)
						if (Group.m_tpaSuspiciousNodes[i].dwNodeID == AI_NodeID) {
							Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
							iLastIndex = i;
							break;
						}
					int Index = ifGetSuspiciousAvailableNode(iLastIndex,Group);
					if (Index != -1) {
						Group.m_tpaSuspiciousNodes[Index].dwSearched = 1;
						Group.m_tpaSuspiciousGroups[Group.m_tpaSuspiciousNodes[Index].dwGroup] = 1;
						AI_Path.DestNode = Group.m_tpaSuspiciousNodes[Index].dwNodeID;
						vfBuildPathToDestinationPoint(0);
					}
					else {
						if (AI_Path.bNeedRebuild) {
							vfInitSelector(SelectorRetreat,Squad,Leader);
							SelectorRetreat.m_tEnemyPosition = tpaDynamicObjects[iIndex].tMySavedPosition;
							SelectorRetreat.m_tpEnemyNode = Level().AI.Node(tpaDynamicObjects[iIndex].dwMyNodeID);
							SelectorRetreat.m_tMyPosition = vPosition;
							SelectorRetreat.m_tpMyNode = AI_Node;
							vfBuildPathToDestinationPoint(0);
						}
						else {
							vfInitSelector(SelectorRetreat,Squad,Leader);
							SelectorRetreat.m_tEnemyPosition = tpaDynamicObjects[iIndex].tMySavedPosition;
							SelectorRetreat.m_tpEnemyNode = Level().AI.Node(tpaDynamicObjects[iIndex].dwMyNodeID);
							SelectorRetreat.m_tMyPosition = vPosition;
							SelectorRetreat.m_tpMyNode = AI_Node;
							vfSearchForBetterPosition(SelectorRetreat,Squad,Leader);
						}
					}
				}
			}
			else
				if (this == Group.Members[1]) {
					if (AI_Path.fSpeed < EPS_L) {
						for (int i=0, iLastIndex = -1; i<Group.m_tpaSuspiciousNodes.size(); i++)
							if (Group.m_tpaSuspiciousNodes[i].dwNodeID == AI_NodeID) {
								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
								iLastIndex = i;
								break;
							}
						int Index = ifGetSuspiciousAvailableNode(iLastIndex,Group);
						if (Index != -1) {
							Group.m_tpaSuspiciousNodes[Index].dwSearched = 1;
							Group.m_tpaSuspiciousGroups[Group.m_tpaSuspiciousNodes[Index].dwGroup] = 1;
							AI_Path.DestNode = Group.m_tpaSuspiciousNodes[Index].dwNodeID;
							vfBuildPathToDestinationPoint(0);
						}
						else {
							if (AI_Path.bNeedRebuild) {
								vfInitSelector(SelectorRetreat,Squad,Leader);
								SelectorRetreat.m_tEnemyPosition = tpaDynamicObjects[iIndex].tMySavedPosition;
								SelectorRetreat.m_tpEnemyNode = Level().AI.Node(tpaDynamicObjects[iIndex].dwMyNodeID);
								SelectorRetreat.m_tMyPosition = vPosition;
								SelectorRetreat.m_tpMyNode = AI_Node;
								vfBuildPathToDestinationPoint(0);
							}
							else {
								vfInitSelector(SelectorRetreat,Squad,Leader);
								SelectorRetreat.m_tEnemyPosition = tpaDynamicObjects[iIndex].tMySavedPosition;
								SelectorRetreat.m_tpEnemyNode = Level().AI.Node(tpaDynamicObjects[iIndex].dwMyNodeID);
								SelectorRetreat.m_tMyPosition = vPosition;
								SelectorRetreat.m_tpMyNode = AI_Node;
								vfSearchForBetterPosition(SelectorRetreat,Squad,Leader);
							}
						}
					}
				}
				else { 
					if (AI_Path.fSpeed < EPS_L) {
						for (int i=0, iLastIndex = -1; i<Group.m_tpaSuspiciousNodes.size(); i++)
							if (Group.m_tpaSuspiciousNodes[i].dwNodeID == AI_NodeID) {
								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
								iLastIndex = i;
								break;
							}
						int Index = ifGetSuspiciousAvailableNode(iLastIndex,Group);
						if (Index != -1) {
							Group.m_tpaSuspiciousNodes[Index].dwSearched = 1;
							Group.m_tpaSuspiciousGroups[Group.m_tpaSuspiciousNodes[Index].dwGroup] = 1;
							AI_Path.DestNode = Group.m_tpaSuspiciousNodes[Index].dwNodeID;
							vfBuildPathToDestinationPoint(0);
						}
						else {
							if (AI_Path.bNeedRebuild) {
								vfInitSelector(SelectorRetreat,Squad,Leader);
								SelectorRetreat.m_tEnemyPosition = tpaDynamicObjects[iIndex].tMySavedPosition;
								SelectorRetreat.m_tpEnemyNode = Level().AI.Node(tpaDynamicObjects[iIndex].dwMyNodeID);
								SelectorRetreat.m_tMyPosition = vPosition;
								SelectorRetreat.m_tpMyNode = AI_Node;
								vfBuildPathToDestinationPoint(0);
							}
							else {
								vfInitSelector(SelectorRetreat,Squad,Leader);
								SelectorRetreat.m_tEnemyPosition = tpaDynamicObjects[iIndex].tMySavedPosition;
								SelectorRetreat.m_tpEnemyNode = Level().AI.Node(tpaDynamicObjects[iIndex].dwMyNodeID);
								SelectorRetreat.m_tMyPosition = vPosition;
								SelectorRetreat.m_tpMyNode = AI_Node;
								vfSearchForBetterPosition(SelectorRetreat,Squad,Leader);
							}
						}
					}
				}
		}
		else {
			bool bFoundEnemyInfo = false;
			if (this != Leader) {
				CAI_Soldier *tpSoldier = dynamic_cast<CAI_Soldier *>(Leader);
				if (tpSoldier) 
					bFoundEnemyInfo = bfAddEnemyToDynamicObjects(tpSoldier);
			}
			for (int i=0; i<Group.Members.size(); i++) {
				if (bFoundEnemyInfo)
					break;
				if (Group.Members[i] == this)
					continue;
				CAI_Soldier *tpSoldier = dynamic_cast<CAI_Soldier *>(Group.Members[i]);
				if (tpSoldier)
					bFoundEnemyInfo = bfAddEnemyToDynamicObjects(tpSoldier);
			}
			if (!bFoundEnemyInfo)
				GO_TO_PREV_STATE_THIS_UPDATE;
		}
	}

	if (AI_Path.fSpeed < EPS_L) {
		SetLessCoverLook(AI_Node);
		StandUp();
		vfSetMovementType(RUN_FORWARD_3);
	}
	else
		//if (bfCheckForDangerPlace() && (!bfTooBigDistance(AI_Path.TravelPath[AI_Path.TravelStart].P,1.f) || (r_torso_target.yaw - r_torso_current.yaw > EPS_L))) {
		if (bfCheckForDangerPlace()) {
			Squat();
			vfSetMovementType(WALK_FORWARD_1);
		}
		else {
			SetDirectionLook();
			StandUp();
			vfSetMovementType(RUN_FORWARD_3);
		}
}

void CAI_Soldier::OnRetreatAloneNonFire()
{
	WRITE_TO_LOG("retreat alone none-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnRetreatAloneFire()
{
	WRITE_TO_LOG("retreat alone fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorRetreat,Squad,Leader);

	if (!Enemy.Enemy)
		SelectorRetreat.m_tEnemyPosition = tSavedEnemyPosition;

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		//if (AI_Path.TravelPath.empty() || (AI_Path.TravelPath.size()/2 < AI_Path.TravelStart))
			vfSearchForBetterPosition(SelectorRetreat,Squad,Leader);

	if (bfTooBigDistance(tSavedEnemyPosition,ATTACK_FIRE_FIRE_DISTANCE) || !Enemy.Enemy || ((Enemy.Enemy) && !Enemy.bVisible) || bfCheckHistoryForState(aiSoldierHurtAlone,10000) || bfCheckHistoryForState(aiSoldierAttackAlone,10000))
		if (AI_Path.fSpeed < EPS_L)
			SetLessCoverLook(AI_Node);
		else
			SetDirectionLook();
	else
		vfAimAtEnemy();

	vfStopFire();

	if (m_cBodyState != BODY_STATE_LIE)
		StandUp();
	else {
		StandUp();
		m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}

	vfSetMovementType(RUN_FORWARD_3);
}

void CAI_Soldier::OnRetreatAloneDialog()
{
	WRITE_TO_LOG("retreat alone dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackAloneNonFireFire()
{
	WRITE_TO_LOG("attack alone non-fire fire");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfNeedRecharge());

	vfAimAtEnemy(true);

	vfSetFire(true,*getGroup());
	
	if (m_cBodyState != BODY_STATE_LIE)
		Squat();
	else {
		Squat();
		m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}
	vfSetMovementType(WALK_NO);
}

void CAI_Soldier::OnAttackAloneNonFireRun()
{
	WRITE_TO_LOG("attack alone non-fire run");
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
	
	vfStopFire();
	
	if (bfNeedRecharge()) {
		if (!bfSaveFromEnemy(Enemy.Enemy)) {
			INIT_SQUAD_AND_LEADER;
			vfInitSelector(SelectorUnderFireCover,Squad,Leader);
			if (AI_Path.bNeedRebuild)
				vfBuildPathToDestinationPoint(0);
			else
				vfSearchForBetterPosition(SelectorUnderFireCover,Squad,Leader);
		}
		else
			switch (m_cBodyState) {
				case BODY_STATE_STAND : {
					m_tpAnimationBeingWaited = tSoldierAnimations.tNormal.tGlobal.tpReload;
					break;
				}
				case BODY_STATE_CROUCH : {
					m_tpAnimationBeingWaited = tSoldierAnimations.tCrouch.tGlobal.tpReload;
					break;
				}
				case BODY_STATE_LIE : {
					m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpReload;
					break;
				}
			}
	}
	else {

	}
}

void CAI_Soldier::OnAttackAloneNonFireSteal()
{
	WRITE_TO_LOG("attack alone non-fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackAloneNonFireDialog()
{
	WRITE_TO_LOG("attack alone non-fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackAloneFireFire()
{
	WRITE_TO_LOG("attack alone fire fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
	
	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfNeedRecharge());

	vfAimAtEnemy(true);

	vfSetFire(true,*getGroup());
	
	if (m_cBodyState != BODY_STATE_LIE)
		Squat();
	else {
		Squat();
		m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}
	vfSetMovementType(WALK_NO);
}

void CAI_Soldier::OnAttackAloneFireRun()
{
	WRITE_TO_LOG("attack alone fire run");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfNeedRecharge(),aiSoldierRecharge)

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!bfTooFarToEnemy(Enemy.Enemy,ATTACK_FIRE_FIRE_DISTANCE))

	if (m_bStateChanged)
		m_dwLastRangeSearch = 0;

	vfStopFire();
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];

	vfInitSelector(SelectorPatrol,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);

	vfAimAtEnemy();
	
	StandUp();
	vfSetMovementType(RUN_FORWARD_3);
}

void CAI_Soldier::OnAttackAloneFireSteal()
{
	WRITE_TO_LOG("attack alone fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
	
	if (m_bStateChanged)
		m_dwLastRangeSearch = 0;

	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0),aiSoldierRecharge)

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckForEntityVisibility(Enemy.Enemy) || (!bfTooFarToEnemy(Enemy.Enemy,ATTACK_FIRE_FIRE_DISTANCE)))

	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];

	vfInitSelector(SelectorPatrol,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);

	vfAimAtEnemy();
	
	vfStopFire();
	
	if (bfTooFarToEnemy(Enemy.Enemy,25.f)) {
		StandUp();
		vfSetMovementType(RUN_FORWARD_3);
	}
	else {
		Squat();
		vfSetMovementType(WALK_FORWARD_1);
	}
}

void CAI_Soldier::OnAttackAloneFireDialog()
{
	WRITE_TO_LOG("attack alone fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneNonFireFire()
{
	WRITE_TO_LOG("defend alone non-fire fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneNonFireRun()
{
	WRITE_TO_LOG("defend alone non-fire run");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneNonFireSteal()
{
	WRITE_TO_LOG("defend alone non-fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneNonFireDialog()
{
	WRITE_TO_LOG("defend alone non-fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneFireFire()
{
	WRITE_TO_LOG("defend alone fire fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneFireRun()
{
	WRITE_TO_LOG("defend alone fire run");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneFireSteal()
{
	WRITE_TO_LOG("defend alone fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendAloneFireDialog()
{
	WRITE_TO_LOG("defend alone fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroup()
{
	WRITE_TO_LOG("attack group");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroup()
{
	WRITE_TO_LOG("defend group");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnPursuitGroup()
{
	WRITE_TO_LOG("pursuit group");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnFindGroup()
{
	WRITE_TO_LOG("find group");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnRetreatGroup()
{
	WRITE_TO_LOG("retreat group");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnHurtGroup()
{
	WRITE_TO_LOG("hurt group");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupNonFire()
{
	WRITE_TO_LOG("attack group non-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupFire()
{
	WRITE_TO_LOG("attack group fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupNonFire()
{
	WRITE_TO_LOG("defend group non-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupFire()
{
	WRITE_TO_LOG("defend group fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnPursuitGroupNonFire()
{
	WRITE_TO_LOG("pursuit group non-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnPursuitGroupFire()
{
	WRITE_TO_LOG("pursuit group fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnFindGroupNonFire()
{
	WRITE_TO_LOG("find group non-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnFindGroupFire()
{
	WRITE_TO_LOG("find group fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnRetreatGroupNonFire()
{
	WRITE_TO_LOG("retreat group non-fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnRetreatGroupFire()
{
	WRITE_TO_LOG("retreat group fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnRetreatGroupDialog()
{
	WRITE_TO_LOG("retreat group dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupNonFireFire()
{
	WRITE_TO_LOG("attack group non-fire fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupNonFireRun()
{
	WRITE_TO_LOG("attack group non-fire run");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupNonFireSteal()
{
	WRITE_TO_LOG("attack group non-fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupNonFireDialog()
{
	WRITE_TO_LOG("attack group non-fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupFireFire()
{
	WRITE_TO_LOG("attack group fire fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupFireRun()
{
	WRITE_TO_LOG("attack group fire run");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupFireSteal()
{
	WRITE_TO_LOG("attack group fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnAttackGroupFireDialog()
{
	WRITE_TO_LOG("attack group fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupNonFireFire()
{
	WRITE_TO_LOG("defend group non-fire fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupNonFireRun()
{
	WRITE_TO_LOG("defend group non-fire run");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupNonFireSteal()
{
	WRITE_TO_LOG("defend group non-fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupNonFireDialog()
{
	WRITE_TO_LOG("defend group non-fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupFireFire()
{
	WRITE_TO_LOG("defend group fire fire");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupFireRun()
{
	WRITE_TO_LOG("defend group fire run");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupFireSteal()
{
	WRITE_TO_LOG("defend group fire steal");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::OnDefendGroupFireDialog()
{
	WRITE_TO_LOG("defend group fire dialog");

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(bfCheckIfActionOrFightTypeChanged())
}

void CAI_Soldier::Die()
{
	WRITE_TO_LOG("die");

	if (m_tpSoundBeingPlayed && m_tpSoundBeingPlayed->feedback)
		m_tpSoundBeingPlayed->feedback->Stop();

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfStopFire();
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	//while (tStateStack.size())
	//	tStateStack.pop();

	vfAddStateToList(aiSoldierDie);
	
	bActive = false;
	bEnabled = false;
}

void CAI_Soldier::OnTurnOver()
{
	WRITE_TO_LOG("turn over");
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)
		
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfCheckForDanger(),aiSoldierFight)

	if (bfTooBigAngle(r_torso_target.yaw,r_torso_current.yaw,PI_DIV_6)) {
		ESoldierStates eDummy = tStateStack.top();
		tStateStack.pop();
		m_ePreviousState = tStateStack.top();
		tStateStack.push(eDummy);
		GO_TO_PREV_STATE_THIS_UPDATE
	}

	q_look.o_look_speed = r_spine_speed = r_torso_speed = TORSO_START_SPEED;

	vfStopFire();
	
	vfSetMovementType(WALK_NO);
}

void CAI_Soldier::OnWaitForAnimation()
{
	WRITE_TO_LOG("wait for animation");

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)

	vfSetMovementType(WALK_NO);

	//CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfCheckForDanger(),aiSoldierFight)

	if (m_bStateChanged)
		m_bActionStarted = true;
		
	vfStopFire();

	CHECK_IF_GO_TO_PREV_STATE(!m_bActionStarted);
}

void CAI_Soldier::OnWaitForTime()
{
	WRITE_TO_LOG("wait for time");

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfCheckForDanger(),aiSoldierFight)

	if (m_bStateChanged)
		m_dwLastRangeSearch = m_dwCurrentUpdate;
		
	CHECK_IF_GO_TO_PREV_STATE(m_dwCurrentUpdate - m_dwLastRangeSearch > m_dwTimeBeingWaited);
}

void CAI_Soldier::OnRecharge()
{
	WRITE_TO_LOG("recharge");
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)

	CHECK_IF_GO_TO_PREV_STATE(bfNoAmmo());
	
	CHECK_IF_GO_TO_PREV_STATE(!bfNeedRecharge());
	
	vfStopFire();
	
	if (Weapons->ActiveWeapon())
		Weapons->ActiveWeapon()->Reload();

    vfSetMovementType(WALK_NO);
}

void CAI_Soldier::OnLookingOver()
{
	WRITE_TO_LOG("looking over");
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE((g_Squad() != SPECIAL_SQUAD) && bfCheckForDanger(),aiSoldierFight)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	AI_Path.TravelPath.clear();
	AI_Path.TravelStart = 0;

	if (m_tpaPatrolPoints.size() > 1) {
		CHECK_IF_SWITCH_TO_NEW_STATE(this == Leader,aiSoldierPatrolRoute)
		SWITCH_TO_NEW_STATE(aiSoldierFollowLeaderPatrol);
	}

	if (m_cBodyState != BODY_STATE_LIE)
		StandUp();
	else {
		StandUp();
		m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}

	SetDirectionLook();
	vfStopFire();
	vfSetMovementType(WALK_NO);
	r_torso_speed = TORSO_SLOW_SPEED;
}

void CAI_Soldier::OnPatrolReturnToRoute()
{
	WRITE_TO_LOG("return to route");

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfCheckForDanger(),aiSoldierFight);

	INIT_SQUAD_AND_LEADER;
	
	if (AI_Path.bNeedRebuild) {
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,0,0);
		if (AI_Path.Nodes.size() > 1) {
			AI_Path.BuildTravelLine(Position());
			CTravelNode tTemp;
			tTemp.floating = FALSE;
			tTemp.P = AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P;
		}
		else {
			AI_Path.TravelPath.clear();
			AI_Path.bNeedRebuild = FALSE;
		}
	}
	else {
		vfInitSelector(SelectorPatrol,Squad,Leader);
		float fDistance;
		if (Leader == this) {
			fDistance = m_tpaPatrolPoints[0].distance_to(Position());
			SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[0];
		}
		else {
			fDistance = vPosition.distance_to(Leader->Position());
			SelectorPatrol.m_tEnemyPosition = Leader->Position();
		}
		
		vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);
		
		if ((fDistance < 5.f) || (AI_NodeID == AI_Path.DestNode)) {
			if (this == Leader) {
				float fDistance = 0.f;
				for (int i=0; i<SelectorPatrol.taMemberPositions.size(); i++) {
					Fvector tTemp;
					tTemp.sub(vPosition,SelectorPatrol.taMemberPositions[i]);
				}
				if (fDistance < 5.f) {
					ESoldierStates eDummy = tStateStack.top();
					tStateStack.pop();
					m_ePreviousState = tStateStack.top();
					tStateStack.push(eDummy);
					GO_TO_PREV_STATE_THIS_UPDATE
				}
			}
			else {
				ESoldierStates eDummy = tStateStack.top();
				tStateStack.pop();
				m_ePreviousState = tStateStack.top();
				tStateStack.push(eDummy);
				GO_TO_PREV_STATE_THIS_UPDATE
			}
		}
		else {
			AI_Path.bNeedRebuild = TRUE;
		}
	}

	vfStopFire();
	SetDirectionLook();
	if (m_cBodyState != BODY_STATE_LIE)
		StandUp();
	else {
		StandUp();
		m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}
	vfSetMovementType(Leader != this ? WALK_FORWARD_3 : WALK_FORWARD_3);
}

void CAI_Soldier::OnPatrolRoute()
{
	WRITE_TO_LOG("patrol route");

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfCheckForDanger(),aiSoldierFight);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(m_bStateChanged,aiSoldierPatrolReturnToRoute)
	
	INIT_SQUAD_AND_LEADER;
	DWORD dwCurTime = m_dwCurrentUpdate;
	CGroup &Group = Squad.Groups[g_Group()];

	if ((m_dwCreatePathAttempts < 20) && ((!(AI_Path.fSpeed)) || (AI_Path.TravelPath.empty()) || (AI_Path.TravelPath[AI_Path.TravelStart].P.distance_to(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) <= .5f))) {
		
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;

		m_dwLastRangeSearch = 0;
		
		if (m_bStateChanged)
			m_dwLoopCount = 0;
		
		for (int i=0; i<Group.Members.size(); i++)
			if (((CCustomMonster*)(Group.Members[i]))->AI_Path.fSpeed > .1f)
				return;

//		if (!m_bLooped) {
//			Fvector tTemp1 = m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 2];
//			tTemp1.sub(m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 1]);
//			vfNormalizeSafe(tTemp1);
//			SRotation sRot;
//			mk_rotation(tTemp1,sRot);
//			if (!((fabsf(r_torso_target.yaw - sRot.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - sRot.yaw) - PI_MUL_2) < PI_DIV_6)))) {
//				AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
//				r_torso_target.yaw = sRot.yaw > PI ? sRot.yaw - PI_MUL_2 : sRot.yaw;
//				r_spine_target.yaw = r_torso_target.yaw; 
//				r_target.yaw = 0;//r_torso_target.yaw; 
//				SWITCH_TO_NEW_STATE(aiSoldierTurnOver);
//			}
//		}

		m_dwLastRangeSearch = dwCurTime;
		
		if ((m_bLooped) || (m_dwLoopCount % 2 == 0)) {
			vector<CLevel::SPatrolPath> &tpaPatrolPaths = Level().tpaPatrolPaths;
			m_dwStartPatrolNode = tpaPatrolPaths[m_dwPatrolPathIndex].dwStartNode;
			vfCreatePointSequence(tpaPatrolPaths[m_dwPatrolPathIndex],m_tpaPatrolPoints,m_bLooped);
			m_tpaPointDeviations.resize(m_tpaPatrolPoints.size());
		}
		
		CHECK_IF_SWITCH_TO_NEW_STATE(vPosition.distance_to(m_tpaPatrolPoints[0]) > 5.f,aiSoldierPatrolReturnToRoute)
		
		vfCreateFastRealisticPath(m_tpaPatrolPoints, m_dwStartPatrolNode, m_tpaPointDeviations, AI_Path.TravelPath, m_dwaNodes, m_bLooped,false);
		
		m_dwCreatePathAttempts++;
		Msg("%s paths : %d",cName(),m_dwCreatePathAttempts);
		// invert path if needed
		if (AI_Path.TravelPath.size()) {
			if (!m_bLooped) {
				m_dwLoopCount++;
				if (m_dwLoopCount % 2 == 0) {
					DWORD dwCount = AI_Path.TravelPath.size();
					for ( i=0; i<dwCount / 2; i++) {
						Fvector tTemp = AI_Path.TravelPath[i].P;
						AI_Path.TravelPath[i].P = AI_Path.TravelPath[dwCount - i - 1].P;
						AI_Path.TravelPath[dwCount - i - 1].P = tTemp;
					}
				}
			}
			m_dwLastRangeSearch = m_dwCurrentUpdate;
		}
	}

	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastRadioTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fRadioRefreshRate) && ((dwCurTime - m_dwLastRadioTalk > m_fMaxRadioIinterval) || ((dwCurTime - m_dwLastRadioTalk > m_fMinRadioIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastRadioTalk - m_fMinRadioIinterval)/(m_fMaxRadioIinterval - m_fMinRadioIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play radio-sound
			m_tpSoundBeingPlayed = &(sndRadio[Random.randI(SND_RADIO_COUNT)]);
			
			if (m_tpSoundBeingPlayed->feedback)			
				return;

			pSounds->PlayAtPos(*m_tpSoundBeingPlayed,this,vPosition);
		}
	}
	
	if (m_cBodyState != BODY_STATE_LIE)
		StandUp();
	else {
		StandUp();
		m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}
	vfSetLookAndFireMovement(false, WALK_FORWARD_3,1.0f,Group,dwCurTime);
}

void CAI_Soldier::OnFollowLeaderPatrol()
{
	WRITE_TO_LOG("follow leader patrol");

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfAmIDead(),aiSoldierDie)
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE_AND_UPDATE(bfCheckForDanger(),aiSoldierFight);
		
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_GO_TO_NEW_STATE(Leader == this,aiSoldierPatrolRoute);

	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged,aiSoldierPatrolReturnToRoute);
	
	if ((!(AI_Path.fSpeed)) || (AI_Path.TravelPath.empty()) || (AI_Path.TravelPath[AI_Path.TravelStart].P.distance_to(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) <= .5f)) {
		CAI_Soldier *SoldierLeader = dynamic_cast<CAI_Soldier *>(Leader);
//		if (!m_bLooped) {
//			Fvector tTemp1 = m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 2];
//			tTemp1.sub(m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 1]);
//			vfNormalizeSafe(tTemp1);
//			SRotation sRot;
//			mk_rotation(tTemp1,sRot);
//			if (!((fabsf(r_torso_target.yaw - sRot.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - sRot.yaw) - PI_MUL_2) < PI_DIV_6)))) {
//				AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
//				r_torso_target.yaw = sRot.yaw > PI ? sRot.yaw - 2*PI : sRot.yaw;
//				r_spine_target.yaw = r_torso_target.yaw; 
//				r_target.yaw = 0;//r_torso_target.yaw; 
//				SWITCH_TO_NEW_STATE(aiSoldierTurnOver);
//			}
//		}
		
		//if ((dwCurTime - SoldierLeader->m_dwLastRangeSearch < 10000) && (SoldierLeader->AI_Path.fSpeed > EPS_L)) {
		if (SoldierLeader->AI_Path.fSpeed > .1f) {
			m_dwLoopCount = SoldierLeader->m_dwLoopCount;
			
			AI_Path.TravelPath.clear();
			AI_Path.TravelPath.resize(SoldierLeader->AI_Path.TravelPath.size());
			
			vector<DWORD> &dwaNodes = SoldierLeader->m_dwaNodes;
			
			CAI_Space &AI = Level().AI;
			float fHalfSubnodeSize = AI.GetHeader().size*.5f;
					
			for (int i=0, j=0, k=0; i<SoldierLeader->AI_Path.TravelPath.size(); i++, j++) {
				
				AI_Path.TravelPath[j].floating = FALSE;
				AI_Path.TravelPath[j].P = SoldierLeader->AI_Path.TravelPath[i].P;

				Fvector tTemp;
				
				if (m_bLooped)
					tTemp.sub(SoldierLeader->AI_Path.TravelPath[i < SoldierLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, SoldierLeader->AI_Path.TravelPath[i].P);
				else
					if (i < SoldierLeader->AI_Path.TravelPath.size() - 1)
						tTemp.sub(SoldierLeader->AI_Path.TravelPath[i < SoldierLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, SoldierLeader->AI_Path.TravelPath[i].P);
					else
						tTemp.sub(SoldierLeader->AI_Path.TravelPath[i].P, SoldierLeader->AI_Path.TravelPath[i - 1].P);

				if (tTemp.magnitude() < .1f) {
					j--;
					continue;
				}

				tTemp.y = 0.f;
				tTemp.normalize();
				
				if (m_bLooped)
					if (Group.Members[0] == this) 
						tTemp.set(tTemp.z,0,-tTemp.x);
					else
						tTemp.set(-tTemp.z,0,tTemp.x);
				else
					if (Group.Members[0] == this) 
						if (m_dwLoopCount % 2)
							tTemp.set(tTemp.z,0,-tTemp.x);
						else
							tTemp.set(-tTemp.z,0,tTemp.x);
					else
						if (m_dwLoopCount % 2)
							tTemp.set(-tTemp.z,0,tTemp.x);
						else
							tTemp.set(tTemp.z,0,-tTemp.x);
				
				AI_Path.TravelPath[j].P.add(tTemp);

				if ((j > 1) && (COMPUTE_DISTANCE_2D(AI_Path.TravelPath[j].P,AI_Path.TravelPath[j-2].P) <= fHalfSubnodeSize + COMPUTE_DISTANCE_2D(AI_Path.TravelPath[j-1].P,AI_Path.TravelPath[j-2].P))) {
					Fvector tPrevious = AI_Path.TravelPath[j-2].P;
					Fvector tCurrent = AI_Path.TravelPath[j-1].P;
					Fvector tNext = AI_Path.TravelPath[j].P;
					Fvector tTemp1, tTemp2;
					tTemp1.sub(tCurrent,tPrevious);
					tTemp2.sub(tNext,tCurrent);
					tTemp1.normalize_safe();
					tTemp1.y = tTemp2.y = 0;
					tTemp2.normalize_safe();
					float fAlpha = tTemp1.dotproduct(tTemp2);
					clamp(fAlpha, -.99999f, +.99999f);
					if ((acosf(fAlpha) < PI_DIV_8*.375f) || (acosf(fAlpha) > 2*PI_DIV_8*.375f)) {
					//if (acosf(fAlpha) < PI_DIV_8*.375f) {
						j--;
						continue;
					}
				}

				int m=k;
				for ( ; (k < dwaNodes.size()) && (!bfInsideNode(AI,AI.Node(dwaNodes[k]),SoldierLeader->AI_Path.TravelPath[i].P,fHalfSubnodeSize)); k++) ;

				if (k >= dwaNodes.size()) {
					k = m;
					AI_Path.TravelPath.erase(AI_Path.TravelPath.begin() + j);
					j--;
					continue;
				}
				//R_ASSERT(k < dwaNodes.size());

				DWORD dwBestNode;
				float fBestCost;
				NodePosition tNodePosition;
				AI.PackPosition(tNodePosition,AI_Path.TravelPath[j].P);
				AI.q_Range_Bit_X(dwaNodes[k],SoldierLeader->AI_Path.TravelPath[i].P,4*fHalfSubnodeSize,&tNodePosition,dwBestNode,fBestCost);
				AI_Path.TravelPath[j].P.y = ffGetY(*(AI.Node(dwBestNode)),AI_Path.TravelPath[j].P.x,AI_Path.TravelPath[j].P.z);
			}
			
			AI_Path.TravelPath.resize(j);
			AI_Path.TravelStart = 0;

//			float fTemp, fDistance = AI_Path.TravelPath[AI_Path.TravelStart = 0].P.distance_to(Position());
//			for ( i=1; i<AI_Path.TravelPath.size(); i++)
//				if ((fTemp = AI_Path.TravelPath[i].P.distance_to(Position())) < fDistance) {
//					fDistance = fTemp;
//					AI_Path.TravelStart = i;
//				}

			m_dwLastRangeSearch = m_dwCurrentUpdate;
		}
		else {
			AI_Path.TravelPath.clear();
			AI_Path.TravelStart = 0;
		}
	}
	
	if ((!m_dwLastRangeSearch) || (m_dwCurrentUpdate - m_dwLastRangeSearch >= 5000)) {
		m_dwLastRangeSearch = m_dwCurrentUpdate;
		m_fMinPatrolDistance = MIN_PATROL_DISTANCE;
		m_fMaxPatrolDistance = MAX_PATROL_DISTANCE - ::Random.randF(0,4);
	}

	Fvector tTemp0;
	tTemp0.sub(Leader->Position(),Position());
	tTemp0.normalize();
	tWatchDirection.normalize();
	
	DWORD dwCurTime = m_dwCurrentUpdate;
	if (m_cBodyState != BODY_STATE_LIE)
		StandUp();
	else {
		StandUp();
		m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}
	if (acosf(tWatchDirection.dotproduct(tTemp0)) < PI_DIV_2) {
		float fDistance = Leader->Position().distance_to(Position());
		if (fDistance >= m_fMaxPatrolDistance) {
			vfSetLookAndFireMovement(false, WALK_FORWARD_3,1.1f,Group,dwCurTime);
		}
		else
			if (fDistance <= m_fMinPatrolDistance) {
				vfSetLookAndFireMovement(false, WALK_FORWARD_3,0.9f,Group,dwCurTime);
			}
			else {
				vfSetLookAndFireMovement(false, WALK_FORWARD_3,float(((fDistance - (m_fMaxPatrolDistance + m_fMinPatrolDistance)*.5f)/((m_fMaxPatrolDistance - m_fMinPatrolDistance)*.5f)*.1f + m_fMinPatrolDistance)),Group,dwCurTime);
			}
	}
	else {
		vfSetLookAndFireMovement(false, WALK_FORWARD_3,0.9f,Group,dwCurTime);
	}
}

void CAI_Soldier::Think()
{
	m_dwUpdateCount++;
	m_dwLastUpdate = m_dwCurrentUpdate;
	m_dwCurrentUpdate = Level().timeServer();
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
}
