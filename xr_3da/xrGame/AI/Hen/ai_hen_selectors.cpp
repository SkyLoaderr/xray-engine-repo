////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.cpp
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\entity.h"
#include "..\\..\\CustomMonster.h"
#include "ai_hen_selectors.h"
#include "..\\ai_monsters.h"

//#define OLD_COVER_COST
#define DEST_POSITIONS

void CHenSelectorBase::Load(CInifile* ini, const char* section)
{
	sscanf(ini->ReadSTRING(section,Name),
		"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
		&fSearchRange,
		&fEnemySurround,
		&fTotalCover,
		&fTravelWeight,
		&fLightWeight,
		&fLaziness,
		&fTotalViewVectorWeight,
		&fCoverFromLeaderWeight,
		&fOptLeaderDistance,
		&fOptLeaderDistanceWeight,
		&fMinLeaderDistance,
		&fMinLeaderDistanceWeight,
		&fMaxLeaderDistance,
		&fMaxLeaderDistanceWeight,
		&fLeaderViewDeviationWeight,
		&fMaxLeaderHeightDistance,
		&fMaxLeaderHeightDistanceWeight,
		&fCoverFromMemberWeight,
		&fOptMemberDistance,
		&fOptMemberDistanceWeight,
		&fMinMemberDistance,
		&fMinMemberDistanceWeight,
		&fMaxMemberDistance,
		&fMaxMemberDistanceWeight,
		&fMemberViewDeviationWeight,
		&fCoverFromEnemyWeight,
		&fOptEnemyDistance,
		&fOptEnemyDistanceWeight,
		&fMinEnemyDistance,
		&fMinEnemyDistanceWeight,
		&fMaxEnemyDistance,
		&fMaxEnemyDistanceWeight,
		&fEnemyViewDeviationWeight
	);
};

CHenSelectorAttack::CHenSelectorAttack()
{ 
	Name = "selector_attack"; 
}

void CHenSelectorBase::Init()
{
	for ( m_iCurrentMember = 0, m_iAliveMemberCount=0; m_iCurrentMember<taMemberPositions.size(); m_iCurrentMember++) 
		if (taMembers[m_iCurrentMember]->g_Health() > 0)
			m_iAliveMemberCount++;
}

IC void CHenSelectorBase::vfAddTravelCost()
{
	m_fResult += m_fDistance*fTravelWeight;
}

IC void CHenSelectorBase::vfAddLightCost()
{
	m_fResult += ((float)(m_tpCurrentNode->light)/255.f)*fLightWeight;
}

IC void CHenSelectorBase::vfComputeCurrentPosition()
{
	Fvector tTemp0, tTemp1;
	Level().AI.UnpackPosition(tTemp0,m_tpCurrentNode->p0);
	Level().AI.UnpackPosition(tTemp1,m_tpCurrentNode->p1);
	m_tCurrentPosition.lerp(tTemp1,tTemp1,.5f);
}

IC void CHenSelectorBase::vfAddDistanceToEnemyCost()
{
	float fDistanceToEnemy = m_tCurrentPosition.distance_to(m_tEnemyPosition);
	if (fDistanceToEnemy < fMinEnemyDistance)
		m_fResult += fMinEnemyDistanceWeight*(fMinEnemyDistance + 0.1f)/(fDistanceToEnemy + 0.1f);
	else
		if (fDistanceToEnemy > fMaxEnemyDistance)
			m_fResult += fMaxEnemyDistanceWeight*(fDistanceToEnemy + 0.1f)/(fMaxEnemyDistance + 0.1f);
		else
			m_fResult += fOptEnemyDistanceWeight*fabs(fDistanceToEnemy - fOptEnemyDistance + 1.f)/(fOptEnemyDistance + 1.f);
}

IC void CHenSelectorBase::vfAddDistanceToLeaderCost()
{
	float fDistanceToLeader = m_tCurrentPosition.distance_to(m_tLeaderPosition);
	if (fDistanceToLeader < fMinLeaderDistance)
		m_fResult += fMinLeaderDistanceWeight*(fMinLeaderDistance + 0.1f)/(fDistanceToLeader + 0.1f);
	else
		if (fDistanceToLeader > fMaxLeaderDistance)
			m_fResult += fMaxLeaderDistanceWeight*(fDistanceToLeader + 0.1f)/(fMaxLeaderDistance + 0.1f);
		else
			m_fResult += fOptLeaderDistanceWeight*fabs(fDistanceToLeader - fOptLeaderDistance + 1.f)/(fOptLeaderDistance + 1.f);
}

