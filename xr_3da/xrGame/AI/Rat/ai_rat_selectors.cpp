////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_selectors.cpp
//	Created 	: 23.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\entity.h"
#include "..\\..\\CustomMonster.h"
#include "ai_rat_selectors.h"

CRatSelectorAttack::CRatSelectorAttack()
{ 
	Name = "selector_attack"; 
}

float CRatSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	vfAddEnemyLookCost();
	CHECK_RESULT;
	if (taMemberPositions.size()) {
		if (m_iAliveMemberCount) {
			for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) {
				vfAssignMemberPositionAndNode();
				vfComputeMemberDirection();
				vfAddDistanceToMemberCost();
				vfComputeSurroundEnemy();
				vfAddCoverFromMemberCost();
				vfAddDeviationFromMemberViewCost();
			}
			vfAddSurroundEnemyCost();
		}
	}
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorFindEnemy::CRatSelectorFindEnemy()
{ 
	Name = "selector_find_enemy"; 
}

float CRatSelectorFindEnemy::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	vfAddDistanceToLastPositionCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorFollowLeader::CRatSelectorFollowLeader()
{ 
	Name = "selector_follow_leader"; 
}

float CRatSelectorFollowLeader::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	if (m_tLeader) {
		vfAddDistanceToLeaderCost();
		CHECK_RESULT;
		vfAddCoverFromLeaderCost();
		CHECK_RESULT;
		if (taMemberPositions.size()) {
			if (m_iAliveMemberCount) {
				for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) {
					vfAssignMemberPositionAndNode();
					vfComputeMemberDirection();
					vfAddDistanceToMemberCost();
					vfAddCoverFromMemberCost();
				}
			}
		}
	}
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorFreeHunting::CRatSelectorFreeHunting()
{ 
	Name = "selector_free_hunting"; 
}

float CRatSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddDeviationFromPreviousDirectionCost();
	CHECK_RESULT;
	//vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorMoreDeadThanAlive::CRatSelectorMoreDeadThanAlive()
{ 
	Name = "selector_mdta"; 
}

float CRatSelectorMoreDeadThanAlive::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorNoWeapon::CRatSelectorNoWeapon()
{ 
	Name = "selector_no_weapon"; 
}

float CRatSelectorNoWeapon::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	/**
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	/**/
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	/**
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	/**/
	// returning a value
	return(m_fResult);
}

CRatSelectorPatrol::CRatSelectorPatrol()
{ 
	Name = "selector_patrol"; 
}

float CRatSelectorPatrol::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddDistanceToEnemyCost();
	return(m_fResult);
}

CRatSelectorPursuit::CRatSelectorPursuit()
{ 
	Name = "selector_pursuit"; 
}

float CRatSelectorPursuit::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	if (taMemberPositions.size()) {
		if (m_iAliveMemberCount) {
			for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) {
				vfAssignMemberPositionAndNode();
				vfComputeMemberDirection();
				vfAddDistanceToMemberCost();
				vfComputeSurroundEnemy();
				vfAddCoverFromMemberCost();
			}
			vfAddSurroundEnemyCost();
		}
	}
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorReload::CRatSelectorReload()
{ 
	Name = "selector_reload"; 
}

float CRatSelectorReload::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	//vfAddTravelCost();
	//CHECK_RESULT;
	//vfAddLightCost();
	//CHECK_RESULT;
	//vfAddTotalCoverCost();
	//CHECK_RESULT;
	//vfAddDistanceToEnemyCost();
	//CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
	//vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorRetreat::CRatSelectorRetreat()
{ 
	Name = "selector_retreat"; 
}

float CRatSelectorRetreat::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorSenseSomething::CRatSelectorSenseSomething()
{ 
	Name = "selector_sense_something"; 
}

float CRatSelectorSenseSomething::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CRatSelectorUnderFire::CRatSelectorUnderFire()
{ 
	Name = "selector_under_fire"; 
}

float CRatSelectorUnderFire::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddDeviationFromPreviousDirectionCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}
