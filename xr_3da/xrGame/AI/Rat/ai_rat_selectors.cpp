////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 23.04.2002
//  Modified 	: 23.04.2002
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
	if (m_tLeader)
		if (taMemberPositions.size()) {
			if (m_iAliveMemberCount) {
				for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) 
					if (taMembers[m_iCurrentMember]->g_Health() > 0) {
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
				for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) 
					if (taMembers[m_iCurrentMember]->g_Health() > 0) {
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

CRatSelectorFollow::CRatSelectorFollow()
{ 
	Name = "selector_follow"; 
}

float CRatSelectorFollow::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
				for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) 
					if (taMembers[m_iCurrentMember]->g_Health() > 0) {
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
	if (m_tLeader) {
		if (taMemberPositions.size()) {
			if (m_iAliveMemberCount) {
				for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) 
					if (taMembers[m_iCurrentMember]->g_Health() > 0) {
						vfAssignMemberPositionAndNode();
						vfComputeMemberDirection();
						vfAddDistanceToMemberCost();
						vfComputeSurroundEnemy();
						vfAddCoverFromMemberCost();
					}
				vfAddSurroundEnemyCost();
			}
		}
	}
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
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	/**
	if (m_tLeader) {
		vfAddDistanceToLeaderCost();
		CHECK_RESULT;
		vfAddCoverFromLeaderCost();
		CHECK_RESULT;
		if (taMemberPositions.size()) {
			if (m_iAliveMemberCount) {
				for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) 
					if (taMembers[m_iCurrentMember]->g_Health() > 0) {
						vfAssignMemberPositionAndNode();
						vfComputeMemberDirection();
						vfAddDistanceToMemberCost();
						vfAddCoverFromMemberCost();
					}
			}
		}
	}
	/**/
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}