IC void CHenSelectorBase::vfAddDistanceToMemberCost()
{
#ifdef DEST_POSITIONS
	float fDistanceToMember = m_tCurrentPosition.distance_to(taDestMemberPositions[m_iCurrentMember]);
#else
	float fDistanceToMember = m_tCurrentPosition.distance_to(taMemberPositions[m_iCurrentMember]);
#endif
	if (fDistanceToMember < fMinMemberDistance)
		m_fResult += fMinMemberDistanceWeight*(fMinMemberDistance + 0.1f)/(fDistanceToMember + 0.1f)/m_iAliveMemberCount;
	else
		if (fDistanceToMember > fMaxMemberDistance)
			m_fResult += fMaxMemberDistanceWeight*(fDistanceToMember + 0.1f)/(fMaxMemberDistance + 0.1f)/m_iAliveMemberCount;
		else
			m_fResult += fOptMemberDistanceWeight*fabs(fDistanceToMember - fOptMemberDistance + 1.f)/(fOptMemberDistance + 1.f)/m_iAliveMemberCount;
}

IC void CHenSelectorBase::vfAddCoverFromEnemyCost()
{
#ifdef OLD_COVER_COST
	m_tEnemyDirection.x = m_tEnemyPosition.x - m_tCurrentPosition.x;
	m_tEnemyDirection.y = m_tEnemyPosition.y - m_tCurrentPosition.y;
	m_tEnemyDirection.z = m_tEnemyPosition.z - m_tCurrentPosition.z;
	if (m_tEnemyDirection.x < 0.0) {
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpCurrentNode->cover[0])/255.f);
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpEnemyNode->cover[2])/255.f);
	}
	else {
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpCurrentNode->cover[2])/255.f);
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpEnemyNode->cover[0])/255.f);
	}
	if (m_tEnemyDirection.z < 0.0) {
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpCurrentNode->cover[3])/255.f);
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpEnemyNode->cover[1])/255.f);
	}
	else {
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpCurrentNode->cover[1])/255.f);
		m_fResult += fCoverFromEnemyWeight*(1.0 - float(m_tpEnemyNode->cover[3])/255.f);
	}
#else
	m_tEnemyDirection.x = m_tEnemyPosition.x - m_tCurrentPosition.x;
	m_tEnemyDirection.y = m_tEnemyPosition.y - m_tCurrentPosition.y;
	m_tEnemyDirection.z = m_tEnemyPosition.z - m_tCurrentPosition.z;
	vfNormalizeSafe(m_tEnemyDirection);
	float fAlpha;
	if (m_tEnemyDirection.x < 0.0)
		if (m_tEnemyDirection.z >= 0.0) {
			fAlpha = acosf(m_tEnemyDirection.dotproduct(tLeft));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[1])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[3])/255.f));
		}
		else {
			fAlpha = acosf(m_tEnemyDirection.dotproduct(tLeft));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[3])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[1])/255.f));
		}
	else
		if (m_tEnemyDirection.z >= 0.0) {
			fAlpha = acosf(m_tEnemyDirection.dotproduct(tRight));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[1])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[3])/255.f));
		}
		else {
			fAlpha = acosf(m_tEnemyDirection.dotproduct(tRight));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[3])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(0.0 + float(m_tpEnemyNode->cover[1])/255.f));
		}
#endif
}

IC void CHenSelectorBase::vfAddCoverFromLeaderCost()
{
#ifdef OLD_COVER_COST
	m_tLeaderDirection.x = m_tLeaderPosition.x - m_tCurrentPosition.x;
	m_tLeaderDirection.y = m_tLeaderPosition.y - m_tCurrentPosition.y;
	m_tLeaderDirection.z = m_tLeaderPosition.z - m_tCurrentPosition.z;
	if (m_tLeaderDirection.x < 0.0) {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[0])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpLeaderNode->cover[2])/255.f);
	}
	else {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[2])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpLeaderNode->cover[0])/255.f);
	}
	if (m_tLeaderDirection.z < 0.0) {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[3])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpLeaderNode->cover[1])/255.f);
	}
	else {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[1])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpLeaderNode->cover[3])/255.f);
	}
	if (fabs(m_tLeaderDirection.y) > 2.f)
		m_fResult += fabs(m_tLeaderDirection.y)*fMaxLeaderHeightDistanceWeight;
