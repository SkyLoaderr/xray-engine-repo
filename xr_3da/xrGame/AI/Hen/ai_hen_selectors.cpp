////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_selectors.cpp
//	Created 	: 05.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\entity.h"
#include "..\\..\\CustomMonster.h"
#include "ai_hen_selectors.h"

CHenSelectorAttack::CHenSelectorAttack()
{ 
	Name = "selector_attack"; 
}

float CHenSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
			for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
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

CHenSelectorFreeHunting::CHenSelectorFreeHunting()
{ 
	Name = "selector_free_hunting"; 
}

float CHenSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	bStop = true;
	return(0);
	
	// initialization
//	m_tpCurrentNode = tNode;
//	m_fDistance = fDistance;
//	vfInit();
//	// computations
//	vfAddTravelCost();
//	CHECK_RESULT;
//	vfAddLightCost();
//	CHECK_RESULT;
//	vfAddTotalCoverCost();
//	CHECK_RESULT;
//	if (m_tLeader) {
//		vfAddDistanceToLeaderCost();
//		CHECK_RESULT;
//		vfAddCoverFromLeaderCost();
//		CHECK_RESULT;
//		if (taMemberPositions.size()) {
//			if (m_iAliveMemberCount) {
//				for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
//					vfAssignMemberPositionAndNode();
//					vfComputeMemberDirection();
//					vfAddDistanceToMemberCost();
//					vfAddCoverFromMemberCost();
//				}
//			}
//		}
//	}
//	// checking for epsilon
//	vfCheckForEpsilon(bStop);
//	// returning a value
//	return(m_fResult);
}

CHenSelectorFollow::CHenSelectorFollow()
{ 
	Name = "selector_follow"; 
}

float CHenSelectorFollow::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
				for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
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

CHenSelectorPursuit::CHenSelectorPursuit()
{ 
	Name = "selector_pursuit"; 
}

float CHenSelectorPursuit::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
			for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
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

CHenSelectorUnderFire::CHenSelectorUnderFire()
{ 
	Name = "selector_under_fire"; 
}

float CHenSelectorUnderFire::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
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
				for ( m_iCurrentMember=0 ; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) {
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
