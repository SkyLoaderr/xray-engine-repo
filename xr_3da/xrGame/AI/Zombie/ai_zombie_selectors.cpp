////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_selectors.cpp
//	Created 	: 07.05.2002
//  Modified 	: 07.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI selectors for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\entity.h"
#include "..\\..\\CustomMonster.h"
#include "ai_zombie_selectors.h"

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

	if (m_fDistance < .7f)
		m_fResult += 10000;
	//CHECK_RESULT;
	vfAddDeviationFromPreviousDirectionCost();
	CHECK_RESULT;
	//vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

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