#else
	m_tLeaderDirection.x = m_tLeaderPosition.x - m_tCurrentPosition.x;
	m_tLeaderDirection.y = m_tLeaderPosition.y - m_tCurrentPosition.y;
	m_tLeaderDirection.z = m_tLeaderPosition.z - m_tCurrentPosition.z;
	if (fabs(m_tLeaderDirection.y) > 2.f)
		m_fResult += fabs(m_tLeaderDirection.y)*fMaxLeaderHeightDistanceWeight;
	vfNormalizeSafe(m_tLeaderDirection);
	float fAlpha;
	if (m_tLeaderDirection.x < 0.0)
		if (m_tLeaderDirection.z >= 0.0) {
			fAlpha = acosf(m_tLeaderDirection.dotproduct(tLeft));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[1])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[3])/255.f));
		}
		else {
			fAlpha = acosf(m_tLeaderDirection.dotproduct(tLeft));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[3])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[1])/255.f));
		}
	else
		if (m_tLeaderDirection.z >= 0.0) {
			fAlpha = acosf(m_tLeaderDirection.dotproduct(tRight));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[1])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[3])/255.f));
		}
		else {
			fAlpha = acosf(m_tLeaderDirection.dotproduct(tRight));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[3])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpLeaderNode->cover[1])/255.f));
		}
#endif
}

IC void CHenSelectorBase::vfAddCoverFromMemberCost()
{
#ifdef OLD_COVER_COST
	m_tCurrentMemberDirection.x = m_tCurrentMemberPosition.x - m_tCurrentPosition.x;
	m_tCurrentMemberDirection.y = m_tCurrentMemberPosition.y - m_tCurrentPosition.y;
	m_tCurrentMemberDirection.z = m_tCurrentMemberPosition.z - m_tCurrentPosition.z;
	if (m_tCurrentMemberDirection.x < 0.0) {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[0])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentMemberNode->cover[2])/255.f);
	}
	else {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[2])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentMemberNode->cover[0])/255.f);
	}
	if (m_tCurrentMemberDirection.z < 0.0) {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[3])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentMemberNode->cover[1])/255.f);
	}
	else {
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentNode->cover[1])/255.f);
		m_fResult += fCoverFromLeaderWeight*(1.0 - float(m_tpCurrentMemberNode->cover[3])/255.f);
	}
#else
	m_tCurrentMemberDirection.x = m_tLeaderPosition.x - m_tCurrentPosition.x;
	m_tCurrentMemberDirection.y = m_tLeaderPosition.y - m_tCurrentPosition.y;
	m_tCurrentMemberDirection.z = m_tLeaderPosition.z - m_tCurrentPosition.z;
	vfNormalizeSafe(m_tCurrentMemberDirection);
	float fAlpha;
	if (m_tCurrentMemberDirection.x < 0.0)
		if (m_tCurrentMemberDirection.z >= 0.0) {
			fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tLeft));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[1])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[3])/255.f));
		}
		else {
			fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tLeft));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[3])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[1])/255.f));
		}
	else
		if (m_tCurrentMemberDirection.z >= 0.0) {
			fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tRight));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[1])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[3])/255.f));
		}
		else {
			fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tRight));
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[2])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentNode->cover[3])/255.f));
			fAlpha = PI/2 - fAlpha;
			m_fResult += fCoverFromLeaderWeight*(fAlpha/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[0])/255.f) + (PI/2 - fAlpha)/(PI/2)*(1.0 - float(m_tpCurrentMemberNode->cover[1])/255.f));
		}
#endif
}

IC void CHenSelectorBase::vfAddTotalCoverCost()
{
	m_fResult += fTotalCover*(float(m_tpCurrentNode->cover[0])/255.f + float(m_tpCurrentNode->cover[1])/255.f + float(m_tpCurrentNode->cover[2])/255.f + float(m_tpCurrentNode->cover[3])/255.f);
}

IC void CHenSelectorBase::vfAddEnemyLookCost()
{
	if (fabs(m_dwCurTime - m_dwHitTime) < ATTACK_HIT_REACTION_TIME) {
		Fvector tTempDirection0, tTempDirection1 = m_tHitDir;
		tTempDirection0.sub(m_tEnemyPosition,m_tCurrentPosition);
		vfNormalizeSafe(tTempDirection0);
		vfNormalizeSafe(tTempDirection1);
		float fAlpha = acosf(tTempDirection0.dotproduct(tTempDirection1));
		m_fResult += fEnemyViewDeviationWeight*(PI - fAlpha);
	}
}

IC void CHenSelectorBase::vfAssignMemberPositionAndNode()
{
#ifdef DEST_POSITIONS
	m_tCurrentMemberPosition = taDestMemberPositions[m_iCurrentMember];
	m_tpCurrentMemberNode = Level().AI.Node(taDestMemberNodes[m_iCurrentMember]);
#else
	m_tCurrentMemberPosition = taMemberPositions[m_iCurrentMember];
	m_tpCurrentMemberNode = Level().AI.Node(taMemberNodes[m_iCurrentMember]);
#endif
}

IC void CHenSelectorBase::vfComputeMemberDirection()
{
#ifdef DEST_POSITIONS
	m_tCurrentMemberDirection.sub(taDestMemberPositions[m_iCurrentMember],m_tCurrentPosition);
#else
	m_tCurrentMemberDirection.sub(taMemberPositions[m_iCurrentMember],m_tCurrentPosition);
#endif
}

IC void CHenSelectorBase::vfComputeSurroundEnemy()
{
#ifdef DEST_POSITIONS
	m_tEnemySurroundDirection.x += taDestMemberPositions[m_iCurrentMember].x - m_tCurrentPosition.x;
	m_tEnemySurroundDirection.y += taDestMemberPositions[m_iCurrentMember].y - m_tCurrentPosition.y;
	m_tEnemySurroundDirection.z += taDestMemberPositions[m_iCurrentMember].z - m_tCurrentPosition.z;
#else
	m_tEnemySurroundDirection.x += taMemberPositions[m_iCurrentMember].x - m_tCurrentPosition.x;
	m_tEnemySurroundDirection.y += taMemberPositions[m_iCurrentMember].y - m_tCurrentPosition.y;
	m_tEnemySurroundDirection.z += taMemberPositions[m_iCurrentMember].z - m_tCurrentPosition.z;
#endif
}

IC void CHenSelectorBase::vfAddSurroundEnemyCost()
{
	vfNormalizeSafe(m_tEnemySurroundDirection);
	m_fResult += fEnemySurround*sqrt(SQR(m_tEnemySurroundDirection.x) + SQR(m_tEnemySurroundDirection.y) + SQR(m_tEnemySurroundDirection.z));
}

IC void CHenSelectorBase::vfCheckForEpsilon(BOOL &bStop)
{
	if (m_fResult < EPS)
		bStop = TRUE;
}

float CHenSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_fResult = 0.f;
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	m_tEnemySurroundDirection.set(0,0,0);
	vfComputeCurrentPosition();
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
	// initialization
	m_fResult = 0.f;
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfComputeCurrentPosition();
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

CHenSelectorFollow::CHenSelectorFollow()
{ 
	Name = "selector_follow"; 
}

float CHenSelectorFollow::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_fResult = 0.f;
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfComputeCurrentPosition();
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

CHenSelectorPursuit::CHenSelectorPursuit()
{ 
	Name = "selector_pursuit"; 
}

float CHenSelectorPursuit::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_fResult = 0.f;
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	m_tEnemySurroundDirection.set(0,0,0);
	vfComputeCurrentPosition();
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

CHenSelectorUnderFire::CHenSelectorUnderFire()
{ 
	Name = "selector_under_fire"; 
}

float CHenSelectorUnderFire::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
{
	// initialization
	m_fResult = 0.f;
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfComputeCurrentPosition();
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
