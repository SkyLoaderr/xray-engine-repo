////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_selectors.cpp
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\entity.h"
#include "..\\..\\CustomMonster.h"
#include "ai_zombie_selectors.h"

CZombieSelectorAttack::CZombieSelectorAttack()
{ 
	Name = "selector_attack"; 
}

float CZombieSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorDefend::CZombieSelectorDefend()
{ 
	Name = "selector_defend"; 
}

float CZombieSelectorDefend::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorFindEnemy::CZombieSelectorFindEnemy()
{ 
	Name = "selector_find_enemy"; 
}

float CZombieSelectorFindEnemy::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorFollowLeader::CZombieSelectorFollowLeader()
{ 
	Name = "selector_follow_leader"; 
}

float CZombieSelectorFollowLeader::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorFreeHunting::CZombieSelectorFreeHunting()
{ 
	Name = "selector_free_hunting"; 
}

float CZombieSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorMoreDeadThanAlive::CZombieSelectorMoreDeadThanAlive()
{ 
	Name = "selector_mdta"; 
}

float CZombieSelectorMoreDeadThanAlive::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorNoWeapon::CZombieSelectorNoWeapon()
{ 
	Name = "selector_no_weapon"; 
}

float CZombieSelectorNoWeapon::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorPatrol::CZombieSelectorPatrol()
{ 
	Name = "selector_patrol"; 
}

float CZombieSelectorPatrol::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddDistanceToEnemyCost();
	return(m_fResult);
}

CZombieSelectorPursuit::CZombieSelectorPursuit()
{ 
	Name = "selector_pursuit"; 
}

float CZombieSelectorPursuit::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorReload::CZombieSelectorReload()
{ 
	Name = "selector_reload"; 
}

float CZombieSelectorReload::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorRetreat::CZombieSelectorRetreat()
{ 
	Name = "selector_retreat"; 
}

float CZombieSelectorRetreat::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorSenseSomething::CZombieSelectorSenseSomething()
{ 
	Name = "selector_sense_something"; 
}

float CZombieSelectorSenseSomething::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CZombieSelectorUnderFire::CZombieSelectorUnderFire()
{ 
	Name = "selector_under_fire"; 
}

float CZombieSelectorUnderFire::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
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
	//vfAddDeviationFromPreviousDirectionCost();
	//CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	
	CHECK_RESULT;
	return(m_fResult);
}
