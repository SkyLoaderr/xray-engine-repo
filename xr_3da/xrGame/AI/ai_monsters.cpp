////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters.cpp
//	Created 	: 24.04.2002
//  Modified 	: 24.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for all the monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_monsters.h"

//#define OLD_COVER_COST
#define DEST_POSITIONS
#define ATTACK_HIT_REACTION_TIME	30000

IC float ffGetDistance(Fvector P1, Fvector P2)
{
	return(float(sqrt(float(_sqr(P2.x - P1.x) + _sqr(P2.z - P1.z) + _sqr(P2.y - P1.y)))));
}

IC void vfNormalizeSafe(Fvector& Vector)
{
	float fMagnitude = Vector.magnitude(); 
	if (fMagnitude > MAGNITUDE_EPSILON) {
		Vector.x /= fMagnitude;
		Vector.y /= fMagnitude;
		Vector.z /= fMagnitude;
	}
	else {
		Vector.x = 1.f;
		Vector.y = 0.f;
		Vector.z = 0.f;
	}
}

void CAISelectorBase::Load	(LPCSTR section)
{
	sscanf(pSettings->ReadSTRING(section,Name),
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

void CAISelectorBase::vfAddTravelCost()
{
	m_fResult += m_fDistance*fTravelWeight;
}

void CAISelectorBase::vfAddLightCost()
{
	m_fResult += ((float)(m_tpCurrentNode->light)/255.f)*fLightWeight;
}

void CAISelectorBase::vfInit()
{
	m_fResult = 0.f;
	m_tEnemySurroundDirection.set(0,0,0);
	m_tCurrentPosition = Level().AI.tfGetNodeCenter(m_tpCurrentNode);
	m_iAliveMemberCount = taMemberPositions.size();
	m_fFireDispersionAngle = PI/10;
}

void CAISelectorBase::vfAddDistanceToEnemyCost()
{
	float fDistanceToEnemy = m_tCurrentPosition.distance_to(m_tEnemyPosition);
	if (fDistanceToEnemy < fMinEnemyDistance)
		m_fResult += fMinEnemyDistanceWeight*(fMinEnemyDistance + 0.1f)/(fDistanceToEnemy + 0.1f);
	else
		if (fDistanceToEnemy > fMaxEnemyDistance)
			m_fResult += fMaxEnemyDistanceWeight*(fDistanceToEnemy + 0.1f)/(fMaxEnemyDistance + 0.1f);
		else
			m_fResult += fOptEnemyDistanceWeight*fabsf(fDistanceToEnemy - fOptEnemyDistance + 1.f)/(fOptEnemyDistance + 1.f);
}

void CAISelectorBase::vfAddDistanceToLeaderCost()
{
	float fDistanceToLeader = m_tCurrentPosition.distance_to(m_tLeaderPosition);
	if (fDistanceToLeader < fMinLeaderDistance)
		m_fResult += fMinLeaderDistanceWeight*(fMinLeaderDistance + 0.1f)/(fDistanceToLeader + 0.1f);
	else
		if (fDistanceToLeader > fMaxLeaderDistance)
			m_fResult += fMaxLeaderDistanceWeight*(fDistanceToLeader + 0.1f)/(fMaxLeaderDistance + 0.1f);
		else
			m_fResult += fOptLeaderDistanceWeight*fabsf(fDistanceToLeader - fOptLeaderDistance + 1.f)/(fOptLeaderDistance + 1.f);
}

void CAISelectorBase::vfAddDistanceToMemberCost()
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
			m_fResult += fOptMemberDistanceWeight*fabsf(fDistanceToMember - fOptMemberDistance + 1.f)/(fOptMemberDistance + 1.f)/m_iAliveMemberCount;
}

void CAISelectorBase::vfAddCoverFromEnemyCost()
{
	if (m_tpEnemyNode) {
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
}

void CAISelectorBase::vfAddCoverFromLeaderCost()
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

void CAISelectorBase::vfAddCoverFromMemberCost()
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

void CAISelectorBase::vfAddTotalCoverCost()
{
	m_fResult += fTotalCover*(float(m_tpCurrentNode->cover[0])/255.f + float(m_tpCurrentNode->cover[1])/255.f + float(m_tpCurrentNode->cover[2])/255.f + float(m_tpCurrentNode->cover[3])/255.f);
}

void CAISelectorBase::vfAddEnemyLookCost()
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

void CAISelectorBase::vfAddEnemyLookCost(float fAngle)
{
	if (fabs(m_dwCurTime - m_dwHitTime) < ATTACK_HIT_REACTION_TIME) {
		Fvector tTempDirection0, tTempDirection1 = m_tHitDir;
		tTempDirection0.sub(m_tEnemyPosition,m_tCurrentPosition);
		vfNormalizeSafe(tTempDirection0);
		vfNormalizeSafe(tTempDirection1);
		float fAlpha = acosf(tTempDirection0.dotproduct(tTempDirection1));
		m_fResult += fEnemyViewDeviationWeight*fabsf(fAngle - fAlpha);
	}
}

void CAISelectorBase::vfAssignMemberPositionAndNode()
{
#ifdef DEST_POSITIONS
	m_tCurrentMemberPosition = taDestMemberPositions[m_iCurrentMember];
	m_tpCurrentMemberNode = Level().AI.Node(taDestMemberNodes[m_iCurrentMember]);
#else
	m_tCurrentMemberPosition = taMemberPositions[m_iCurrentMember];
	m_tpCurrentMemberNode = Level().AI.Node(taMemberNodes[m_iCurrentMember]);
#endif
}

void CAISelectorBase::vfComputeMemberDirection()
{
#ifdef DEST_POSITIONS
	m_tCurrentMemberDirection.sub(taDestMemberPositions[m_iCurrentMember],m_tCurrentPosition);
#else
	m_tCurrentMemberDirection.sub(taMemberPositions[m_iCurrentMember],m_tCurrentPosition);
#endif
}

void CAISelectorBase::vfComputeSurroundEnemy()
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

void CAISelectorBase::vfAddSurroundEnemyCost()
{
	vfNormalizeSafe(m_tEnemySurroundDirection);
	m_fResult += fEnemySurround*m_tEnemySurroundDirection.magnitude();
}

void CAISelectorBase::vfCheckForEpsilon(BOOL &bStop)
{
	if (m_fResult < EPS)
		bStop = TRUE;
}

void CAISelectorBase::vfAddDeviationFromMemberViewCost()
{
	Fvector tTempDirection0, tTempDirection1;
	tTempDirection0.sub(m_tEnemyPosition,m_tCurrentMemberPosition);
	tTempDirection1.sub(m_tCurrentPosition,m_tCurrentMemberPosition);
	vfNormalizeSafe(tTempDirection0);
	vfNormalizeSafe(tTempDirection1);
	float fAlpha = tTempDirection0.dotproduct(tTempDirection1);
	clamp(fAlpha,-0.999999f,0.999999f);
	fAlpha = acosf(fAlpha);
	if (fAlpha < m_fFireDispersionAngle)
		m_fResult += fMemberViewDeviationWeight;
}

void CAISelectorBase::vfAddDeviationFromPreviousDirectionCost()
{
	Fvector tTempDirection;
	tTempDirection.sub(m_tMyPosition,m_tCurrentPosition);
	vfNormalizeSafe(tTempDirection);
	float fAlpha = tTempDirection.dotproduct(m_tDirection);
	clamp(fAlpha,-0.99999f,0.99999f);
	fAlpha = acosf(fAlpha);
	m_fResult += fAlpha * fMemberViewDeviationWeight;
}

void CAISelectorBase::vfAddDistanceToLastPositionCost()
{
	Fvector tTemp;
	tTemp.sub(m_tCurrentPosition,m_tLastEnemyPosition);
	m_fResult += 20.f*tTemp.magnitude();
}
