////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.h
//	Created 	: 23.04.2002
//  Modified 	: 23.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_RAT_SELECTORS__
#define __XRAY_AI_RAT_SELECTORS__

class CRatSelectorBase : public AI::NodeEstimator 
{
	public:
		
		// section name
		LPSTR			Name;
		
		// .ini file constants
		float		
						fSearchRange,
						fEnemySurround,
						fTotalCover,
						fTravelWeight,
						fLightWeight,
						fLaziness,
						fTotalViewVectorWeight,
						fCoverFromLeaderWeight,
						fOptLeaderDistance,
						fOptLeaderDistanceWeight,
						fMinLeaderDistance,
						fMinLeaderDistanceWeight,
						fMaxLeaderDistance,
						fMaxLeaderDistanceWeight,
						fLeaderViewDeviationWeight,
						fMaxLeaderHeightDistance,
						fMaxLeaderHeightDistanceWeight,
						fCoverFromMemberWeight,
						fOptMemberDistance,
						fOptMemberDistanceWeight,
						fMinMemberDistance,
						fMinMemberDistanceWeight,
						fMaxMemberDistance,
						fMaxMemberDistanceWeight,
						fMemberViewDeviationWeight,
						fCoverFromEnemyWeight,
						fOptEnemyDistance,
						fOptEnemyDistanceWeight,
						fMinEnemyDistance,
						fMinEnemyDistanceWeight,
						fMaxEnemyDistance,
						fMaxEnemyDistanceWeight,
						fEnemyViewDeviationWeight;
		
		// hit information
		Fvector			m_tHitDir;
		DWORD			m_dwHitTime;
		
		// current time information
		DWORD			m_dwCurTime;

		// myself
		CEntity*        m_tMe;
		NodeCompressed* m_tpMyNode;
		Fvector			m_tMyPosition;
		
		// leader
		CEntity*        m_tLeader;
		NodeCompressed* m_tpLeaderNode;
		Fvector			m_tLeaderPosition;
		Fvector			m_tLeaderDirection;
		DWORD			m_tLeaderNode;

		// enemy
		CEntity*        m_tEnemy;
		NodeCompressed* m_tpEnemyNode;
		Fvector			m_tEnemyPosition;
		Fvector			m_tEnemyDirection;
		Fvector			m_tEnemySurroundDirection;
		
		// postion being tested
		float			m_fDistance;
		NodeCompressed*	m_tpCurrentNode;
		Fvector			m_tCurrentPosition;

		// members
		int				m_iAliveMemberCount;
		int				m_iCurrentMember;
		NodeCompressed*	m_tpCurrentMemberNode;
		Fvector			m_tCurrentMemberPosition;
		Fvector			m_tCurrentMemberDirection;

		// common
		float			m_fResult;

		virtual	void Load(CInifile* ini, const char* section);
		virtual void Init();
		
		IC void vfAddTravelCost();
		IC void vfAddLightCost();
		IC void vfComputeCurrentPosition();
		IC void vfAssignMemberPositionAndNode();
		IC void vfAddDistanceToEnemyCost();
		IC void vfAddDistanceToLeaderCost();
		IC void vfAddDistanceToMemberCost();
		IC void vfAddCoverFromEnemyCost();
		IC void vfAddCoverFromLeaderCost();
		IC void vfAddCoverFromMemberCost();
		IC void vfAddCoverFromSavedEnemyCost();
		IC void vfAddTotalCoverCost();
		IC void vfAddEnemyLookCost();
		IC void vfComputeMemberDirection();
		IC void vfComputeSurroundEnemy();
		IC void vfAddSurroundEnemyCost();
		IC void vfCheckForEpsilon(BOOL &bStop);
};

class CRatSelectorAttack : public CRatSelectorBase
{
	public:
		CRatSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorFreeHunting : public CRatSelectorBase
{
	public:
		CRatSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorFollow : public CRatSelectorBase
{
	public:
		CRatSelectorFollow	();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

class CRatSelectorPursuit : public CRatSelectorBase
{
	public:
		CRatSelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CRatSelectorUnderFire : public CRatSelectorBase
{
	public:
		CRatSelectorUnderFire();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif